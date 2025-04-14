# CMake generated Testfile for 
# Source directory: /home/shovon/srsRAN_Project/tests/unittests/phy/upper/channel_processors/pdsch
# Build directory: /home/shovon/srsRAN_Project/build/tests/unittests/phy/upper/channel_processors/pdsch
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(pdsch_processor_unittest "pdsch_processor_unittest")
set_tests_properties(pdsch_processor_unittest PROPERTIES  _BACKTRACE_TRIPLES "/home/shovon/srsRAN_Project/tests/unittests/phy/upper/channel_processors/pdsch/CMakeLists.txt;28;add_test;/home/shovon/srsRAN_Project/tests/unittests/phy/upper/channel_processors/pdsch/CMakeLists.txt;0;")
add_test(pdsch_processor_validator_test "pdsch_processor_validator_test")
set_tests_properties(pdsch_processor_validator_test PROPERTIES  _BACKTRACE_TRIPLES "/home/shovon/srsRAN_Project/tests/unittests/phy/upper/channel_processors/pdsch/CMakeLists.txt;40;add_test;/home/shovon/srsRAN_Project/tests/unittests/phy/upper/channel_processors/pdsch/CMakeLists.txt;0;")
set_directory_properties(PROPERTIES LABELS "phy")
