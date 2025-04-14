# CMake generated Testfile for 
# Source directory: /home/shovon/srsRAN_Project/tests/integrationtests/rlc
# Build directory: /home/shovon/srsRAN_Project/build/tests/integrationtests/rlc
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(rlc_um6_eia2_eea2_stress_test "rlc_stress_test" "--mode=UM6" "--pdcp_integrity_algo=2" "--pdcp_ciphering_algo=2")
set_tests_properties(rlc_um6_eia2_eea2_stress_test PROPERTIES  LABELS "tsan" _BACKTRACE_TRIPLES "/home/shovon/srsRAN_Project/tests/integrationtests/rlc/CMakeLists.txt;38;add_test;/home/shovon/srsRAN_Project/tests/integrationtests/rlc/CMakeLists.txt;0;")
add_test(rlc_um12_eia2_eea2_stress_test "rlc_stress_test" "--mode=UM12" "--pdcp_integrity_algo=2" "--pdcp_ciphering_algo=2")
set_tests_properties(rlc_um12_eia2_eea2_stress_test PROPERTIES  LABELS "tsan" _BACKTRACE_TRIPLES "/home/shovon/srsRAN_Project/tests/integrationtests/rlc/CMakeLists.txt;42;add_test;/home/shovon/srsRAN_Project/tests/integrationtests/rlc/CMakeLists.txt;0;")
add_test(rlc_am12_eia2_eea2_stress_test "rlc_stress_test" "--mode=AM12" "--pdcp_integrity_algo=2" "--pdcp_ciphering_algo=2")
set_tests_properties(rlc_am12_eia2_eea2_stress_test PROPERTIES  LABELS "tsan" _BACKTRACE_TRIPLES "/home/shovon/srsRAN_Project/tests/integrationtests/rlc/CMakeLists.txt;46;add_test;/home/shovon/srsRAN_Project/tests/integrationtests/rlc/CMakeLists.txt;0;")
add_test(rlc_am18_eia2_eea2_stress_test "rlc_stress_test" "--mode=AM18" "--pdcp_integrity_algo=2" "--pdcp_ciphering_algo=2")
set_tests_properties(rlc_am18_eia2_eea2_stress_test PROPERTIES  LABELS "tsan" _BACKTRACE_TRIPLES "/home/shovon/srsRAN_Project/tests/integrationtests/rlc/CMakeLists.txt;50;add_test;/home/shovon/srsRAN_Project/tests/integrationtests/rlc/CMakeLists.txt;0;")
add_test(rlc_um12_eia1_eea1_stress_test "rlc_stress_test" "--mode=UM12" "--pdcp_integrity_algo=1" "--pdcp_ciphering_algo=1" "--nof_ttis=50")
set_tests_properties(rlc_um12_eia1_eea1_stress_test PROPERTIES  LABELS "tsan" _BACKTRACE_TRIPLES "/home/shovon/srsRAN_Project/tests/integrationtests/rlc/CMakeLists.txt;59;add_test;/home/shovon/srsRAN_Project/tests/integrationtests/rlc/CMakeLists.txt;0;")
add_test(rlc_um12_eia3_eea3_stress_test "rlc_stress_test" "--mode=UM12" "--pdcp_integrity_algo=3" "--pdcp_ciphering_algo=3")
set_tests_properties(rlc_um12_eia3_eea3_stress_test PROPERTIES  LABELS "tsan" _BACKTRACE_TRIPLES "/home/shovon/srsRAN_Project/tests/integrationtests/rlc/CMakeLists.txt;63;add_test;/home/shovon/srsRAN_Project/tests/integrationtests/rlc/CMakeLists.txt;0;")
add_test(rlc_um12_eia2_eea0_stress_test "rlc_stress_test" "--mode=UM12" "--pdcp_integrity_algo=2" "--pdcp_ciphering_algo=0")
set_tests_properties(rlc_um12_eia2_eea0_stress_test PROPERTIES  LABELS "tsan" _BACKTRACE_TRIPLES "/home/shovon/srsRAN_Project/tests/integrationtests/rlc/CMakeLists.txt;67;add_test;/home/shovon/srsRAN_Project/tests/integrationtests/rlc/CMakeLists.txt;0;")
set_directory_properties(PROPERTIES LABELS "rlc")
