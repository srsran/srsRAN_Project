#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

# ASN1 utils
add_library(asn1_utils STATIC asn1_utils.cpp)
target_compile_options(asn1_utils PRIVATE -fno-exceptions)
target_link_libraries(asn1_utils srslog srsran_support)

# RRC NR ASN1
add_subdirectory(rrc_nr)

# F1AP ASN1
add_library(f1ap_asn1 STATIC f1ap/common.cpp f1ap/f1ap_ies.cpp f1ap/f1ap_pdu_items.cpp f1ap/f1ap_pdu_contents_ue.cpp
        f1ap/f1ap_pdu_contents.cpp f1ap/f1ap.cpp)
target_compile_options(f1ap_asn1 PRIVATE -Os -fno-exceptions)
target_link_libraries(f1ap_asn1 asn1_utils)

# E1AP ASN1
add_library(e1ap_asn1 STATIC e1ap/e1ap.cpp e1ap/e1ap_ies.cpp e1ap/e1ap_pdu_contents.cpp)
target_compile_options(e1ap_asn1 PRIVATE -Os -fno-exceptions)
target_link_libraries(e1ap_asn1 asn1_utils)

# E2AP ASN1
add_library(e2ap_asn1 STATIC e2ap/e2ap.cpp e2sm/e2sm_common_ies.cpp e2sm/e2sm_kpm_ies.cpp e2sm/e2sm_rc_ies.cpp)
target_compile_options(e2ap_asn1 PRIVATE -Os -fno-exceptions)
target_link_libraries(e2ap_asn1 asn1_utils)

# NGAP ASN1
add_library(ngap_asn1 STATIC ngap/common.cpp ngap/ngap_ies.cpp ngap/ngap_pdu_contents.cpp ngap/ngap.cpp)
target_compile_options(ngap_asn1 PRIVATE -Os -fno-exceptions)
target_link_libraries(ngap_asn1 asn1_utils)
