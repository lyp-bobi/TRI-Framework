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
include CMakeFiles/test-rtree-RTreeLoad.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/test-rtree-RTreeLoad.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test-rtree-RTreeLoad.dir/flags.make

CMakeFiles/test-rtree-RTreeLoad.dir/rtree/RTreeLoad.obj: CMakeFiles/test-rtree-RTreeLoad.dir/flags.make
CMakeFiles/test-rtree-RTreeLoad.dir/rtree/RTreeLoad.obj: CMakeFiles/test-rtree-RTreeLoad.dir/includes_CXX.rsp
CMakeFiles/test-rtree-RTreeLoad.dir/rtree/RTreeLoad.obj: ../rtree/RTreeLoad.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\MbbcOnC-\test\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/test-rtree-RTreeLoad.dir/rtree/RTreeLoad.obj"
	C:\PROGRA~1\MINGW-~1\X86_64~1.0-P\mingw64\bin\G__~1.EXE  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\test-rtree-RTreeLoad.dir\rtree\RTreeLoad.obj -c D:\MbbcOnC-\test\rtree\RTreeLoad.cc

CMakeFiles/test-rtree-RTreeLoad.dir/rtree/RTreeLoad.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test-rtree-RTreeLoad.dir/rtree/RTreeLoad.i"
	C:\PROGRA~1\MINGW-~1\X86_64~1.0-P\mingw64\bin\G__~1.EXE $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\MbbcOnC-\test\rtree\RTreeLoad.cc > CMakeFiles\test-rtree-RTreeLoad.dir\rtree\RTreeLoad.i

CMakeFiles/test-rtree-RTreeLoad.dir/rtree/RTreeLoad.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test-rtree-RTreeLoad.dir/rtree/RTreeLoad.s"
	C:\PROGRA~1\MINGW-~1\X86_64~1.0-P\mingw64\bin\G__~1.EXE $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\MbbcOnC-\test\rtree\RTreeLoad.cc -o CMakeFiles\test-rtree-RTreeLoad.dir\rtree\RTreeLoad.s

# Object files for target test-rtree-RTreeLoad
test__rtree__RTreeLoad_OBJECTS = \
"CMakeFiles/test-rtree-RTreeLoad.dir/rtree/RTreeLoad.obj"

# External object files for target test-rtree-RTreeLoad
test__rtree__RTreeLoad_EXTERNAL_OBJECTS =

test-rtree-RTreeLoad.exe: CMakeFiles/test-rtree-RTreeLoad.dir/rtree/RTreeLoad.obj
test-rtree-RTreeLoad.exe: CMakeFiles/test-rtree-RTreeLoad.dir/build.make
test-rtree-RTreeLoad.exe: CMakeFiles/test-rtree-RTreeLoad.dir/linklibs.rsp
test-rtree-RTreeLoad.exe: CMakeFiles/test-rtree-RTreeLoad.dir/objects1.rsp
test-rtree-RTreeLoad.exe: CMakeFiles/test-rtree-RTreeLoad.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=D:\MbbcOnC-\test\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable test-rtree-RTreeLoad.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\test-rtree-RTreeLoad.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test-rtree-RTreeLoad.dir/build: test-rtree-RTreeLoad.exe

.PHONY : CMakeFiles/test-rtree-RTreeLoad.dir/build

CMakeFiles/test-rtree-RTreeLoad.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\test-rtree-RTreeLoad.dir\cmake_clean.cmake
.PHONY : CMakeFiles/test-rtree-RTreeLoad.dir/clean

CMakeFiles/test-rtree-RTreeLoad.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" D:\MbbcOnC-\test D:\MbbcOnC-\test D:\MbbcOnC-\test\cmake-build-debug D:\MbbcOnC-\test\cmake-build-debug D:\MbbcOnC-\test\cmake-build-debug\CMakeFiles\test-rtree-RTreeLoad.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/test-rtree-RTreeLoad.dir/depend

