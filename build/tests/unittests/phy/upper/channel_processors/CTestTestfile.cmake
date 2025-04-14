# CMake generated Testfile for 
# Source directory: /home/shovon/srsRAN_Project/tests/unittests/phy/upper/channel_processors
# Build directory: /home/shovon/srsRAN_Project/build/tests/unittests/phy/upper/channel_processors
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(ssb_processor_unittest "ssb_processor_unittest")
set_tests_properties(ssb_processor_unittest PROPERTIES  _BACKTRACE_TRIPLES "/home/shovon/srsRAN_Project/tests/unittests/phy/upper/channel_processors/CMakeLists.txt;34;add_test;/home/shovon/srsRAN_Project/tests/unittests/phy/upper/channel_processors/CMakeLists.txt;0;")
subdirs("pdcch")
subdirs("pdsch")
subdirs("pucch")
subdirs("pusch")
subdirs("uci")
set_directory_properties(PROPERTIES LABELS "phy")
