
add_executable(dmrs_pbch_processor_test dmrs_pbch_processor_test.cpp)
target_link_libraries(dmrs_pbch_processor_test signal_processors srsvec sequence_generators srslog)
add_test(dmrs_pbch_processor_test dmrs_pbch_processor_test)

add_custom_command(
  TARGET dmrs_pbch_processor_test
  POST_BUILD
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/unittests/phy/upper/signal_processors/
  COMMAND ${CMAKE_COMMAND} -E tar xvf ${CMAKE_CURRENT_SOURCE_DIR}/dmrs_pbch_processor_test/data/dmrs_pbch_processor_test_data.tar.gz
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/dmrs_pbch_processor_test/data/dmrs_pbch_processor_test_data.tar.gz
  VERBATIM)

add_executable(dmrs_pdsch_processor_test dmrs_pdsch_processor_test.cpp)
target_link_libraries(dmrs_pdsch_processor_test signal_processors srsvec sequence_generators srslog)
add_test(dmrs_pdsch_processor_test dmrs_pdsch_processor_test)

add_executable(pss_processor_test pss_processor_test.cpp)
target_link_libraries(pss_processor_test signal_processors srsvec srslog)
add_test(pss_processor_test pss_processor_test)

add_executable(sss_processor_test sss_processor_test.cpp)
target_link_libraries(sss_processor_test signal_processors srsvec srslog)
add_test(sss_processor_test sss_processor_test)
