set(ENV{CC} "clang")
set(ENV{CXX} "clang++")
set(ENV{CXXFLAGS} "-Wall -Wpedantic")

set(CTEST_SOURCE_DIRECTORY ".")
set(CTEST_BINARY_DIRECTORY "build-test-clang")
set(CTEST_CMAKE_GENERATOR "Unix Makefiles")

ctest_start("Continuous")
ctest_configure()
ctest_build()
ctest_test()
