#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES gtpu_tunnel_ngu_factory.cpp
            gtpu_tunnel_nru_factory.cpp
            gtpu_pdu.cpp
            gtpu_demux_factory.cpp
            gtpu_echo_factory.cpp
            gtpu_teid_pool_factory.cpp
            gtpu_demux_impl.cpp
            gtpu_gateway.cpp)

add_library(srsran_gtpu STATIC ${SOURCES})
target_link_libraries(srsran_gtpu srsran_nru srsran_psup srsran_support srsran_gateway)
