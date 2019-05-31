set(ENV{CC} "gcc")
set(ENV{CXX} "g++")
set(ENV{CXXFLAGS} "--coverage -Wall -Wpedantic")

set(CTEST_SOURCE_DIRECTORY ".")
set(CTEST_BINARY_DIRECTORY "build-test-gcc")
set(CTEST_CMAKE_GENERATOR "Unix Makefiles")

set(CTEST_COVERAGE_COMMAND "/usr/bin/gcov")
set(CTEST_MEMORYCHECK_COMMAND "/usr/bin/valgrind")

ctest_start("Continuous")
ctest_configure()
ctest_build()
ctest_test()
ctest_coverage()
ctest_memcheck()
