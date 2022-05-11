add_subdirectory(messages)

add_library(ssb_fapi_adaptor STATIC ssb.cpp)
target_link_libraries(ssb_fapi_adaptor srsvec)
