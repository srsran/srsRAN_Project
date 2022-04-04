set(SOURCES pdcp_factory.cpp pdcp_entity_factory.cpp)

add_library(srsran_pdcp STATIC ${SOURCES})
target_link_libraries(srsran_pdcp)
