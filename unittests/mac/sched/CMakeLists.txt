
set_directory_properties(PROPERTIES LABELS "sched")

include_directories(../../../)

add_executable(sched_prb_test sched_prb_test.cc)
target_link_libraries(sched_prb_test srsgnb_sched srslog)
add_test(sched_prb_test sched_prb_test)

add_executable(sched_harq_test sched_harq_test.cc)
target_link_libraries(sched_harq_test srsgnb_sched srslog)
add_test(sched_harq_test sched_prb_test)

add_executable(sched_no_ue_test sched_no_ue_test.cc)
target_link_libraries(sched_no_ue_test srsgnb_sched srslog)
add_test(sched_no_ue_test sched_no_ue_test)

add_executable(sched_signalling_test sched_signalling_test.cc)
target_link_libraries(sched_signalling_test srsgnb_sched srslog)
add_test(sched_signalling_test sched_signalling_test)

add_executable(cell_configuration_test cell_configuration_test.cc)
target_link_libraries(cell_configuration_test srsgnb_sched srslog)
add_test(cell_configuration_test cell_configuration_test)
