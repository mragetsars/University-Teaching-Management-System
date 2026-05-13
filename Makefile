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
	./out.utms data_majors.csv data_students.csv data_courses.csv data_professors.csv --cli

run-web: all
	./out.utms data_majors.csv data_students.csv data_courses.csv data_professors.csv --web

sanitize:
	$(MAKE) clean
	$(MAKE) CXXFLAGS="$(CXXFLAGS) $(SANITIZE_FLAGS)" all

clean:
	rm -rf $(OBJDIR) $(EXECUTABLE) utms.out

.PHONY: all clean run-cli run-web sanitize
