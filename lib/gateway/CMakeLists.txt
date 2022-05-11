#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(srsgnb_gateway sctp_network_gateway.cpp)
target_link_libraries(srsgnb_gateway srslog srsgnb_support srsgnb_du_manager srsgnb_mac srsgnb_f1)
