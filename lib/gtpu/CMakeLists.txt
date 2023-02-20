#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES gtpu_tunnel_factory.cpp
            gtpu_pdu.cpp
            gtpu_demux_factory.cpp
            gtpu_demux_impl.cpp)

add_library(srsran_gtpu STATIC ${SOURCES})
target_link_libraries(srsran_gtpu srsran_support)
