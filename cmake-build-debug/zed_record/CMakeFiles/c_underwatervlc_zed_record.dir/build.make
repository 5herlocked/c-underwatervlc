# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.19

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
CMAKE_COMMAND = /home/sherlock/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/211.7142.21/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/sherlock/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/211.7142.21/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/sherlock/source/c_underwatervlc

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/sherlock/source/c_underwatervlc/cmake-build-debug

# Include any dependencies generated for this target.
include zed_record/CMakeFiles/c_underwatervlc_zed_record.dir/depend.make

# Include the progress variables for this target.
include zed_record/CMakeFiles/c_underwatervlc_zed_record.dir/progress.make

# Include the compile flags for this target's objects.
include zed_record/CMakeFiles/c_underwatervlc_zed_record.dir/flags.make

zed_record/CMakeFiles/c_underwatervlc_zed_record.dir/src/zed_record.cpp.o: zed_record/CMakeFiles/c_underwatervlc_zed_record.dir/flags.make
zed_record/CMakeFiles/c_underwatervlc_zed_record.dir/src/zed_record.cpp.o: ../zed_record/src/zed_record.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/sherlock/source/c_underwatervlc/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object zed_record/CMakeFiles/c_underwatervlc_zed_record.dir/src/zed_record.cpp.o"
	cd /home/sherlock/source/c_underwatervlc/cmake-build-debug/zed_record && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/c_underwatervlc_zed_record.dir/src/zed_record.cpp.o -c /home/sherlock/source/c_underwatervlc/zed_record/src/zed_record.cpp

zed_record/CMakeFiles/c_underwatervlc_zed_record.dir/src/zed_record.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/c_underwatervlc_zed_record.dir/src/zed_record.cpp.i"
	cd /home/sherlock/source/c_underwatervlc/cmake-build-debug/zed_record && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/sherlock/source/c_underwatervlc/zed_record/src/zed_record.cpp > CMakeFiles/c_underwatervlc_zed_record.dir/src/zed_record.cpp.i

zed_record/CMakeFiles/c_underwatervlc_zed_record.dir/src/zed_record.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/c_underwatervlc_zed_record.dir/src/zed_record.cpp.s"
	cd /home/sherlock/source/c_underwatervlc/cmake-build-debug/zed_record && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/sherlock/source/c_underwatervlc/zed_record/src/zed_record.cpp -o CMakeFiles/c_underwatervlc_zed_record.dir/src/zed_record.cpp.s

# Object files for target c_underwatervlc_zed_record
c_underwatervlc_zed_record_OBJECTS = \
"CMakeFiles/c_underwatervlc_zed_record.dir/src/zed_record.cpp.o"

# External object files for target c_underwatervlc_zed_record
c_underwatervlc_zed_record_EXTERNAL_OBJECTS =

zed_record/c_underwatervlc_zed_record: zed_record/CMakeFiles/c_underwatervlc_zed_record.dir/src/zed_record.cpp.o
zed_record/c_underwatervlc_zed_record: zed_record/CMakeFiles/c_underwatervlc_zed_record.dir/build.make
zed_record/c_underwatervlc_zed_record: zed_record/CMakeFiles/c_underwatervlc_zed_record.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/sherlock/source/c_underwatervlc/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable c_underwatervlc_zed_record"
	cd /home/sherlock/source/c_underwatervlc/cmake-build-debug/zed_record && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/c_underwatervlc_zed_record.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
zed_record/CMakeFiles/c_underwatervlc_zed_record.dir/build: zed_record/c_underwatervlc_zed_record

.PHONY : zed_record/CMakeFiles/c_underwatervlc_zed_record.dir/build

zed_record/CMakeFiles/c_underwatervlc_zed_record.dir/clean:
	cd /home/sherlock/source/c_underwatervlc/cmake-build-debug/zed_record && $(CMAKE_COMMAND) -P CMakeFiles/c_underwatervlc_zed_record.dir/cmake_clean.cmake
.PHONY : zed_record/CMakeFiles/c_underwatervlc_zed_record.dir/clean

zed_record/CMakeFiles/c_underwatervlc_zed_record.dir/depend:
	cd /home/sherlock/source/c_underwatervlc/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/sherlock/source/c_underwatervlc /home/sherlock/source/c_underwatervlc/zed_record /home/sherlock/source/c_underwatervlc/cmake-build-debug /home/sherlock/source/c_underwatervlc/cmake-build-debug/zed_record /home/sherlock/source/c_underwatervlc/cmake-build-debug/zed_record/CMakeFiles/c_underwatervlc_zed_record.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : zed_record/CMakeFiles/c_underwatervlc_zed_record.dir/depend

