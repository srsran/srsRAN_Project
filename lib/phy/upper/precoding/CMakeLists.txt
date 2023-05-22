#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(srsran_channel_precoder channel_precoder_impl.cpp channel_precoder_generic.cpp precoding_factories.cpp)

install(TARGETS srsran_channel_precoder
        EXPORT srsran_export)
