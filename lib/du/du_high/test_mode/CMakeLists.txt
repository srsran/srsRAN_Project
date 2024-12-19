#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(srsran_du_high_adapters
        mac_test_mode_adapter.cpp
        f1ap_test_mode_adapter.cpp
        mac_test_mode_helpers.cpp
        mac_test_mode_ue_repository.cpp)
target_link_libraries(srsran_du_high_adapters srslog srsran_support srsran_du_manager srsran_mac srsran_f1ap_du)
