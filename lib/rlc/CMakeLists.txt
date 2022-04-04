
set(SOURCES rlc_factory.cpp rlc_impl.cpp)

add_library(srsgnb_rlc STATIC ${SOURCES})
target_link_libraries(srsgnb_rlc)
