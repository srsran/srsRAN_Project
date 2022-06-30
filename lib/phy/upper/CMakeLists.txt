#
# Copyright 2013-2022 Software Radio Systems Limited
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

add_library(srsgnb_downlink_processor downlink_processor_single_executor_impl.cpp downlink_processor_pool_impl.cpp downlink_processor_factory_impl.cpp)

add_library(log_likelihood_ratio log_likelihood_ratio.cpp)

add_library(srsgnb_upper_phy_support
        re_pattern.cpp
        rb_allocation.cpp
        rx_softbuffer_pool_impl.cpp
        vrb_to_prb_mapper.cpp
        )

install(TARGETS log_likelihood_ratio
                srsgnb_upper_phy_support
        EXPORT srsgnb_export)
