# CMake generated Testfile for 
# Source directory: /home/shovon/srsRAN_Project/tests/benchmarks/phy/generic_functions
# Build directory: /home/shovon/srsRAN_Project/build/tests/benchmarks/phy/generic_functions
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(dft_processor_generic_benchmark "dft_processor_benchmark" "-F" "generic" "-R" "10" "-s")
set_tests_properties(dft_processor_generic_benchmark PROPERTIES  _BACKTRACE_TRIPLES "/home/shovon/srsRAN_Project/tests/benchmarks/phy/generic_functions/CMakeLists.txt;26;add_test;/home/shovon/srsRAN_Project/tests/benchmarks/phy/generic_functions/CMakeLists.txt;0;")
add_test(dft_processor_fftw_benchmark "dft_processor_benchmark" "-F" "fftw" "-R" "10" "-s")
set_tests_properties(dft_processor_fftw_benchmark PROPERTIES  _BACKTRACE_TRIPLES "/home/shovon/srsRAN_Project/tests/benchmarks/phy/generic_functions/CMakeLists.txt;28;add_test;/home/shovon/srsRAN_Project/tests/benchmarks/phy/generic_functions/CMakeLists.txt;0;")
set_directory_properties(PROPERTIES LABELS "phy")
