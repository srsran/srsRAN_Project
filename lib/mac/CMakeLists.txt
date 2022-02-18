
add_subdirectory(sched)

set(SOURCES mac_factory.cc mac_impl.cc)

add_library(srsgnb_mac STATIC ${SOURCES})
target_link_libraries(srsgnb_mac srsgnb_sched)
