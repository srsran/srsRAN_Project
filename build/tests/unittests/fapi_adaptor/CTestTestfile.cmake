# CMake generated Testfile for 
# Source directory: /home/shovon/srsRAN_Project/tests/unittests/fapi_adaptor
# Build directory: /home/shovon/srsRAN_Project/build/tests/unittests/fapi_adaptor
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
include("/home/shovon/srsRAN_Project/build/tests/unittests/fapi_adaptor/precoding_matrix_table_generator_test[1]_include.cmake")
include("/home/shovon/srsRAN_Project/build/tests/unittests/fapi_adaptor/uci_part2_correspondence_test[1]_include.cmake")
add_test(fapi_adaptor_performance_dl_tti_request "fapi_adaptor_performance_dl_tti_request")
set_tests_properties(fapi_adaptor_performance_dl_tti_request PROPERTIES  _BACKTRACE_TRIPLES "/home/shovon/srsRAN_Project/tests/unittests/fapi_adaptor/CMakeLists.txt;27;add_test;/home/shovon/srsRAN_Project/tests/unittests/fapi_adaptor/CMakeLists.txt;0;")
subdirs("mac")
subdirs("phy")
set_directory_properties(PROPERTIES LABELS "fapi_adaptor")
