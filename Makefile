# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

# Default target executed when no arguments are given to make.
default_target: all
.PHONY : default_target

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

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/mysql/mysql-5.5.35

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/mysql/mysql-5.5.35

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running interactive CMake command-line interface..."
	/usr/bin/cmake -i .
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache
.PHONY : edit_cache/fast

# Special rule for the target install
install: preinstall
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Install the project..."
	/usr/bin/cmake -P cmake_install.cmake
.PHONY : install

# Special rule for the target install
install/fast: preinstall/fast
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Install the project..."
	/usr/bin/cmake -P cmake_install.cmake
.PHONY : install/fast

# Special rule for the target install/local
install/local: preinstall
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Installing only the local directory..."
	/usr/bin/cmake -DCMAKE_INSTALL_LOCAL_ONLY=1 -P cmake_install.cmake
.PHONY : install/local

# Special rule for the target install/local
install/local/fast: install/local
.PHONY : install/local/fast

# Special rule for the target install/strip
install/strip: preinstall
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Installing the project stripped..."
	/usr/bin/cmake -DCMAKE_INSTALL_DO_STRIP=1 -P cmake_install.cmake
.PHONY : install/strip

# Special rule for the target install/strip
install/strip/fast: install/strip
.PHONY : install/strip/fast

# Special rule for the target list_install_components
list_install_components:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Available install components are: \"Client\" \"DataFiles\" \"DebugBinaries\" \"Development\" \"Documentation\" \"Info\" \"IniFiles\" \"ManPages\" \"Readme\" \"Server\" \"Server_Scripts\" \"SharedLibraries\" \"SqlBench\" \"SupportFiles\" \"Test\" \"Unspecified\""
.PHONY : list_install_components

# Special rule for the target list_install_components
list_install_components/fast: list_install_components
.PHONY : list_install_components/fast

# Special rule for the target package
package: preinstall
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Run CPack packaging tool..."
	cd /home/mysql/mysql-5.5.35 && /usr/bin/cpack --config ./CPackConfig.cmake
.PHONY : package

# Special rule for the target package
package/fast: package
.PHONY : package/fast

# Special rule for the target package_source
package_source:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Run CPack packaging tool for source..."
	cd /home/mysql/mysql-5.5.35 && /usr/bin/cpack --config ./CPackSourceConfig.cmake /home/mysql/mysql-5.5.35/CPackSourceConfig.cmake
.PHONY : package_source

# Special rule for the target package_source
package_source/fast: package_source
.PHONY : package_source/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache
.PHONY : rebuild_cache/fast

# Special rule for the target test
test:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running tests..."
	/usr/bin/ctest --force-new-ctest-process $(ARGS)
.PHONY : test

# Special rule for the target test
test/fast: test
.PHONY : test/fast

# The main all target
all: cmake_check_build_system
	cd /home/mysql/mysql-5.5.35 && $(CMAKE_COMMAND) -E cmake_progress_start /home/mysql/mysql-5.5.35/CMakeFiles /home/mysql/mysql-5.5.35/storage/smalldb/CMakeFiles/progress.marks
	cd /home/mysql/mysql-5.5.35 && $(MAKE) -f CMakeFiles/Makefile2 storage/smalldb/all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/mysql/mysql-5.5.35/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	cd /home/mysql/mysql-5.5.35 && $(MAKE) -f CMakeFiles/Makefile2 storage/smalldb/clean
.PHONY : clean

# The main clean target
clean/fast: clean
.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	cd /home/mysql/mysql-5.5.35 && $(MAKE) -f CMakeFiles/Makefile2 storage/smalldb/preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	cd /home/mysql/mysql-5.5.35 && $(MAKE) -f CMakeFiles/Makefile2 storage/smalldb/preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	cd /home/mysql/mysql-5.5.35 && $(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

# Convenience name for target.
storage/smalldb/CMakeFiles/smalldb.dir/rule:
	cd /home/mysql/mysql-5.5.35 && $(MAKE) -f CMakeFiles/Makefile2 storage/smalldb/CMakeFiles/smalldb.dir/rule
.PHONY : storage/smalldb/CMakeFiles/smalldb.dir/rule

# Convenience name for target.
smalldb: storage/smalldb/CMakeFiles/smalldb.dir/rule
.PHONY : smalldb

# fast build rule for target.
smalldb/fast:
	cd /home/mysql/mysql-5.5.35 && $(MAKE) -f storage/smalldb/CMakeFiles/smalldb.dir/build.make storage/smalldb/CMakeFiles/smalldb.dir/build
.PHONY : smalldb/fast

ha_smalldb.o: ha_smalldb.cc.o
.PHONY : ha_smalldb.o

# target to build an object file
ha_smalldb.cc.o:
	cd /home/mysql/mysql-5.5.35 && $(MAKE) -f storage/smalldb/CMakeFiles/smalldb.dir/build.make storage/smalldb/CMakeFiles/smalldb.dir/ha_smalldb.cc.o
.PHONY : ha_smalldb.cc.o

ha_smalldb.i: ha_smalldb.cc.i
.PHONY : ha_smalldb.i

# target to preprocess a source file
ha_smalldb.cc.i:
	cd /home/mysql/mysql-5.5.35 && $(MAKE) -f storage/smalldb/CMakeFiles/smalldb.dir/build.make storage/smalldb/CMakeFiles/smalldb.dir/ha_smalldb.cc.i
.PHONY : ha_smalldb.cc.i

ha_smalldb.s: ha_smalldb.cc.s
.PHONY : ha_smalldb.s

# target to generate assembly for a file
ha_smalldb.cc.s:
	cd /home/mysql/mysql-5.5.35 && $(MAKE) -f storage/smalldb/CMakeFiles/smalldb.dir/build.make storage/smalldb/CMakeFiles/smalldb.dir/ha_smalldb.cc.s
.PHONY : ha_smalldb.cc.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... install"
	@echo "... install/local"
	@echo "... install/strip"
	@echo "... list_install_components"
	@echo "... package"
	@echo "... package_source"
	@echo "... rebuild_cache"
	@echo "... smalldb"
	@echo "... test"
	@echo "... ha_smalldb.o"
	@echo "... ha_smalldb.i"
	@echo "... ha_smalldb.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	cd /home/mysql/mysql-5.5.35 && $(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system
