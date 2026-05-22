#include "server.hpp"

#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <vector>

#include "strutils.hpp"
#include "utilities.hpp"

#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501 // win xp
#endif
#include <Ws2tcpip.h>
#include <winsock2.h>
#else
// POSIX sockets
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h> //close()
#endif

#ifdef _WIN32
#define ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
#define CLOSESOCKET(s)   closesocket(s)
#define GETSOCKETERRNO() (WSAGetLastError())
#else
#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s)   close(s)
#define GETSOCKETERRNO() (errno)
#endif

#ifndef _WIN32
#ifdef MSG_NOSIGNAL
#define UTMS_SEND_FLAGS MSG_NOSIGNAL
#else
#define UTMS_SEND_FLAGS 0
#endif
#else
#define UTMS_SEND_FLAGS 0
#endif

static const char* getSocketError() {
#ifdef _WIN32
    static char message[256];
    message[0] = '\0';
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL, WSAGetLastError(), 0, (LPSTR)&message, sizeof(message),
                   NULL);
    char* newline = strrchr(message, '\n');
    if (newline)
        *newline = '\0';
    return message;
#else
    return strerror(errno);
#endif
}

using namespace std;

class NotFoundHandler : public RequestHandler {
public:
    NotFoundHandler(const string& notFoundErrPage = "")
        : notFoundErrPage_(notFoundErrPage) {}
    Response* callback(Request* req) {
        Response* res = new Response(Response::Status::notFound);
        if (!notFoundErrPage_.empty()) {
            res->setHeader("Content-Type", "text/" + utils::getExtension(notFoundErrPage_));
            res->setBody(utils::readFile(notFoundErrPage_));
        }
        return res;
    }

private:
    string notFoundErrPage_;
};

class ServerErrorHandler {
public:
    static Response* callback(const string& msg) {
        Response* res = new Response(Response::Status::internalServerError);
        res->setHeader("Content-Type", "application/json");
        res->setBody("{ \"code\": \"500\", \"message\": \"" + msg + "\" }\n");
        return res;
    }
};

static bool splitKeyValue(const string& input, string& key, string& value, bool valueMayBeEmpty) {
    size_t pos = input.find('=');
    if (pos == string::npos) {
        if (!valueMayBeEmpty) return false;
        key = input;
        value.clear();
        return true;
    }
    key = input.substr(0, pos);
    value = input.substr(pos + 1);
    return !key.empty();
}

static bool splitHeaderLine(const string& input, string& key, string& value) {
    size_t pos = input.find(':');
    if (pos == string::npos || pos == 0) return false;
    key = input.substr(0, pos);
    value = input.substr(pos + 1);
    while (!value.empty() && value.front() == ' ') value.erase(value.begin());
    return true;
}

static bool httpLogEnabled() {
    const char* value = std::getenv("UTMS_HTTP_LOG");
    if (value == nullptr) {
        return true;
    }
    std::string normalized = strutils::tolower(value);
    return !(normalized == "0" || normalized == "false" || normalized == "off" || normalized == "no");
}

static void setSocketIoTimeout(SOCKET socketFd, int seconds) {
#ifdef _WIN32
    DWORD timeoutMs = static_cast<DWORD>(seconds * 1000);
    setsockopt(socketFd, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&timeoutMs), sizeof(timeoutMs));
    setsockopt(socketFd, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char*>(&timeoutMs), sizeof(timeoutMs));
#else
    struct timeval timeout;
    timeout.tv_sec = seconds;
    timeout.tv_usec = 0;
    setsockopt(socketFd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    setsockopt(socketFd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
#endif
}

static void sendAll(SOCKET socketFd, const std::string& data) {
    size_t sent = 0;
    while (sent < data.size()) {
        int chunk = static_cast<int>(std::min<size_t>(data.size() - sent, INT_MAX));
        int written = send(socketFd, data.data() + sent, chunk, UTMS_SEND_FLAGS);
        if (written <= 0) {
            throw Server::Exception("Send error: " + string(getSocketError()));
        }
        sent += static_cast<size_t>(written);
    }
}

Request* parseRawReq(char* reqData, size_t length) {
    std::unique_ptr<Request> req;
    string boundary;
    string lastFieldKey;
    string lastFieldValue;
    string reqDataStr(reqData, reqData + length);
    try {
        size_t endOfHeader = reqDataStr.find("\r\n\r\n");
        if (endOfHeader == string::npos) {
            return nullptr;
        }
        string reqHeader = reqDataStr.substr(0, endOfHeader);
        string reqBody = reqDataStr.substr(endOfHeader + 4);
        vector<string> headers = strutils::split(reqHeader, "\r\n");
        if (reqHeader.find('\0') != string::npos) {
            throw Server::Exception("Binary data in header.");
        }
        size_t realBodySize = length - endOfHeader - 4; // string("\r\n\r\n").size();

        vector<string> R = strutils::split(headers[0], ' ');
        if (R.size() != 3) {
            throw Server::Exception("Invalid header (request line)");
        }
        if (!(R[0] == "GET" || R[0] == "POST" || R[0] == "PUT" || R[0] == "DELETE" || R[0] == "DEL"))
            throw Server::Exception("Unsupported HTTP method: " + R[0]);
        if (R[1].empty() || R[1][0] != '/' || R[1].find("..") != string::npos)
            throw Server::Exception("Invalid request path.");
        req = std::make_unique<Request>(R[0]);
        req->setPath(R[1]);
        size_t pos = req->getPath().find('?');
        if (pos != string::npos && pos != req->getPath().size() - 1) {
            vector<string> Q1 = strutils::split(req->getPath().substr(pos + 1), '&');
            for (const string& queryPart : Q1) {
                string key, value;
                if (!splitKeyValue(queryPart, key, value, false))
                    throw Server::Exception("Invalid query");
                req->setQueryParam(key, value, false);
            }
        }
        req->setPath(req->getPath().substr(0, pos));

        for (size_t headerIndex = 1; headerIndex < headers.size(); headerIndex++) {
            string key, value;
            if (!splitHeaderLine(headers[headerIndex], key, value))
                throw Server::Exception("Invalid header");
            req->setHeader(key, value, false);
            if (strutils::tolower(key) == strutils::tolower("Content-Length"))
            {
                size_t declaredBodySize = 0;
                try
                {
                    declaredBodySize = static_cast<size_t>(stoull(value));
                }
                catch (...)
                {
                    throw Server::Exception("Invalid Content-Length header.");
                }
                if (declaredBodySize > BUFSIZE)
                    throw Server::Exception("Request body is too large.");
                if (realBodySize != declaredBodySize)
                    return nullptr;
            }
        }

        string contentType = req->getHeader("Content-Type");
        if (realBodySize != 0 && !contentType.empty()) {
            if (strutils::startsWith(contentType, "application/x-www-form-urlencoded")) {
                vector<string> urlencodedParts = strutils::split(reqBody, "\r\n");
                for (const string& part : urlencodedParts) {
                    vector<string> body = strutils::split(part, '&');
                    for (const string& bodyPart : body) {
                        string key, value;
                        if (!splitKeyValue(bodyPart, key, value, true))
                            throw Server::Exception("Invalid body");
                        req->setBodyParam(key, value, "application/x-www-form-urlencoded", false);
                    }
                }
            }
            else if (strutils::startsWith(contentType, "multipart/form-data")) {
                size_t boundaryStart = contentType.find("boundary=");
                if (boundaryStart == string::npos)
                    throw Server::Exception("Boundary attribute not found.");
                boundary = contentType.substr(boundaryStart + 9);
                if (boundary.empty())
                    throw Server::Exception("Empty multipart boundary.");
                size_t firstBoundary = reqBody.find("--" + boundary);
                if (firstBoundary == string::npos) {
                    throw Server::Exception("Boundary data not found.");
                }
                reqBody.erase(reqBody.begin(), reqBody.begin() + firstBoundary + 2 + boundary.size());

                vector<string> boundaries = strutils::split(reqBody, "--" + boundary);
                if (!boundaries.empty())
                    boundaries.pop_back();

                for (string b : boundaries) {
                    if (b.size() >= 2 && b.substr(0, 2) == "\r\n")
                        b.erase(0, 2);
                    if (b.size() >= 2 && b.substr(b.size() - 2) == "\r\n")
                        b.erase(b.size() - 2);
                    string boundaryContentType = "text/plain";
                    lastFieldKey.clear();

                    size_t separator = b.find("\r\n\r\n");
                    if (separator == string::npos)
                        throw Server::Exception("Invalid multipart boundary.");
                    size_t endOfBoundaryHeader = separator + 4;
                    vector<string> abc = strutils::split(b.substr(0, separator), "\r\n");
                    for (const string& line : abc) {
                        if (line.empty()) {
                            break;
                        }
                        string key, value;
                        if (!splitHeaderLine(line, key, value)) throw Server::Exception("Invalid header");
                        if (strutils::tolower(key) == strutils::tolower("Content-Disposition")) {
                            vector<string> A = strutils::split(value, "; ");
                            for (const string& attrText : A) {
                                string attrKey, attrValue;
                                if (splitKeyValue(attrText, attrKey, attrValue, false)) {
                                    if (strutils::tolower(attrKey) == strutils::tolower("name") && attrValue.size() >= 2) {
                                        lastFieldKey = attrValue.substr(1, attrValue.size() - 2);
                                    }
                                }
                            }
                        }
                        else if (strutils::tolower(key) == strutils::tolower("Content-Type")) {
                            boundaryContentType = strutils::tolower(value);
                        }
                    }
                    if (lastFieldKey.empty())
                        throw Server::Exception("Multipart field name missing.");
                    lastFieldValue = b.substr(endOfBoundaryHeader);
                    req->setBodyParam(lastFieldKey, lastFieldValue, boundaryContentType, false);
                }
            }
            else {
                throw Server::Exception("Unsupported body type: " + contentType);
            }
        }
    }
    catch (const Server::Exception&) {
        throw;
    }
    catch (const std::exception& e) {
        throw Server::Exception("Error on parsing request: " + std::string(e.what()));
    }
    return req.release();
}

Server::Server(int port) : port_(port) {
#ifdef _WIN32
    WSADATA wsa_data;
    int initializeResult = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (initializeResult != 0) {
        throw Exception("Error: WinSock WSAStartup failed: " +
                        string(getSocketError()));
    }
#endif
#ifndef _WIN32
    signal(SIGPIPE, SIG_IGN);
#endif

    notFoundHandler_ = new NotFoundHandler();

    sc_ = socket(AF_INET, SOCK_STREAM, 0);
    int sc_option = 1;

#ifdef _WIN32
    setsockopt(sc_, SOL_SOCKET, SO_REUSEADDR, (char*)&sc_option, sizeof(sc_option));
#else
    setsockopt(sc_, SOL_SOCKET, SO_REUSEADDR, &sc_option, sizeof(sc_option));
#endif
    if (!ISVALIDSOCKET(sc_)) {
        throw Exception("Error on opening socket: " + string(getSocketError()));
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port_);

    if (::bind(sc_, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) != 0) {
        throw Exception("Error on binding: " + string(getSocketError()));
    }
}

void Server::mapRequest(const string& path, RequestHandler* handler, Request::Method method) {
    Route* route = new Route(method, path);
    route->setHandler(handler);
    routes_.push_back(route);
}

void Server::get(const std::string& path, RequestHandler* handler) {
    mapRequest(path, handler, Request::Method::GET);
}

void Server::post(const std::string& path, RequestHandler* handler) {
    mapRequest(path, handler, Request::Method::POST);
}

void Server::put(const std::string& path, RequestHandler* handler) {
    mapRequest(path, handler, Request::Method::PUT);
}

void Server::del(const std::string& path, RequestHandler* handler) {
    mapRequest(path, handler, Request::Method::DEL);
}

void Server::run() {
    ::listen(sc_, 10);

    struct sockaddr_in cli_addr;
    socklen_t clilen;
    clilen = sizeof(cli_addr);
    SOCKET newsc;

    while (true) {
        newsc = ::accept(sc_, (struct sockaddr*)&cli_addr, &clilen);
        if (!ISVALIDSOCKET(newsc))
            throw Exception("Error on accept: " + string(getSocketError()));
        setSocketIoTimeout(newsc, 5);
        std::unique_ptr<Response> res;
        try {
            std::vector<char> data(BUFSIZE + 1, 0);
            size_t recv_total_len = 0;
            std::unique_ptr<Request> req;
            while (!req) {
                if (recv_total_len >= BUFSIZE)
                    throw Exception("Request is too large.");
                ssize_t recv_len = recv(newsc, data.data() + recv_total_len, BUFSIZE - recv_total_len, 0);
                if (recv_len > 0) {
                    recv_total_len += static_cast<size_t>(recv_len);
                    data[recv_total_len] = 0;
                    req.reset(parseRawReq(data.data(), recv_total_len));
                }
                else if (recv_len == 0) {
                    break;
                }
                else {
#ifdef _WIN32
                    if (GETSOCKETERRNO() == WSAETIMEDOUT && recv_total_len == 0) {
                        break;
                    }
#else
                    if ((GETSOCKETERRNO() == EAGAIN || GETSOCKETERRNO() == EWOULDBLOCK) && recv_total_len == 0) {
                        break;
                    }
#endif
                    throw Exception("Receive error: " + string(getSocketError()));
                }
            }
            if (!recv_total_len) {
                CLOSESOCKET(newsc);
                continue;
            }
            if (!req) {
                throw Exception("Incomplete HTTP request.");
            }
            if (httpLogEnabled())
                req->log();
            size_t i = 0;
            for (; i < routes_.size(); i++) {
                if (routes_[i]->isMatch(req->getMethod(), req->getPath())) {
                    res.reset(routes_[i]->handle(req.get()));
                    break;
                }
            }
            if (i == routes_.size() && notFoundHandler_) {
                res.reset(notFoundHandler_->callback(req.get()));
            }
        }
        catch (const Exception& exc) {
            res.reset(ServerErrorHandler::callback(exc.getMessage()));
        }
        if (!res)
            res.reset(ServerErrorHandler::callback("No response generated."));
        if (httpLogEnabled())
            res->log();
        string res_data = res->getResponse();
        try {
            sendAll(newsc, res_data);
        } catch (const Exception& exc) {
            if (httpLogEnabled())
                std::clog << "HTTP send failed: " << exc.getMessage() << std::endl;
        }
        CLOSESOCKET(newsc);
    }
}

Server::~Server() {
    if (sc_ >= 0) {
        CLOSESOCKET(sc_);
    }
    delete notFoundHandler_;
    for (size_t i = 0; i < routes_.size(); ++i) {
        delete routes_[i];
    }
#ifdef _WIN32
    WSACleanup();
#endif
}

Server::Exception::Exception(const string message) : message_(message) {}

string Server::Exception::getMessage() const { return message_; }

ShowFile::ShowFile(const string& filePath, const string& fileType)
    : filePath_(filePath),
      fileType_(fileType) {}

Response* ShowFile::callback(Request* req) {
    Response* res = new Response();
    res->setHeader("Content-Type", fileType_);
    res->setBody(utils::readFile(filePath_));
    return res;
}

ShowPage::ShowPage(const string& filePath)
    : ShowFile(filePath, "text/" + utils::getExtension(filePath)) {}

ShowImage::ShowImage(const string& filePath)
    : ShowFile(filePath, "image/" + utils::getExtension(filePath)) {}

void Server::setNotFoundErrPage(const std::string& notFoundErrPage) {
    delete notFoundHandler_;
    notFoundHandler_ = new NotFoundHandler(notFoundErrPage);
}

RequestHandler::~RequestHandler() {}

TemplateHandler::TemplateHandler(const string& filePath)
    : filePath_(filePath),
      parser_(new TemplateParser(filePath)) {}

TemplateHandler::~TemplateHandler() {
    delete parser_;
}

Response* TemplateHandler::callback(Request* req) {
    map<string, string> context;
    context = this->handle(req);
    Response* res = new Response();
    res->setHeader("Content-Type", "text/html");
    res->setBody(parser_->getHtml(context));
    return res;
}

map<string, string> TemplateHandler::handle(Request* req) {
    map<string, string> context;
    return context;
}
