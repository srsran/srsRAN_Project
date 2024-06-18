#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(srsran_network
        transport_layer_address.cpp
        io_broker_factory.cpp
        io_broker_epoll.cpp
        io_timer_source.cpp
        sctp_socket.cpp
        sockets.cpp)
target_link_libraries(srsran_network srslog)

add_to_exported_libs(srsran_network)
