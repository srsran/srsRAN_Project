#
# Copyright 2021-2024 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

add_subdirectory(channel_coding)
add_subdirectory(channel_modulation)
add_subdirectory(channel_processors)
add_subdirectory(equalization)
add_subdirectory(sequence_generators)
add_subdirectory(signal_processors)

add_library(srsran_downlink_processor
        downlink_processor_single_executor_impl.cpp
        downlink_processor_pool_impl.cpp)
target_link_libraries(srsran_downlink_processor
        srsran_instrumentation
        srsran_channel_processors)

add_library(srsran_uplink_processor
        uplink_processor_impl.cpp
        uplink_processor_pool_impl.cpp
        uplink_request_processor_impl.cpp)
target_link_libraries(srsran_uplink_processor srsran_channel_processors)

add_library(srsran_upper_phy upper_phy_factories.cpp upper_phy_impl.cpp upper_phy_factories.cpp
                             upper_phy_rx_symbol_handler_impl.cpp upper_phy_rx_results_notifier_wrapper.cpp
                             upper_phy_error_handler_impl.cpp)
target_link_libraries(srsran_upper_phy
        srsran_instrumentation
        srsran_downlink_processor
        srsran_uplink_processor
        srsran_phy_support
        srsran_generic_funcs
        srsran_channel_equalizer)

add_library(log_likelihood_ratio log_likelihood_ratio.cpp)
target_link_libraries(log_likelihood_ratio srsvec)

add_library(srsran_upper_phy_support
        rb_allocation.cpp
        rx_buffer_pool_impl.cpp
        vrb_to_prb_mapper.cpp
        )

install(TARGETS log_likelihood_ratio
        srsran_upper_phy_support
        EXPORT srsran_export)
