CXX = g++
CXXFLAGS = -std=c++20 -I./includes -Wall -Wextra -pedantic
SRCDIR = sources
OBJDIR = objects
BINDIR = .
EXECUTABLE = $(BINDIR)/utms.out

SOURCES := $(wildcard $(SRCDIR)/*.cpp)
OBJECTS := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))
HEADERS := $(wildcard includes/*.hpp)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS)
	mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run-cli: all
	./utms.out data_majors.csv data_students.csv data_courses.csv data_professors.csv --cli

run-web: all
	./utms.out data_majors.csv data_students.csv data_courses.csv data_professors.csv --web

clean:
	rm -rf $(OBJDIR) $(EXECUTABLE)

.PHONY: all clean run-cli run-web
