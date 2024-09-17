#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        ethernet_factories.cpp
        ethernet_frame_builder_impl.cpp
        ethernet_transmitter_impl.cpp
        ethernet_receiver_impl.cpp
        ethernet_rx_buffer_impl.cpp
        vlan_ethernet_frame_builder_impl.cpp
        vlan_ethernet_frame_decoder_impl.cpp)

add_library(srsran_ofh_ethernet STATIC ${SOURCES})
target_link_libraries(srsran_ofh_ethernet srslog srsran_instrumentation)

if (DPDK_FOUND)
    add_subdirectory(dpdk)
    target_link_libraries(srsran_ofh_ethernet srsran_ofh_dpdk_ethernet)
endif (DPDK_FOUND)
