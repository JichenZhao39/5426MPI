# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.12

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/jichen/Documents/C/MPI

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/jichen/Documents/C/MPI/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/MPI.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/MPI.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/MPI.dir/flags.make

CMakeFiles/MPI.dir/main.c.o: CMakeFiles/MPI.dir/flags.make
CMakeFiles/MPI.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/jichen/Documents/C/MPI/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/MPI.dir/main.c.o"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/MPI.dir/main.c.o   -c /Users/jichen/Documents/C/MPI/main.c

CMakeFiles/MPI.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/MPI.dir/main.c.i"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/jichen/Documents/C/MPI/main.c > CMakeFiles/MPI.dir/main.c.i

CMakeFiles/MPI.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/MPI.dir/main.c.s"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/jichen/Documents/C/MPI/main.c -o CMakeFiles/MPI.dir/main.c.s

CMakeFiles/MPI.dir/tmpi_rank.c.o: CMakeFiles/MPI.dir/flags.make
CMakeFiles/MPI.dir/tmpi_rank.c.o: ../tmpi_rank.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/jichen/Documents/C/MPI/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/MPI.dir/tmpi_rank.c.o"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/MPI.dir/tmpi_rank.c.o   -c /Users/jichen/Documents/C/MPI/tmpi_rank.c

CMakeFiles/MPI.dir/tmpi_rank.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/MPI.dir/tmpi_rank.c.i"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/jichen/Documents/C/MPI/tmpi_rank.c > CMakeFiles/MPI.dir/tmpi_rank.c.i

CMakeFiles/MPI.dir/tmpi_rank.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/MPI.dir/tmpi_rank.c.s"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/jichen/Documents/C/MPI/tmpi_rank.c -o CMakeFiles/MPI.dir/tmpi_rank.c.s

# Object files for target MPI
MPI_OBJECTS = \
"CMakeFiles/MPI.dir/main.c.o" \
"CMakeFiles/MPI.dir/tmpi_rank.c.o"

# External object files for target MPI
MPI_EXTERNAL_OBJECTS =

MPI: CMakeFiles/MPI.dir/main.c.o
MPI: CMakeFiles/MPI.dir/tmpi_rank.c.o
MPI: CMakeFiles/MPI.dir/build.make
MPI: CMakeFiles/MPI.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/jichen/Documents/C/MPI/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable MPI"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/MPI.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/MPI.dir/build: MPI

.PHONY : CMakeFiles/MPI.dir/build

CMakeFiles/MPI.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/MPI.dir/cmake_clean.cmake
.PHONY : CMakeFiles/MPI.dir/clean

CMakeFiles/MPI.dir/depend:
	cd /Users/jichen/Documents/C/MPI/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/jichen/Documents/C/MPI /Users/jichen/Documents/C/MPI /Users/jichen/Documents/C/MPI/cmake-build-debug /Users/jichen/Documents/C/MPI/cmake-build-debug /Users/jichen/Documents/C/MPI/cmake-build-debug/CMakeFiles/MPI.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/MPI.dir/depend

