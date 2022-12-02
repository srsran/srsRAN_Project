#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(cu-up main.cpp)
target_link_libraries(cu-up srsgnb_f1c_cu srsgnb_security srsgnb_pdcp  srsran_sdap srsgnb_support)
