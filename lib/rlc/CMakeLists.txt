
set(SOURCES rlc_factory.cc rlc_impl.cc)

add_library(srsgnb_rlc STATIC ${SOURCES})
target_link_libraries(srsgnb_rlc)
