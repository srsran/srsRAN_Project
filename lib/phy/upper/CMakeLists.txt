#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(channel_coding)
add_subdirectory(channel_modulation)
add_subdirectory(channel_processors)
add_subdirectory(sequence_generators)
add_subdirectory(signal_processors)
add_subdirectory(equalization)

add_library(srsran_downlink_processor downlink_processor_single_executor_impl.cpp downlink_processor_pool_impl.cpp)
target_link_libraries(srsran_downlink_processor srsran_channel_processors)

add_library(srsran_uplink_processor
        uplink_processor_impl.cpp
        uplink_processor_pool_impl.cpp
        uplink_request_processor_impl.cpp)
target_link_libraries(srsran_uplink_processor srsran_channel_processors)

add_library(srsran_upper_phy upper_phy_factories.cpp upper_phy_impl.cpp upper_phy_factories.cpp upper_phy_rx_symbol_handler_impl.cpp upper_phy_rx_results_notifier_wrapper.cpp)
target_link_libraries(srsran_upper_phy srsran_downlink_processor srsran_uplink_processor srsran_phy_support srsran_generic_funcs srsran_channel_equalizer)

add_library(log_likelihood_ratio log_likelihood_ratio.cpp)

add_library(srsran_upper_phy_support
        re_pattern.cpp
        rb_allocation.cpp
        rx_softbuffer_pool_impl.cpp
        vrb_to_prb_mapper.cpp
        )