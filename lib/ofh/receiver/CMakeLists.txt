#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        ofh_closed_rx_window_handler.cpp
        ofh_data_flow_uplane_uplink_data_impl.cpp
        ofh_data_flow_uplane_uplink_prach_impl.cpp
        ofh_message_receiver.cpp
        ofh_receiver_controller.cpp
        ofh_receiver_factories.cpp
        ofh_receiver_impl.cpp
        ofh_rx_window_checker.cpp
        ofh_uplane_rx_symbol_data_flow_notifier.cpp
        ofh_uplane_rx_symbol_data_flow_writer.cpp
        ofh_uplane_prach_data_flow_notifier.cpp
        ofh_uplane_prach_symbol_data_flow_writer.cpp)

add_library(srsran_ofh_receiver STATIC ${SOURCES})
target_link_libraries(srsran_ofh_receiver srsran_ran srsran_ofh_message_serdes srsran_ofh_compression
                      srsran_ofh_ethernet srsran_ofh_ecpri srslog srsran_instrumentation)
