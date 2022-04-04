set(SOURCES du_manager_factory.cpp du_manager_impl.cpp du_ue_manager.cpp converters/mac_cell_asn1_helpers.cpp)

add_library(srsgnb_du_manager STATIC ${SOURCES})
target_link_libraries(srsgnb_du_manager srsgnb_rlc)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
