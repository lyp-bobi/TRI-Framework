# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.14

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "D:\Program Files\JetBrains\CLion 2019.1.2\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "D:\Program Files\JetBrains\CLion 2019.1.2\bin\cmake\win\bin\cmake.exe" -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = D:\MbbcOnC-\test

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = D:\MbbcOnC-\test\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/test-rtree-RTreeQuery.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/test-rtree-RTreeQuery.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test-rtree-RTreeQuery.dir/flags.make

CMakeFiles/test-rtree-RTreeQuery.dir/rtree/RTreeQuery.obj: CMakeFiles/test-rtree-RTreeQuery.dir/flags.make
CMakeFiles/test-rtree-RTreeQuery.dir/rtree/RTreeQuery.obj: CMakeFiles/test-rtree-RTreeQuery.dir/includes_CXX.rsp
CMakeFiles/test-rtree-RTreeQuery.dir/rtree/RTreeQuery.obj: ../rtree/RTreeQuery.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\MbbcOnC-\test\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/test-rtree-RTreeQuery.dir/rtree/RTreeQuery.obj"
	C:\PROGRA~1\MINGW-~1\X86_64~1.0-P\mingw64\bin\G__~1.EXE  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\test-rtree-RTreeQuery.dir\rtree\RTreeQuery.obj -c D:\MbbcOnC-\test\rtree\RTreeQuery.cc

CMakeFiles/test-rtree-RTreeQuery.dir/rtree/RTreeQuery.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test-rtree-RTreeQuery.dir/rtree/RTreeQuery.i"
	C:\PROGRA~1\MINGW-~1\X86_64~1.0-P\mingw64\bin\G__~1.EXE $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\MbbcOnC-\test\rtree\RTreeQuery.cc > CMakeFiles\test-rtree-RTreeQuery.dir\rtree\RTreeQuery.i

CMakeFiles/test-rtree-RTreeQuery.dir/rtree/RTreeQuery.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test-rtree-RTreeQuery.dir/rtree/RTreeQuery.s"
	C:\PROGRA~1\MINGW-~1\X86_64~1.0-P\mingw64\bin\G__~1.EXE $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\MbbcOnC-\test\rtree\RTreeQuery.cc -o CMakeFiles\test-rtree-RTreeQuery.dir\rtree\RTreeQuery.s

# Object files for target test-rtree-RTreeQuery
test__rtree__RTreeQuery_OBJECTS = \
"CMakeFiles/test-rtree-RTreeQuery.dir/rtree/RTreeQuery.obj"

# External object files for target test-rtree-RTreeQuery
test__rtree__RTreeQuery_EXTERNAL_OBJECTS =

test-rtree-RTreeQuery.exe: CMakeFiles/test-rtree-RTreeQuery.dir/rtree/RTreeQuery.obj
test-rtree-RTreeQuery.exe: CMakeFiles/test-rtree-RTreeQuery.dir/build.make
test-rtree-RTreeQuery.exe: CMakeFiles/test-rtree-RTreeQuery.dir/linklibs.rsp
test-rtree-RTreeQuery.exe: CMakeFiles/test-rtree-RTreeQuery.dir/objects1.rsp
test-rtree-RTreeQuery.exe: CMakeFiles/test-rtree-RTreeQuery.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=D:\MbbcOnC-\test\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable test-rtree-RTreeQuery.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\test-rtree-RTreeQuery.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test-rtree-RTreeQuery.dir/build: test-rtree-RTreeQuery.exe

.PHONY : CMakeFiles/test-rtree-RTreeQuery.dir/build

CMakeFiles/test-rtree-RTreeQuery.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\test-rtree-RTreeQuery.dir\cmake_clean.cmake
.PHONY : CMakeFiles/test-rtree-RTreeQuery.dir/clean

CMakeFiles/test-rtree-RTreeQuery.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" D:\MbbcOnC-\test D:\MbbcOnC-\test D:\MbbcOnC-\test\cmake-build-debug D:\MbbcOnC-\test\cmake-build-debug D:\MbbcOnC-\test\cmake-build-debug\CMakeFiles\test-rtree-RTreeQuery.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/test-rtree-RTreeQuery.dir/depend

