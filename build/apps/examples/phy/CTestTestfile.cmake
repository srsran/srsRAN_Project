# CMake generated Testfile for 
# Source directory: /home/shovon/srsRAN_Project/apps/examples/phy
# Build directory: /home/shovon/srsRAN_Project/build/apps/examples/phy
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(radio_ssb_zmq_20MHz_n7 "radio_ssb" "-P" "zmq_20MHz_n7" "-D" "10" "-L" "1" "-v" "warning" "-du")
set_tests_properties(radio_ssb_zmq_20MHz_n7 PROPERTIES  _BACKTRACE_TRIPLES "/home/shovon/srsRAN_Project/apps/examples/phy/CMakeLists.txt;39;add_test;/home/shovon/srsRAN_Project/apps/examples/phy/CMakeLists.txt;0;")
add_test(radio_ssb_zmq_20MHz_n78 "radio_ssb" "-P" "zmq_20MHz_n78" "-D" "10" "-L" "1" "-v" "warning" "-du")
set_tests_properties(radio_ssb_zmq_20MHz_n78 PROPERTIES  _BACKTRACE_TRIPLES "/home/shovon/srsRAN_Project/apps/examples/phy/CMakeLists.txt;40;add_test;/home/shovon/srsRAN_Project/apps/examples/phy/CMakeLists.txt;0;")
set_directory_properties(PROPERTIES LABELS "phy")
