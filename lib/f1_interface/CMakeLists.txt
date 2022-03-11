
set(SOURCES f1ap_du_factory.cc f1ap_du_impl.cc)

add_library(srsgnb_f1 STATIC ${SOURCES})
target_link_libraries(srsgnb_f1)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})