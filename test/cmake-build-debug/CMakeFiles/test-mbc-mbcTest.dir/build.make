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
include CMakeFiles/test-mbc-mbcTest.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/test-mbc-mbcTest.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test-mbc-mbcTest.dir/flags.make

CMakeFiles/test-mbc-mbcTest.dir/mbc/mbcTest.obj: CMakeFiles/test-mbc-mbcTest.dir/flags.make
CMakeFiles/test-mbc-mbcTest.dir/mbc/mbcTest.obj: CMakeFiles/test-mbc-mbcTest.dir/includes_CXX.rsp
CMakeFiles/test-mbc-mbcTest.dir/mbc/mbcTest.obj: ../mbc/mbcTest.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\MbbcOnC-\test\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/test-mbc-mbcTest.dir/mbc/mbcTest.obj"
	C:\PROGRA~1\MINGW-~1\X86_64~1.0-P\mingw64\bin\G__~1.EXE  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\test-mbc-mbcTest.dir\mbc\mbcTest.obj -c D:\MbbcOnC-\test\mbc\mbcTest.cc

CMakeFiles/test-mbc-mbcTest.dir/mbc/mbcTest.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test-mbc-mbcTest.dir/mbc/mbcTest.i"
	C:\PROGRA~1\MINGW-~1\X86_64~1.0-P\mingw64\bin\G__~1.EXE $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\MbbcOnC-\test\mbc\mbcTest.cc > CMakeFiles\test-mbc-mbcTest.dir\mbc\mbcTest.i

CMakeFiles/test-mbc-mbcTest.dir/mbc/mbcTest.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test-mbc-mbcTest.dir/mbc/mbcTest.s"
	C:\PROGRA~1\MINGW-~1\X86_64~1.0-P\mingw64\bin\G__~1.EXE $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\MbbcOnC-\test\mbc\mbcTest.cc -o CMakeFiles\test-mbc-mbcTest.dir\mbc\mbcTest.s

# Object files for target test-mbc-mbcTest
test__mbc__mbcTest_OBJECTS = \
"CMakeFiles/test-mbc-mbcTest.dir/mbc/mbcTest.obj"

# External object files for target test-mbc-mbcTest
test__mbc__mbcTest_EXTERNAL_OBJECTS =

test-mbc-mbcTest.exe: CMakeFiles/test-mbc-mbcTest.dir/mbc/mbcTest.obj
test-mbc-mbcTest.exe: CMakeFiles/test-mbc-mbcTest.dir/build.make
test-mbc-mbcTest.exe: CMakeFiles/test-mbc-mbcTest.dir/linklibs.rsp
test-mbc-mbcTest.exe: CMakeFiles/test-mbc-mbcTest.dir/objects1.rsp
test-mbc-mbcTest.exe: CMakeFiles/test-mbc-mbcTest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=D:\MbbcOnC-\test\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable test-mbc-mbcTest.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\test-mbc-mbcTest.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test-mbc-mbcTest.dir/build: test-mbc-mbcTest.exe

.PHONY : CMakeFiles/test-mbc-mbcTest.dir/build

CMakeFiles/test-mbc-mbcTest.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\test-mbc-mbcTest.dir\cmake_clean.cmake
.PHONY : CMakeFiles/test-mbc-mbcTest.dir/clean

CMakeFiles/test-mbc-mbcTest.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" D:\MbbcOnC-\test D:\MbbcOnC-\test D:\MbbcOnC-\test\cmake-build-debug D:\MbbcOnC-\test\cmake-build-debug D:\MbbcOnC-\test\cmake-build-debug\CMakeFiles\test-mbc-mbcTest.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/test-mbc-mbcTest.dir/depend

