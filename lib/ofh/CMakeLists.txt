#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(compression)
add_subdirectory(ecpri)
add_subdirectory(ethernet)
add_subdirectory(receiver)
add_subdirectory(serdes)
add_subdirectory(timing)
add_subdirectory(transmitter)

add_library(srsran_ofh ofh_factories.cpp ofh_sector_impl.cpp ofh_sector_controller.cpp)
target_link_libraries(srsran_ofh srsran_ofh_receiver srsran_ofh_timing srsran_ofh_transmitter srsran_ofh_message_serdes)

if (DPDK_FOUND)
    add_definitions(-DDPDK_FOUND)
endif (DPDK_FOUND)
