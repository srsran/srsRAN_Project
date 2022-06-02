#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "sched")

include_directories(../..)

add_executable(sched_prb_test sched_prb_test.cpp)
target_link_libraries(sched_prb_test srsgnb_sched srslog)
add_test(sched_prb_test sched_prb_test)

add_executable(sched_harq_test sched_harq_test.cpp)
target_link_libraries(sched_harq_test srsgnb_sched srslog)
add_test(sched_harq_test sched_prb_test)

add_executable(sched_no_ue_test sched_no_ue_test.cpp)
target_link_libraries(sched_no_ue_test srsgnb_sched srslog mac_configuration_helpers)
add_test(sched_no_ue_test sched_no_ue_test)

add_executable(sched_signalling_test sched_signalling_test.cpp)
target_link_libraries(sched_signalling_test srsgnb_sched srslog mac_configuration_helpers)
add_test(sched_signalling_test sched_signalling_test)

add_executable(cell_configuration_test cell_configuration_test.cpp)
target_link_libraries(cell_configuration_test srsgnb_sched srslog mac_configuration_helpers)
add_test(cell_configuration_test cell_configuration_test)

add_executable(resource_allocator_test resource_allocator_test.cpp)
target_link_libraries(resource_allocator_test srsgnb_sched srslog mac_configuration_helpers)
add_test(resource_allocator_test resource_allocator_test)

add_executable(ra_scheduler_test ra_scheduler_test.cpp)
target_link_libraries(ra_scheduler_test srsgnb_sched srslog rrc_nr_asn1 mac_configuration_helpers)
add_test(ra_scheduler_test ra_scheduler_test)

add_executable(pdcch_scheduler_test pdcch_scheduler_test.cpp)
target_link_libraries(pdcch_scheduler_test srsgnb_sched srslog mac_configuration_helpers)
add_test(pdcch_scheduler_test pdcch_scheduler_test)

add_executable(ue_configuration_test ue_configuration_test.cpp)
target_link_libraries(ue_configuration_test srsgnb_sched srslog mac_configuration_helpers)
add_test(ue_configuration_test ue_configuration_test)

add_executable(sib1_scheduler_test sib1_scheduler_test.cpp)
target_link_libraries(sib1_scheduler_test srsgnb_sched srslog rrc_nr_asn1 mac_configuration_helpers)
add_test(sib1_scheduler_test sib1_scheduler_test)