set(SOURCES sdap_factory.cpp sdap_entity_factory.cpp)

add_library(srsran_sdap STATIC ${SOURCES})
target_link_libraries(srsran_sdap)
