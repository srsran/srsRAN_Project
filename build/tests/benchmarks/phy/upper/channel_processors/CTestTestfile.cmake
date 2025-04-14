# CMake generated Testfile for 
# Source directory: /home/shovon/srsRAN_Project/tests/benchmarks/phy/upper/channel_processors
# Build directory: /home/shovon/srsRAN_Project/build/tests/benchmarks/phy/upper/channel_processors
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(pdsch_processor_benchmark_generic_qpsk "pdsch_processor_benchmark" "-m" "silent" "-R" "1" "-B" "1" "-T" "2" "-P" "pdsch_scs15_5MHz_qpsk_min" "-t" "generic")
set_tests_properties(pdsch_processor_benchmark_generic_qpsk PROPERTIES  _BACKTRACE_TRIPLES "/home/shovon/srsRAN_Project/tests/benchmarks/phy/upper/channel_processors/CMakeLists.txt;54;add_test;/home/shovon/srsRAN_Project/tests/benchmarks/phy/upper/channel_processors/CMakeLists.txt;0;")
add_test(pdsch_processor_benchmark_generic_256qam "pdsch_processor_benchmark" "-m" "silent" "-R" "1" "-B" "1" "-T" "2" "-P" "2port_2layer_scs30_100MHz_256qam" "-t" "generic")
set_tests_properties(pdsch_processor_benchmark_generic_256qam PROPERTIES  _BACKTRACE_TRIPLES "/home/shovon/srsRAN_Project/tests/benchmarks/phy/upper/channel_processors/CMakeLists.txt;55;add_test;/home/shovon/srsRAN_Project/tests/benchmarks/phy/upper/channel_processors/CMakeLists.txt;0;")
add_test(pdsch_processor_benchmark_flexible "pdsch_processor_benchmark" "-m" "silent" "-R" "1" "-B" "1" "-T" "2" "-P" "pdsch_scs15_5MHz_qpsk_min" "-t" "flexible:4.0")
set_tests_properties(pdsch_processor_benchmark_flexible PROPERTIES  _BACKTRACE_TRIPLES "/home/shovon/srsRAN_Project/tests/benchmarks/phy/upper/channel_processors/CMakeLists.txt;56;add_test;/home/shovon/srsRAN_Project/tests/benchmarks/phy/upper/channel_processors/CMakeLists.txt;0;")
add_test(pdsch_processor_benchmark_lite "pdsch_processor_benchmark" "-m" "silent" "-R" "1" "-B" "1" "-T" "2" "-P" "pdsch_scs15_5MHz_qpsk_min" "-t" "lite")
set_tests_properties(pdsch_processor_benchmark_lite PROPERTIES  _BACKTRACE_TRIPLES "/home/shovon/srsRAN_Project/tests/benchmarks/phy/upper/channel_processors/CMakeLists.txt;57;add_test;/home/shovon/srsRAN_Project/tests/benchmarks/phy/upper/channel_processors/CMakeLists.txt;0;")
add_test(prach_detector_benchmark "prach_detector_benchmark" "-R" "1")
set_tests_properties(prach_detector_benchmark PROPERTIES  _BACKTRACE_TRIPLES "/home/shovon/srsRAN_Project/tests/benchmarks/phy/upper/channel_processors/CMakeLists.txt;65;add_test;/home/shovon/srsRAN_Project/tests/benchmarks/phy/upper/channel_processors/CMakeLists.txt;0;")
subdirs("pucch")
subdirs("pusch")
