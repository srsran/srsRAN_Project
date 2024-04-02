#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "ofh")

add_executable(ofh_integration_test ofh_integration_test.cpp)
target_link_libraries(ofh_integration_test srslog srsran_ru_ofh srsran_phy_support srsran_support)
add_test(ofh_integration_test ofh_integration_test)
set_tests_properties(ofh_integration_test PROPERTIES LABELS "tsan;NO_MEMCHECK")

set (SOURCES ru_emulator_cli11_schema.cpp ru_emulator.cpp ru_emulator_socket_transceiver.cpp)
if (DPDK_FOUND)
    list(APPEND SOURCES ru_emulator_dpdk_transceiver.cpp)
endif (DPDK_FOUND)

add_executable(ru_emulator ${SOURCES})
target_include_directories(ru_emulator PRIVATE ${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/external)
target_link_libraries(ru_emulator srslog srsran_ofh_ethernet srsran_phy_support srsran_support)

if (DPDK_FOUND)
    target_compile_options(ru_emulator PRIVATE ${DPDK_CFLAGS})
    target_link_libraries(ru_emulator hal_dpdk ${DPDK_LIBRARIES})
    add_definitions(-DDPDK_FOUND)
endif (DPDK_FOUND)
