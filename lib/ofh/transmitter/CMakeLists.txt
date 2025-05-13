#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        ofh_data_flow_cplane_scheduling_commands_impl.cpp
        ofh_data_flow_uplane_downlink_data_impl.cpp
        ofh_downlink_handler_impl.cpp
        ofh_message_transmitter_impl.cpp
        ofh_uplane_fragment_size_calculator.cpp
        ofh_uplink_request_handler_impl.cpp
        ofh_uplink_request_handler_task_dispatcher.cpp
        ofh_transmitter_factories.cpp
        ofh_transmitter_impl.cpp)

add_library(srsran_ofh_transmitter STATIC ${SOURCES})
target_link_libraries(srsran_ofh_transmitter
        srsran_ran
        srsran_instrumentation
        srslog
        srsvec)
