# CMake generated Testfile for 
# Source directory: /home/shovon/srsRAN_Project/tests/integrationtests/ofh
# Build directory: /home/shovon/srsRAN_Project/build/tests/integrationtests/ofh
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(ofh_integration_test "ofh_integration_test" "-d" "'{0,1}'")
set_tests_properties(ofh_integration_test PROPERTIES  LABELS "tsan;NO_MEMCHECK" _BACKTRACE_TRIPLES "/home/shovon/srsRAN_Project/tests/integrationtests/ofh/CMakeLists.txt;30;add_test;/home/shovon/srsRAN_Project/tests/integrationtests/ofh/CMakeLists.txt;0;")
set_directory_properties(PROPERTIES LABELS "ofh")
