
add_executable(pbch_encoder_test pbch_encoder_test.cpp)
target_link_libraries(pbch_encoder_test pbch_encoder srslog)
add_test(pbch_encoder_test pbch_encoder_test)

add_executable(pbch_modulator_test pbch_modulator_test.cpp)
target_link_libraries(pbch_modulator_test pbch_modulator srslog)
add_test(pbch_modulator_test pbch_modulator_test)

add_custom_command(
  TARGET pbch_modulator_test
  POST_BUILD
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/unittests/phy/upper/channel_processors/
  COMMAND ${CMAKE_COMMAND} -E tar xvf ${CMAKE_CURRENT_SOURCE_DIR}/pbch_modulator_test/data/pbch_modulator_test_data.tar.gz
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/pbch_modulator_test/data/pbch_modulator_test_data.tar.gz
  VERBATIM)

add_executable(pdcch_modulator_test pdcch_modulator_test.cpp)
target_link_libraries(pdcch_modulator_test upper_phy pdcch_modulator srslog modulation_mapper pseudo_random_generator)
add_test(pdcch_modulator_test pdcch_modulator_test)

add_executable(pdsch_modulator_test pdsch_modulator_test.cpp)
target_link_libraries(pdsch_modulator_test upper_phy pdsch_modulator srslog)
add_test(pdsch_modulator_test pdsch_modulator_test)

add_executable(ssb_processor_test ssb_processor_test.cpp)
target_link_libraries(ssb_processor_test ssb_processor srslog)
add_test(ssb_processor_test ssb_processor_test)
