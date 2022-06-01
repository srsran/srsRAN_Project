#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(srsgnb_cu_cp cu_cp.cpp)
target_link_libraries(srsgnb_cu_cp srslog srsgnb_support srsgnb_f1)
