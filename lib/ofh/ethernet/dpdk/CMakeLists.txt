#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        dpdk_ethernet_port_context.cpp
        dpdk_ethernet_transmitter.cpp
        dpdk_ethernet_factories.cpp
        dpdk_ethernet_receiver.cpp)

add_library(srsran_ofh_dpdk_ethernet STATIC ${SOURCES})
target_compile_options(srsran_ofh_dpdk_ethernet PRIVATE ${DPDK_CFLAGS})
target_link_libraries(srsran_ofh_dpdk_ethernet srslog srsran_instrumentation ${DPDK_LIBRARIES})
