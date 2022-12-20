
#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

# ASN1 utils
add_library(asn1_utils STATIC asn1_utils.cpp)
target_link_libraries(asn1_utils srslog)
INSTALL(TARGETS asn1_utils DESTINATION ${LIBRARY_DIR})

# RRC NR ASN1
add_library(rrc_nr_asn1 STATIC rrc_nr/rrc_nr.cpp rrc_nr/common.cpp rrc_nr/serving_cell.cpp rrc_nr/ue_cap.cpp)
target_compile_options(rrc_nr_asn1 PRIVATE "-Os")
target_link_libraries(rrc_nr_asn1 asn1_utils)
INSTALL(TARGETS rrc_nr_asn1 DESTINATION ${LIBRARY_DIR})

# F1AP ASN1
add_library(f1ap_asn1 STATIC f1ap/f1ap.cpp)
target_compile_options(f1ap_asn1 PRIVATE "-Os")
target_link_libraries(f1ap_asn1 asn1_utils)
INSTALL(TARGETS f1ap_asn1 DESTINATION ${LIBRARY_DIR})

# E1AP ASN1
add_library(e1ap_asn1 STATIC e1ap/e1ap.cpp)
target_compile_options(e1ap_asn1 PRIVATE "-Os")
target_link_libraries(e1ap_asn1 asn1_utils)
INSTALL(TARGETS e1ap_asn1 DESTINATION ${LIBRARY_DIR})

# E2AP ASN1
add_library(e2ap_asn1 STATIC e2ap.cpp)
target_compile_options(e2ap_asn1 PRIVATE "-Os")
target_link_libraries(e2ap_asn1 asn1_utils)
INSTALL(TARGETS e2ap_asn1 DESTINATION ${LIBRARY_DIR})

# NGAP ASN1
add_library(ngap_asn1 STATIC ngap/ngap.cpp)
target_compile_options(ngap_asn1 PRIVATE "-Os")
target_link_libraries(ngap_asn1 asn1_utils)
INSTALL(TARGETS ngap_asn1 DESTINATION ${LIBRARY_DIR})

# RRC NR ASN1 helpers
add_library(rrc_nr_asn1_helpers STATIC rrc_nr/tdd_cfg_helper.cpp)
target_compile_options(rrc_nr_asn1_helpers PRIVATE "-Os")
target_link_libraries(rrc_nr_asn1_helpers rrc_nr_asn1 asn1_utils)

