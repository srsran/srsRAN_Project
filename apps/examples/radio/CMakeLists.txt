#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

if (UHD_FOUND)
    add_executable(radio_util_sample radio_util_sample.cpp)
    target_link_libraries(radio_util_sample srsran_radio srslog srsran_support)
endif (UHD_FOUND)