set(SOURCES pdcp_factory.cc pdcp_entity_factory.cc)

add_library(srsran_pdcp STATIC ${SOURCES})
target_link_libraries(srsran_pdcp)
