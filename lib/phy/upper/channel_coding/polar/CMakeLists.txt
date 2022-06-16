#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

include_directories(${PROJECT_SOURCE_DIR}/include/srsgnb/phy/upper/channel_coding/polar)

add_library(srsgnb_polar STATIC
        polar_allocator_impl.cpp
        polar_code_impl.cpp
        polar_deallocator_impl.cpp
        polar_decoder_impl.cpp
        polar_encoder_impl.cpp
        polar_interleaver_impl.cpp
        polar_rate_dematcher_impl.cpp
        polar_rate_matcher_impl.cpp
)
target_link_libraries(srsgnb_polar log_likelihood_ratio srslog)

install(TARGETS srsgnb_polar EXPORT srsgnb_matlab)
