#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(srsgnb_io_broker io_broker_factory.cpp io_broker_epoll.cpp)
target_link_libraries(srsgnb_io_broker srslog)
install(TARGETS srsgnb_io_broker EXPORT srsgnb_export)
