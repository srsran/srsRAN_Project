add_library(pdsch_fapi_adaptor STATIC pdsch.cpp)
target_link_libraries(pdsch_fapi_adaptor)

add_subdirectory(messages)

add_library(ssb_fapi_adaptor STATIC ssb.cpp)
target_link_libraries(ssb_fapi_adaptor srsvec)


