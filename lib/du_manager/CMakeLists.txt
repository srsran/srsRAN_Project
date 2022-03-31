set(SOURCES du_manager_factory.cc du_manager_impl.cc du_ue_manager.cc converters/mac_cell_asn1_helpers.cc)

add_library(srsgnb_du_manager STATIC ${SOURCES})
target_link_libraries(srsgnb_du_manager srsgnb_rlc)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})