#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "cu;du;integrationtest")

add_executable(cu_du_test cu_du_test.cpp du_high_cu_test_simulator.cpp cu_multi_du_test.cpp)
target_link_libraries(cu_du_test srsran_cu_cp
        srsran_du_high
        du_high_worker_manager
        srsran_f1ap_cu
        mac_test_doubles
        f1ap_test_doubles
        du_test_doubles
        cu_cp_test_helpers
        srsran_rrc
        srsran_support
        srslog
        ngap_test_helpers
        f1ap_asn1
        ngap_asn1
        gtest
        gtest_main)
target_include_directories(cu_du_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(cu_du_test)
