# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/Dev/libs/game/PeakEngines/PeakEngine/old/peakengine-luna/lib/toluapp

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/Dev/libs/game/PeakEngines/PeakEngine/old/peakengine-luna/lib/toluapp

# Include any dependencies generated for this target.
include CMakeFiles/toluapp.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/toluapp.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/toluapp.dir/flags.make

CMakeFiles/toluapp.dir/src/bin/tolua.c.o: CMakeFiles/toluapp.dir/flags.make
CMakeFiles/toluapp.dir/src/bin/tolua.c.o: src/bin/tolua.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/Dev/libs/game/PeakEngines/PeakEngine/old/peakengine-luna/lib/toluapp/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/toluapp.dir/src/bin/tolua.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/toluapp.dir/src/bin/tolua.c.o   -c /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/Dev/libs/game/PeakEngines/PeakEngine/old/peakengine-luna/lib/toluapp/src/bin/tolua.c

CMakeFiles/toluapp.dir/src/bin/tolua.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/toluapp.dir/src/bin/tolua.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/Dev/libs/game/PeakEngines/PeakEngine/old/peakengine-luna/lib/toluapp/src/bin/tolua.c > CMakeFiles/toluapp.dir/src/bin/tolua.c.i

CMakeFiles/toluapp.dir/src/bin/tolua.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/toluapp.dir/src/bin/tolua.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/Dev/libs/game/PeakEngines/PeakEngine/old/peakengine-luna/lib/toluapp/src/bin/tolua.c -o CMakeFiles/toluapp.dir/src/bin/tolua.c.s

CMakeFiles/toluapp.dir/src/bin/tolua.c.o.requires:

.PHONY : CMakeFiles/toluapp.dir/src/bin/tolua.c.o.requires

CMakeFiles/toluapp.dir/src/bin/tolua.c.o.provides: CMakeFiles/toluapp.dir/src/bin/tolua.c.o.requires
	$(MAKE) -f CMakeFiles/toluapp.dir/build.make CMakeFiles/toluapp.dir/src/bin/tolua.c.o.provides.build
.PHONY : CMakeFiles/toluapp.dir/src/bin/tolua.c.o.provides

CMakeFiles/toluapp.dir/src/bin/tolua.c.o.provides.build: CMakeFiles/toluapp.dir/src/bin/tolua.c.o


CMakeFiles/toluapp.dir/src/bin/toluabind.c.o: CMakeFiles/toluapp.dir/flags.make
CMakeFiles/toluapp.dir/src/bin/toluabind.c.o: src/bin/toluabind.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/Dev/libs/game/PeakEngines/PeakEngine/old/peakengine-luna/lib/toluapp/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/toluapp.dir/src/bin/toluabind.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/toluapp.dir/src/bin/toluabind.c.o   -c /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/Dev/libs/game/PeakEngines/PeakEngine/old/peakengine-luna/lib/toluapp/src/bin/toluabind.c

CMakeFiles/toluapp.dir/src/bin/toluabind.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/toluapp.dir/src/bin/toluabind.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/Dev/libs/game/PeakEngines/PeakEngine/old/peakengine-luna/lib/toluapp/src/bin/toluabind.c > CMakeFiles/toluapp.dir/src/bin/toluabind.c.i

CMakeFiles/toluapp.dir/src/bin/toluabind.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/toluapp.dir/src/bin/toluabind.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/Dev/libs/game/PeakEngines/PeakEngine/old/peakengine-luna/lib/toluapp/src/bin/toluabind.c -o CMakeFiles/toluapp.dir/src/bin/toluabind.c.s

CMakeFiles/toluapp.dir/src/bin/toluabind.c.o.requires:

.PHONY : CMakeFiles/toluapp.dir/src/bin/toluabind.c.o.requires

CMakeFiles/toluapp.dir/src/bin/toluabind.c.o.provides: CMakeFiles/toluapp.dir/src/bin/toluabind.c.o.requires
	$(MAKE) -f CMakeFiles/toluapp.dir/build.make CMakeFiles/toluapp.dir/src/bin/toluabind.c.o.provides.build
.PHONY : CMakeFiles/toluapp.dir/src/bin/toluabind.c.o.provides

CMakeFiles/toluapp.dir/src/bin/toluabind.c.o.provides.build: CMakeFiles/toluapp.dir/src/bin/toluabind.c.o


# Object files for target toluapp
toluapp_OBJECTS = \
"CMakeFiles/toluapp.dir/src/bin/tolua.c.o" \
"CMakeFiles/toluapp.dir/src/bin/toluabind.c.o"

# External object files for target toluapp
toluapp_EXTERNAL_OBJECTS =

toluapp: CMakeFiles/toluapp.dir/src/bin/tolua.c.o
toluapp: CMakeFiles/toluapp.dir/src/bin/toluabind.c.o
toluapp: CMakeFiles/toluapp.dir/build.make
toluapp: libtoluapp.so
toluapp: /usr/lib64/liblua.so
toluapp: /usr/lib64/libm.so
toluapp: CMakeFiles/toluapp.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/Dev/libs/game/PeakEngines/PeakEngine/old/peakengine-luna/lib/toluapp/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable toluapp"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/toluapp.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/toluapp.dir/build: toluapp

.PHONY : CMakeFiles/toluapp.dir/build

CMakeFiles/toluapp.dir/requires: CMakeFiles/toluapp.dir/src/bin/tolua.c.o.requires
CMakeFiles/toluapp.dir/requires: CMakeFiles/toluapp.dir/src/bin/toluabind.c.o.requires

.PHONY : CMakeFiles/toluapp.dir/requires

CMakeFiles/toluapp.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/toluapp.dir/cmake_clean.cmake
.PHONY : CMakeFiles/toluapp.dir/clean

CMakeFiles/toluapp.dir/depend:
	cd /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/Dev/libs/game/PeakEngines/PeakEngine/old/peakengine-luna/lib/toluapp && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/Dev/libs/game/PeakEngines/PeakEngine/old/peakengine-luna/lib/toluapp /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/Dev/libs/game/PeakEngines/PeakEngine/old/peakengine-luna/lib/toluapp /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/Dev/libs/game/PeakEngines/PeakEngine/old/peakengine-luna/lib/toluapp /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/Dev/libs/game/PeakEngines/PeakEngine/old/peakengine-luna/lib/toluapp /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/Dev/libs/game/PeakEngines/PeakEngine/old/peakengine-luna/lib/toluapp/CMakeFiles/toluapp.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/toluapp.dir/depend

