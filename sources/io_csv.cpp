#include "io_csv.hpp"

vector<string> split(const string &s, char delimiter)
{
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter))
        tokens.push_back(token);
    return tokens;
}

template <typename Struct>
void data_extraction(const string &filename, vector<Struct> &vec, void (*Struct_extraction)(vector<string> &, Struct &))
{
    ifstream file(filename);
    string line;
    getline(file, line);
    while (getline(file, line))
    {
        if (line.empty())
            continue;
        vector<string> tokens = split(line, COMMA);
        Struct item;
        Struct_extraction(tokens, item);
        vec.push_back(item);
    }
}

void major_extraction(vector<string> &tokens, MAJOR &major)
{
    major.id = tokens[0];
    major.name = tokens[1];
}

void student_extraction(vector<string> &tokens, STUDENT &student)
{
    student.info.id = tokens[0];
    student.info.name = tokens[1];
    student.info.major_id = tokens[2];
    student.semester = str_to_int(tokens[3]);
    student.info.password = tokens[4];
}

void course_extraction(std::vector<string> &tokens, COURSE &course)
{
    course.id = tokens[0];
    course.name = tokens[1];
    course.credit = str_to_int(tokens[2]);
    course.prerequisite = str_to_int(tokens[3]);
    course.major_ids = split(tokens[4], SEMICOLON);
}

void professor_extraction(vector<string> &tokens, PROFESSOR &professor)
{
    professor.info.id = tokens[0];
    professor.info.name = tokens[1];
    professor.info.major_id = tokens[2];
    professor.position_name = tokens[3];
    professor.position = str_to_pos(tokens[3]);
    professor.info.password = tokens[4];
}

DATA read_csv(char *majors_csv_address, char *students_csv_address,
              char *courses_csv_address, char *professors_csv_address)
{
    DATA output;

    output.system_manager.id = SYSTEM_MANAGER_ID;
    output.system_manager.major_id = SYSTEM_MANAGER_MAJOR_ID;
    output.system_manager.name = SYSTEM_MANAGER_NAME;
    output.system_manager.password = SYSTEM_MANAGER_PASSWORD;

    data_extraction(majors_csv_address, output.majors, major_extraction);
    data_extraction(students_csv_address, output.students, student_extraction);
    data_extraction(courses_csv_address, output.courses, course_extraction);
    data_extraction(professors_csv_address, output.professors, professor_extraction);

    return output;
}
