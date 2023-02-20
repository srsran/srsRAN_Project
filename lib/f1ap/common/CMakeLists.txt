#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(srsran_f1ap_common asn1_helpers.cpp f1ap_asn1_packer.cpp)
target_link_libraries(srsran_f1ap_common f1ap_asn1)
