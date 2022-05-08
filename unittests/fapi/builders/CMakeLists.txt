
set_directory_properties(PROPERTIES LABELS "fapi")

add_executable(dl_pdcch_builder_test pdcch_test.cpp)
target_link_libraries(dl_pdcch_builder_test srsgnb_support srslog)
add_test(dl_pdcch_builder_test dl_pdcch_builder_test)
