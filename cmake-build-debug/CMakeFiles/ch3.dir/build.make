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
include CMakeFiles/ch3.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/ch3.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ch3.dir/flags.make

CMakeFiles/ch3.dir/Chapter3/MPI_recom.c.o: CMakeFiles/ch3.dir/flags.make
CMakeFiles/ch3.dir/Chapter3/MPI_recom.c.o: ../Chapter3/MPI_recom.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/jichen/Documents/C/MPI/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/ch3.dir/Chapter3/MPI_recom.c.o"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/ch3.dir/Chapter3/MPI_recom.c.o   -c /Users/jichen/Documents/C/MPI/Chapter3/MPI_recom.c

CMakeFiles/ch3.dir/Chapter3/MPI_recom.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/ch3.dir/Chapter3/MPI_recom.c.i"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/jichen/Documents/C/MPI/Chapter3/MPI_recom.c > CMakeFiles/ch3.dir/Chapter3/MPI_recom.c.i

CMakeFiles/ch3.dir/Chapter3/MPI_recom.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/ch3.dir/Chapter3/MPI_recom.c.s"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/jichen/Documents/C/MPI/Chapter3/MPI_recom.c -o CMakeFiles/ch3.dir/Chapter3/MPI_recom.c.s

# Object files for target ch3
ch3_OBJECTS = \
"CMakeFiles/ch3.dir/Chapter3/MPI_recom.c.o"

# External object files for target ch3
ch3_EXTERNAL_OBJECTS =

ch3: CMakeFiles/ch3.dir/Chapter3/MPI_recom.c.o
ch3: CMakeFiles/ch3.dir/build.make
ch3: CMakeFiles/ch3.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/jichen/Documents/C/MPI/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable ch3"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ch3.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ch3.dir/build: ch3

.PHONY : CMakeFiles/ch3.dir/build

CMakeFiles/ch3.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ch3.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ch3.dir/clean

CMakeFiles/ch3.dir/depend:
	cd /Users/jichen/Documents/C/MPI/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/jichen/Documents/C/MPI /Users/jichen/Documents/C/MPI /Users/jichen/Documents/C/MPI/cmake-build-debug /Users/jichen/Documents/C/MPI/cmake-build-debug /Users/jichen/Documents/C/MPI/cmake-build-debug/CMakeFiles/ch3.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ch3.dir/depend
