
set_directory_properties(PROPERTIES LABELS "du_manager")

add_executable(ue_manager_test ue_manager_test.cc)
target_link_libraries(ue_manager_test srsgnb_du_manager srsgnb_support srslog f1ap_asn1)
target_include_directories(ue_manager_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(ue_manager_test ue_manager_test)
