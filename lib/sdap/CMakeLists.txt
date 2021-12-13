set(SOURCES sdap_factory.cc sdap_entity_factory.cc)

add_library(srsran_sdap STATIC ${SOURCES})
target_link_libraries(srsran_sdap)
