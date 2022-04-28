
set_directory_properties(PROPERTIES LABELS "fapi_adaptor")

add_executable(fapi_adaptor_test fapi_adaptor_test.cpp)
target_link_libraries(fapi_adaptor_test ssb_fapi_adaptor srslog)
add_test(fapi_adaptor_test fapi_adaptor_test)
