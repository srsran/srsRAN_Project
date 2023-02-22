#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

find_package(SCTP REQUIRED)

add_library(srsran_gateway
              sctp_network_gateway_impl.cpp
              udp_network_gateway_impl.cpp
              sctp_network_gateway_factory.cpp
              udp_network_gateway_factory.cpp
            )
target_link_libraries(srsran_gateway srslog ${SCTP_LIBRARIES})