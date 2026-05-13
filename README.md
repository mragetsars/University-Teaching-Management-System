# University Teaching Management System (UTMS)

> **Advanced Programming (AP) – University of Tehran – Department of Electrical & Computer Engineering**

![Language](https://img.shields.io/badge/Language-C%2B%2B20-orange)
![Tool](https://img.shields.io/badge/Tool-g%2B%2B-blue)
![Interface](https://img.shields.io/badge/Interface-CLI%20%2B%20Web-8A2BE2)
![Status](https://img.shields.io/badge/Status-Completed-success)
![License](https://img.shields.io/badge/License-MIT-green)

## Overview

This repository contains the source code for **UTMS**, a university teaching management and social-media platform implemented in **C++20**. It was developed as the *Sixth Computer Assignment* for the *Advanced Programming* course at the **University of Tehran**.

The system combines a simplified educational management service with a social network. Students, professors, and the default university account can authenticate, publish posts, connect with each other, receive notifications, manage course offerings, enroll in courses, use teaching-assistant workflows, and interact with course announcement channels. The project also includes a lightweight HTTP web interface powered by the provided APHTTP-style server library.

## Project Objectives

- ✅ **University course management:** Create course offerings, enforce prerequisites, verify major eligibility, prevent schedule/exam conflicts, and manage student enrollment.
- ✅ **Social-media layer:** Publish personal posts, connect users bidirectionally, browse personal pages, and receive chronological notifications.
- ✅ **Teaching-assistant workflow:** Professors can publish TA forms, students can submit requests, and professors can accept or reject applicants interactively.
- ✅ **Course announcement channels:** Each course offering has a dedicated channel where professors and accepted TAs can post announcements for enrolled users.
- ✅ **Complete web user interface:** A browser-accessible dashboard provides graphical pages and forms for the project workflows, so normal usage no longer requires the CLI.
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

The third phase adds a lightweight HTTP interface on top of the same UTMS engine. The temporary demo pages from the starter web project were removed and replaced by role-aware pages matching the assignment requirements.

- `/` provides the login page and preserves the original UTMS login visual style.
- `/home` shows a dashboard customized for students, professors, and the system manager.
- Students can enroll in courses, drop courses, view registered courses, browse course channels, and request TA positions through web forms.
- Professors can publish course-channel posts, create TA forms, review applicants, and close TA forms through web pages.
- The system manager can create new course offerings from a graphical form.
- All ordinary users can publish posts with optional PNG images, update/delete profile photos, browse personal pages, connect to users, and read notifications.

The web interface delegates to the same `Instruction_Handler` business rules used by the terminal version, keeping the core behavior centralized while providing a full graphical workflow.

## Screenshots

| Login | Dashboard | Post Details |
| ----- | --------- | ------------ |
| ![Login - Dark Theme](./screenshots/login-dark.png) | ![Dashboard - Dark Theme](./screenshots/dashboard-dark.png) | ![Post Details - Dark Theme](./screenshots/post-details-dark.png) |
| ![Login - Light Theme](./screenshots/login-light.png) | ![Dashboard - Light Theme](./screenshots/dashboard-light.png) | ![Post Details - Light Theme](./screenshots/post-details-light.png) |

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
├── web/                            # Static web assets and runtime uploads
│   ├── UTMS.png                     # Transparent UTMS logo asset
│   ├── login.html                   # Login page style reference
│   └── uploads/                     # Runtime PNG uploads
├── screenshots/                     # README preview screenshots
│   ├── login-dark.png
│   ├── login-light.png
│   ├── dashboard-dark.png
│   ├── dashboard-light.png
│   ├── post-details-dark.png
│   └── post-details-light.png
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

The executable required by the third phase is generated as:

```bash
./out.utms
```

A compatibility symlink named `utms.out` is also created by the Makefile.

**3. Run in terminal mode**

```bash
./out.utms data_majors.csv data_students.csv data_courses.csv data_professors.csv --cli
```

You can also pipe commands directly:

```bash
cat tests/smoke_input.txt | ./out.utms data_majors.csv data_students.csv data_courses.csv data_professors.csv --cli
```

**4. Optional smoke tests**

```bash
./scripts/run_smoke_tests.sh
./scripts/run_web_smoke_tests.sh
```

The web smoke test covers login, role switching, course offering creation, session protection, PNG upload, enrollment, and course viewing.

**5. Optional sanitizer build**

```bash
make sanitize
```

This target rebuilds with AddressSanitizer/UndefinedBehaviorSanitizer flags for local debugging.

**6. Run in web mode**

```bash
./out.utms data_majors.csv data_students.csv data_courses.csv data_professors.csv --web
```

Then open:

```text
http://localhost:5000
```

HTTP request/response logging is enabled by default in web mode so development actions are visible in the terminal. To disable it for a quieter run:

```bash
UTMS_HTTP_LOG=0 ./out.utms data_majors.csv data_students.csv data_courses.csv data_professors.csv --web
```

## Web Workflow Coverage

The web UI contains dedicated pages for the operations required in the third phase:

- Login/logout and a role-aware dashboard
- Create personal posts with optional PNG images
- Save or delete profile photos
- View course offerings
- Search and display personal pages with profile images and post images
- Enroll in and drop course offerings as a student
- View registered courses as a student
- Create course offerings as the system manager
- Browse course channels and read channel posts
- Publish course-channel posts as a professor or accepted TA
- Create, request, review, and close TA forms
- Connect to users and read notifications

The CLI remains available for automated regression testing and compatibility with the earlier phases.

## Implementation Notes

- Command arguments are order-independent after `?`, as required by the assignment.
- Personal posts and course posts are displayed newest-first where the specification requires timeline ordering.
- Course offerings are stored in a `deque` so object addresses remain stable when new offerings are added.
- Generated build artifacts are ignored through `.gitignore`; rebuild with `make` whenever needed.
- The login page intentionally does not expose privileged account credentials.
- Web requests are checked against the active session cookie before serving dashboard/action pages.
- Uploaded PNG files are validated by signature, stored under `web/uploads/`, and cleaned up when replaced or deleted through the UI.
- HTTP request parsing uses RAII-managed request/response objects in the server loop to avoid leaks on malformed or interrupted requests.
- The web server logs requests/responses by default, redacts passwords/cookies in logs, ignores `SIGPIPE`, sends responses in a loop to avoid partial-send bugs, and applies socket I/O timeouts so an idle or partial browser connection cannot block the single-threaded server indefinitely.

## Author

This project was developed for the **Advanced Programming** course at the **University of Tehran**.

- **[Meraj Rastegar](https://github.com/mragetsars)**

## License

This project is licensed under the **MIT License**. See [`LICENSE`](./LICENSE) for details.
