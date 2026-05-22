# CLI Command Reference

The CLI remains compatible with the original AP assignment format.

```bash
./out.utms data/data_majors.csv data/data_students.csv data/data_courses.csv data/data_professors.csv --cli
```

Commands follow this shape:

```text
<METHOD> <command> ? <key> <value> <key> <value> ...
```

Arguments after `?` are order-independent. Text values may be wrapped in double quotes.

## Core examples

```text
POST login ? id 810102612 password ImtheproblemItsme
GET courses ?
POST post ? title "Hello" message "My first post"
POST connect ? id 810102342
GET notification ?
POST logout ?
```

## Manager example

```text
POST login ? id 0 password UT_account
POST course_offer ? course_id 1 professor_id 810420432 capacity 70 time Sunday:13-15 exam_date 1403/4/4 class_number 2
POST logout ?
```

## Student example

```text
POST login ? id 810102612 password ImtheproblemItsme
PUT my_courses ? id 1
GET my_courses ?
DELETE my_courses ? id 1
```

## Professor / TA workflow example

```text
POST login ? id 810420432 password halalalaylalalay
POST ta_form ? course_id 1 message "TA for project support"
POST close_ta_form ? id 1
```
