#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES gtpu_tunnel_factory.cpp
            gtpu_pdu.cpp
            gtpu_demux_factory.cpp
            gtpu_demux_impl.cpp)

add_library(srsgnb_gtpu STATIC ${SOURCES})
target_link_libraries(srsgnb_gtpu srsgnb_support)
