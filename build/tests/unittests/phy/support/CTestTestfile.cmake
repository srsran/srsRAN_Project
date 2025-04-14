# CMake generated Testfile for 
# Source directory: /home/shovon/srsRAN_Project/tests/unittests/phy/support
# Build directory: /home/shovon/srsRAN_Project/build/tests/unittests/phy/support
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
include("/home/shovon/srsRAN_Project/build/tests/unittests/phy/support/resource_grid_mapper_test[1]_include.cmake")
include("/home/shovon/srsRAN_Project/build/tests/unittests/phy/support/precoding_configuration_test[1]_include.cmake")
add_test(resource_grid_pool_test "resource_grid_pool_test")
set_tests_properties(resource_grid_pool_test PROPERTIES  _BACKTRACE_TRIPLES "/home/shovon/srsRAN_Project/tests/unittests/phy/support/CMakeLists.txt;25;add_test;/home/shovon/srsRAN_Project/tests/unittests/phy/support/CMakeLists.txt;0;")
add_test(resource_grid_test "resource_grid_test")
set_tests_properties(resource_grid_test PROPERTIES  _BACKTRACE_TRIPLES "/home/shovon/srsRAN_Project/tests/unittests/phy/support/CMakeLists.txt;29;add_test;/home/shovon/srsRAN_Project/tests/unittests/phy/support/CMakeLists.txt;0;")
add_test(interpolator_test "interpolator_test")
set_tests_properties(interpolator_test PROPERTIES  _BACKTRACE_TRIPLES "/home/shovon/srsRAN_Project/tests/unittests/phy/support/CMakeLists.txt;33;add_test;/home/shovon/srsRAN_Project/tests/unittests/phy/support/CMakeLists.txt;0;")
add_test(re_pattern_test "re_pattern_test")
set_tests_properties(re_pattern_test PROPERTIES  _BACKTRACE_TRIPLES "/home/shovon/srsRAN_Project/tests/unittests/phy/support/CMakeLists.txt;56;add_test;/home/shovon/srsRAN_Project/tests/unittests/phy/support/CMakeLists.txt;0;")
set_directory_properties(PROPERTIES LABELS "phy")
