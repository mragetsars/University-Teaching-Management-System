# Optional State Persistence

The original assignment initializes data from CSV files and keeps runtime changes in memory. For showcase and local development, UTMS can optionally persist runtime state.

## Usage

```bash
./out.utms data/data_majors.csv data/data_students.csv data/data_courses.csv data/data_professors.csv --web --state state/local.utms-state
```

or:

```bash
UTMS_STATE_FILE=state/local.utms-state ./out.utms data/data_majors.csv data/data_students.csv data/data_courses.csv data/data_professors.csv --web
```

## Stored data

The state file stores a human-readable snapshot of runtime changes, including:

- profile photos
- personal posts
- connections
- pending notifications
- course offerings
- enrollments
- teaching assistants
- channel posts
- open TA forms and requests

The feature is optional and does not change the CSV-based grading workflow.
