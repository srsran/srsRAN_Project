#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(srsran_e1ap_common e1ap_asn1_packer.cpp log_helpers.cpp)
target_link_libraries(srsran_e1ap_common e1ap_asn1)

add_to_exported_libs(srsran_e1ap_common)
