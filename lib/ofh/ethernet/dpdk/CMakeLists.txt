#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        dpdk_ethernet_transmitter.cpp
        dpdk_ethernet_factories.cpp
        dpdk_ethernet_receiver.cpp)

add_library(srsran_ofh_dpdk_ethernet STATIC ${SOURCES})
target_link_libraries(srsran_ofh_dpdk_ethernet srslog ${DPDK_LIBRARIES})
