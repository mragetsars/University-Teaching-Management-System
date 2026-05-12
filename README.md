# University Teaching Management System (UTMS)

> **Advanced Programming (AP) – University of Tehran – Department of Computer Engineering**

![Language](https://img.shields.io/badge/Language-C%2B%2B20-orange)
![Tool](https://img.shields.io/badge/Tool-g%2B%2B-blue)
![Interface](https://img.shields.io/badge/Interface-CLI%20%2B%20Web-8A2BE2)
![Status](https://img.shields.io/badge/Status-Completed-success)
![License](https://img.shields.io/badge/License-MIT-green)

## Overview

This repository contains the source code for **UTMS**, a university teaching management and social-media platform implemented in C++20. It was developed as the **Sixth Computer Assignment** for the **Advanced Programming** course at the University of Tehran.

The system combines a simplified educational management service with a social network. Students, professors, and the default university account can authenticate, publish posts, connect with each other, receive notifications, manage course offerings, enroll in courses, use teaching-assistant workflows, and interact with course announcement channels. The project also includes a lightweight HTTP web interface powered by the provided APHTTP-style server library.

## Project Objectives

- ✅ **University course management:** Create course offerings, enforce prerequisites, verify major eligibility, prevent schedule/exam conflicts, and manage student enrollment.
- ✅ **Social-media layer:** Publish personal posts, connect users bidirectionally, browse personal pages, and receive chronological notifications.
- ✅ **Teaching-assistant workflow:** Professors can publish TA forms, students can submit requests, and professors can accept or reject applicants interactively.
- ✅ **Course announcement channels:** Each course offering has a dedicated channel where professors and accepted TAs can post announcements for enrolled users.
- ✅ **Web user interface:** A browser-accessible dashboard and command console are available for interacting with the same application logic.
- ✅ **Clean C++ design:** The codebase is organized as a multi-file C++20 project with domain classes, CSV input handling, terminal parsing, and web handlers separated from the core model.

## System Architecture & Phases

The project is implemented around three development phases matching the original assignment series.

### 1️⃣ Phase 1: Core UTMS Engine

The first phase implements the command-line core for students, professors, and the default university account.

- Users are loaded from CSV datasets.
- Professors can be assigned to course offerings by the system account.
- Students can enroll in and drop course offerings after eligibility checks.
- Users can post on their personal page, connect to other users, and consume notifications.

### 2️⃣ Phase 2: Extended Academic Features

The second phase extends the core engine with course channels, teaching-assistant forms, and profile/post image addresses.

- Professors can publish TA forms for their own course offerings.
- Eligible students can request to become teaching assistants.
- Accepted TAs gain access to course announcement channels.
- Course-channel posts notify the professor, TAs, and enrolled students, excluding the author.
- Profile-photo and post-image addresses are stored for use by the web layer.

### 3️⃣ Phase 3: Web Interface

The third phase adds a lightweight HTTP interface on top of the same command engine.

- `/` provides the login page.
- `/home` shows the dashboard for the currently logged-in user.
- `/console` exposes a CLI-compatible web command console for all supported UTMS commands.
- `/logout` signs out the current user.

The web interface intentionally delegates to the same `Instruction_Handler` and parser used by the terminal version, keeping the business logic centralized and avoiding duplicate behavior.

## Repository Structure

```text
UTMS/
├── APS03-A6.1-Description.pdf      # Phase 1 assignment description
├── APS03-A6.2-Description.pdf      # Phase 2 assignment description
├── APS03-A6.3-Description.pdf      # Phase 3 assignment description
├── data_courses.csv                # Sample course dataset
├── data_majors.csv                 # Sample major dataset
├── data_professors.csv             # Sample professor dataset
├── data_students.csv               # Sample student dataset
├── includes/                       # Header files
│   ├── class_class.hpp             # Course offering and channel model
│   ├── class_professor.hpp         # Professor and TA form logic
│   ├── class_student.hpp           # Student enrollment logic
│   ├── class_systemmanager.hpp     # Default university account
│   ├── class_user.hpp              # Shared user/post/notification behavior
│   ├── handler_instruction.hpp     # Command dispatcher and business rules
│   ├── io_csv.hpp                  # CSV loading interface
│   ├── io_terminal.hpp             # CLI parsing/output interface
│   ├── server.hpp                  # APHTTP-style server interface
│   └── web_handlers.hpp            # Web request handlers
├── sources/                        # C++20 implementation files
│   ├── main.cpp                    # CLI/Web startup mode selection
│   ├── handler_instruction.cpp     # UTMS command execution
│   ├── io_terminal.cpp             # Terminal command parser
│   ├── io_csv.cpp                  # Dataset reader
│   └── ...
├── utils/                          # Original APHTTP utility mirror
├── web/                            # Static web assets and HTML pages
│   ├── UTMS.png
│   ├── login.html
│   ├── home.html
│   └── ...
├── Makefile                        # Build configuration
├── LICENSE                         # MIT License
└── README.md                       # Project documentation
```

## Setup & Verification

Built with **C++20** for Linux environments.

**1. Install dependencies**

Make sure `g++` and `make` are installed. On Ubuntu:

```bash
sudo apt-get update
sudo apt-get install build-essential make
```

**2. Build**

```bash
make clean && make
```

The executable is generated as:

```bash
./utms.out
```

**3. Run in terminal mode**

```bash
./utms.out data_majors.csv data_students.csv data_courses.csv data_professors.csv --cli
```

You can also pipe commands directly:

```bash
cat _input.txt | ./utms.out data_majors.csv data_students.csv data_courses.csv data_professors.csv --cli
```

**4. Run in web mode**

```bash
./utms.out data_majors.csv data_students.csv data_courses.csv data_professors.csv --web
```

Then open:

```text
http://localhost:5000
```

## Command Examples

```text
POST login ? id 0 password UT_account
POST course_offer ? course_id 1 professor_id 810420432 capacity 70 time Sunday:13-15 exam_date 1403/4/4 class_number 2
POST logout ?

POST login ? id 810102612 password ImtheproblemItsme
GET courses ?
PUT my_courses ? id 1
POST post ? title "Hello UTMS" message "Advanced Programming project completed"
GET notification ?
```

## Implementation Notes

- Command arguments are order-independent after `?`, as required by the assignment.
- Personal posts and course posts are displayed newest-first where the specification requires timeline ordering.
- Course offerings are stored in a `deque` so object addresses remain stable when new offerings are added.
- Generated build artifacts are ignored through `.gitignore`; rebuild with `make` whenever needed.
- The default university account is:

```text
id: 0
password: UT_account
```

## Author

This project was developed for the **Advanced Programming** course at the **University of Tehran**.

- **[Meraj Rastegar](https://github.com/mragetsars)**

## License

This project is licensed under the **MIT License**. See [`LICENSE`](./LICENSE) for details.
