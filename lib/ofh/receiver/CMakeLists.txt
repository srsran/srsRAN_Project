#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        ofh_message_receiver.cpp
        ofh_uplane_uplink_packet_handler.cpp
        ofh_uplane_uplink_symbol_manager.cpp
        ofh_receiver_impl.cpp)

add_library(srsran_ofh_receiver STATIC ${SOURCES})
target_link_libraries(srsran_ofh_receiver srsran_ran srsran_ofh_message_serdes srslog)
