add_library(srsgnb_radio STATIC radio_uhd_impl.cpp)
target_link_libraries(srsgnb_radio srsvec)
