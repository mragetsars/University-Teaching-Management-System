#pragma once

#include "global.hpp"
#include "struct_data.hpp"

    vector<string> split(const string &s, char delimiter);

    template <typename Struct>
    void data_extraction(const string &filename, vector<Struct> &vec, void (*fillStruct)(vector<string> &, Struct &));

    void major_extraction(vector<string> &tokens, MAJOR &major);
    void student_extraction(vector<string> &tokens, STUDENT &student);
    void course_extraction(std::vector<string> &tokens, COURSE &course);
    void professor_extraction(vector<string> &tokens, PROFESSOR &professor);

    DATA read_csv(char *majors_csv_address, char *students_csv_address,
                 char *courses_csv_address, char *professors_csv_address);