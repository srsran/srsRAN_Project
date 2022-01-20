set(SOURCES f1u_factory.cc)

add_library(srsran_f1_interface STATIC ${SOURCES})
target_link_libraries(srsran_f1_interface)
