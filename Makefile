CXX = g++
CXXFLAGS = -std=c++20 -I./includes -Wall -Wextra -pedantic -Wno-unused-parameter -Wno-type-limits -Wno-implicit-fallthrough
SANITIZE_FLAGS = -fsanitize=address,undefined -fno-omit-frame-pointer -g
SRCDIR = sources
OBJDIR = objects
BINDIR = .
EXECUTABLE = $(BINDIR)/out.utms

SOURCES := $(wildcard $(SRCDIR)/*.cpp)
OBJECTS := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))
HEADERS := $(wildcard includes/*.hpp)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^
	ln -sf out.utms utms.out

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS)
	mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run-cli: all
	./out.utms data/data_majors.csv data/data_students.csv data/data_courses.csv data/data_professors.csv --cli

run-web: all
	./out.utms data/data_majors.csv data/data_students.csv data/data_courses.csv data/data_professors.csv --web

test: all
	UTMS_SKIP_BUILD=1 ./scripts/run_smoke_tests.sh
	UTMS_SKIP_BUILD=1 ./scripts/run_cli_regression_tests.sh
	UTMS_SKIP_BUILD=1 ./scripts/run_web_smoke_tests.sh

format:
	@if command -v clang-format >/dev/null 2>&1; then clang-format -i includes/*.hpp sources/*.cpp; else echo "clang-format is not installed"; fi

lint:
	@if command -v clang-tidy >/dev/null 2>&1; then clang-tidy sources/*.cpp -- $(CXXFLAGS); else echo "clang-tidy is not installed"; fi

sanitize:
	$(MAKE) clean
	$(MAKE) CXXFLAGS="$(CXXFLAGS) $(SANITIZE_FLAGS)" all

clean:
	rm -rf $(OBJDIR) $(EXECUTABLE) utms.out

.PHONY: all clean run-cli run-web test format lint sanitize
