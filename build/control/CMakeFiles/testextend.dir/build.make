# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/robot/miniMQTT

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/robot/miniMQTT/build

# Include any dependencies generated for this target.
include control/CMakeFiles/testextend.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include control/CMakeFiles/testextend.dir/compiler_depend.make

# Include the progress variables for this target.
include control/CMakeFiles/testextend.dir/progress.make

# Include the compile flags for this target's objects.
include control/CMakeFiles/testextend.dir/flags.make

control/CMakeFiles/testextend.dir/test/test.c.o: control/CMakeFiles/testextend.dir/flags.make
control/CMakeFiles/testextend.dir/test/test.c.o: ../control/test/test.c
control/CMakeFiles/testextend.dir/test/test.c.o: control/CMakeFiles/testextend.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/robot/miniMQTT/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object control/CMakeFiles/testextend.dir/test/test.c.o"
	cd /home/robot/miniMQTT/build/control && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT control/CMakeFiles/testextend.dir/test/test.c.o -MF CMakeFiles/testextend.dir/test/test.c.o.d -o CMakeFiles/testextend.dir/test/test.c.o -c /home/robot/miniMQTT/control/test/test.c

control/CMakeFiles/testextend.dir/test/test.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/testextend.dir/test/test.c.i"
	cd /home/robot/miniMQTT/build/control && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/robot/miniMQTT/control/test/test.c > CMakeFiles/testextend.dir/test/test.c.i

control/CMakeFiles/testextend.dir/test/test.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/testextend.dir/test/test.c.s"
	cd /home/robot/miniMQTT/build/control && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/robot/miniMQTT/control/test/test.c -o CMakeFiles/testextend.dir/test/test.c.s

# Object files for target testextend
testextend_OBJECTS = \
"CMakeFiles/testextend.dir/test/test.c.o"

# External object files for target testextend
testextend_EXTERNAL_OBJECTS =

control/libtestextend.so: control/CMakeFiles/testextend.dir/test/test.c.o
control/libtestextend.so: control/CMakeFiles/testextend.dir/build.make
control/libtestextend.so: net/libcontrol.so
control/libtestextend.so: control/CMakeFiles/testextend.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/robot/miniMQTT/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C shared library libtestextend.so"
	cd /home/robot/miniMQTT/build/control && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/testextend.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
control/CMakeFiles/testextend.dir/build: control/libtestextend.so
.PHONY : control/CMakeFiles/testextend.dir/build

control/CMakeFiles/testextend.dir/clean:
	cd /home/robot/miniMQTT/build/control && $(CMAKE_COMMAND) -P CMakeFiles/testextend.dir/cmake_clean.cmake
.PHONY : control/CMakeFiles/testextend.dir/clean

control/CMakeFiles/testextend.dir/depend:
	cd /home/robot/miniMQTT/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/robot/miniMQTT /home/robot/miniMQTT/control /home/robot/miniMQTT/build /home/robot/miniMQTT/build/control /home/robot/miniMQTT/build/control/CMakeFiles/testextend.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : control/CMakeFiles/testextend.dir/depend

