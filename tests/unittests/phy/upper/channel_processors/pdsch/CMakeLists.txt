#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_data)
set(TEST_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/test_data)

set_directory_properties(PROPERTIES LABELS "phy")

add_executable(pdsch_processor_unittest pdsch_processor_unittest.cpp)
target_link_libraries(pdsch_processor_unittest srsran_channel_processors srslog gtest gtest_main)
add_test(pdsch_processor_unittest pdsch_processor_unittest)

add_executable(pdsch_processor_validator_test pdsch_processor_validator_test.cpp)
target_link_libraries(pdsch_processor_validator_test
        srsran_channel_equalizer
        srsran_channel_processors
        srsran_phy_support
        srslog
        srsran_upper_phy_support
        gtest
        gtest_main)
add_test(pdsch_processor_validator_test pdsch_processor_validator_test)

if (USE_PHY_TESTVECTORS)
    add_executable(pdsch_encoder_test pdsch_encoder_test.cpp)
    set(PDSCH_ENCODER_LIBRARIES srsran_channel_processors srslog)

    if (ENABLE_PDSCH_HWACC)
        set_source_files_properties(pdsch_encoder_test.cpp PROPERTIES COMPILE_DEFINITIONS "HWACC_PDSCH_ENABLED")
        list(APPEND PDSCH_ENCODER_LIBRARIES srsran_hal_pdsch)
    endif (ENABLE_PDSCH_HWACC)

    target_link_libraries(pdsch_encoder_test ${PDSCH_ENCODER_LIBRARIES})
    add_test_vector(pdsch_encoder_test pdsch_encoder_test_data.tar.gz "")

    add_executable(pdsch_modulator_test pdsch_modulator_test.cpp)
    target_link_libraries(pdsch_modulator_test srsran_channel_processors srslog)
    add_test_vector(pdsch_modulator_test pdsch_modulator_test_data.tar.gz "")

    add_executable(pdsch_processor_vectortest pdsch_processor_vectortest.cpp)
    set(PDSCH_PROCESSOR_LIBRARIES srsran_channel_processors srslog gtest gtest_main)

    if (ENABLE_PDSCH_HWACC)
        set_source_files_properties(pdsch_processor_vectortest.cpp PROPERTIES COMPILE_DEFINITIONS "HWACC_PDSCH_ENABLED")
        list(APPEND PDSCH_PROCESSOR_LIBRARIES srsran_hal_pdsch)
    endif (ENABLE_PDSCH_HWACC)

    target_link_libraries(pdsch_processor_vectortest ${PDSCH_PROCESSOR_LIBRARIES})
    add_test_vector(pdsch_processor_vectortest pdsch_processor_test_data.tar.gz "")
endif (USE_PHY_TESTVECTORS)
