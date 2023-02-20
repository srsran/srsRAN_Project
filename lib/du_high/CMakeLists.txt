#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(srsran_du_high du_high.cpp)
target_link_libraries(srsran_du_high srslog srsran_support srsran_du_manager srsran_mac srsran_f1ap_du)
