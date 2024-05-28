#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        du_low_factory.cpp
        du_low_impl.cpp
        du_low_wrapper_factory.cpp
        du_low_wrapper_impl.cpp)

add_library(srsran_du_low_wrapper STATIC ${SOURCES})
target_link_libraries(srsran_du_low_wrapper srsran_phy_fapi_adaptor srsran_upper_phy srsran_channel_precoder)
