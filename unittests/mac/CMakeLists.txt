
add_subdirectory(sched)

set_directory_properties(PROPERTIES LABELS "mac")

add_executable(mac_ue_create_test mac_ue_create_test.cc)
target_link_libraries(mac_ue_create_test srsgnb_support srslog)
add_test(mac_ue_create_test mac_ue_create_test)

add_executable(mac_ctrl_test mac_ctrl_test.cc)
target_link_libraries(mac_ctrl_test srsgnb_mac srsgnb_support srslog)
add_test(mac_ctrl_test mac_ctrl_test)

add_executable(mac_dl_cfg_test mac_dl_cfg_test.cc)
target_link_libraries(mac_dl_cfg_test srsgnb_mac srsgnb_support srslog)
add_test(mac_dl_cfg_test mac_dl_cfg_test)
