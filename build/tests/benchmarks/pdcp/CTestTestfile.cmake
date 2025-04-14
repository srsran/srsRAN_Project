# CMake generated Testfile for 
# Source directory: /home/shovon/srsRAN_Project/tests/benchmarks/pdcp
# Build directory: /home/shovon/srsRAN_Project/build/tests/benchmarks/pdcp
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(pdcp_tx_benchmark_nea0 "pdcp_tx_benchmark" "-a0" "-R3")
set_tests_properties(pdcp_tx_benchmark_nea0 PROPERTIES  LABELS "tsan" _BACKTRACE_TRIPLES "/home/shovon/srsRAN_Project/tests/benchmarks/pdcp/CMakeLists.txt;27;add_test;/home/shovon/srsRAN_Project/tests/benchmarks/pdcp/CMakeLists.txt;0;")
add_test(pdcp_tx_benchmark_nea1 "pdcp_tx_benchmark" "-a1" "-R1")
set_tests_properties(pdcp_tx_benchmark_nea1 PROPERTIES  LABELS "tsan" _BACKTRACE_TRIPLES "/home/shovon/srsRAN_Project/tests/benchmarks/pdcp/CMakeLists.txt;28;add_test;/home/shovon/srsRAN_Project/tests/benchmarks/pdcp/CMakeLists.txt;0;")
add_test(pdcp_tx_benchmark_nea2 "pdcp_tx_benchmark" "-a2" "-R3")
set_tests_properties(pdcp_tx_benchmark_nea2 PROPERTIES  LABELS "tsan" _BACKTRACE_TRIPLES "/home/shovon/srsRAN_Project/tests/benchmarks/pdcp/CMakeLists.txt;29;add_test;/home/shovon/srsRAN_Project/tests/benchmarks/pdcp/CMakeLists.txt;0;")
add_test(pdcp_tx_benchmark_nea3 "pdcp_tx_benchmark" "-a3" "-R1")
set_tests_properties(pdcp_tx_benchmark_nea3 PROPERTIES  LABELS "tsan" _BACKTRACE_TRIPLES "/home/shovon/srsRAN_Project/tests/benchmarks/pdcp/CMakeLists.txt;30;add_test;/home/shovon/srsRAN_Project/tests/benchmarks/pdcp/CMakeLists.txt;0;")
add_test(pdcp_rx_benchmark_nea0 "pdcp_rx_benchmark" "-a0" "-R3")
set_tests_properties(pdcp_rx_benchmark_nea0 PROPERTIES  LABELS "tsan" _BACKTRACE_TRIPLES "/home/shovon/srsRAN_Project/tests/benchmarks/pdcp/CMakeLists.txt;38;add_test;/home/shovon/srsRAN_Project/tests/benchmarks/pdcp/CMakeLists.txt;0;")
add_test(pdcp_rx_benchmark_nea1 "pdcp_rx_benchmark" "-a1" "-R1")
set_tests_properties(pdcp_rx_benchmark_nea1 PROPERTIES  LABELS "tsan" _BACKTRACE_TRIPLES "/home/shovon/srsRAN_Project/tests/benchmarks/pdcp/CMakeLists.txt;39;add_test;/home/shovon/srsRAN_Project/tests/benchmarks/pdcp/CMakeLists.txt;0;")
add_test(pdcp_rx_benchmark_nea2 "pdcp_rx_benchmark" "-a2" "-R3")
set_tests_properties(pdcp_rx_benchmark_nea2 PROPERTIES  LABELS "tsan" _BACKTRACE_TRIPLES "/home/shovon/srsRAN_Project/tests/benchmarks/pdcp/CMakeLists.txt;40;add_test;/home/shovon/srsRAN_Project/tests/benchmarks/pdcp/CMakeLists.txt;0;")
add_test(pdcp_rx_benchmark_nea3 "pdcp_rx_benchmark" "-a3" "-R1")
set_tests_properties(pdcp_rx_benchmark_nea3 PROPERTIES  LABELS "tsan" _BACKTRACE_TRIPLES "/home/shovon/srsRAN_Project/tests/benchmarks/pdcp/CMakeLists.txt;41;add_test;/home/shovon/srsRAN_Project/tests/benchmarks/pdcp/CMakeLists.txt;0;")
set_directory_properties(PROPERTIES LABELS "pdcp")
