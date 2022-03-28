
set(SOURCES
    f1ap_du_factory.cc
    f1ap_du_impl.cc
    f1ap_asn1_packer.cc
    handlers/f1c_du_packet_handler.cc
    )

add_library(srsgnb_f1 STATIC ${SOURCES})
target_link_libraries(srsgnb_f1 f1ap_asn1)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})