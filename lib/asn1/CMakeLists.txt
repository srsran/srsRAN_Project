
# ASN1 utils
add_library(asn1_utils STATIC asn1_utils.cc)
target_link_libraries(asn1_utils srslog)
INSTALL(TARGETS asn1_utils DESTINATION ${LIBRARY_DIR})

# RRC NR ASN1
add_library(rrc_nr_asn1 STATIC rrc_nr.cc)
target_compile_options(rrc_nr_asn1 PRIVATE "-Os")
target_link_libraries(rrc_nr_asn1 asn1_utils)
INSTALL(TARGETS rrc_nr_asn1 DESTINATION ${LIBRARY_DIR})

# F1AP ASN1
add_library(f1ap_asn1 STATIC f1ap.cc)
target_compile_options(f1ap_asn1 PRIVATE "-Os")
target_link_libraries(f1ap_asn1 asn1_utils)
INSTALL(TARGETS f1ap_asn1 DESTINATION ${LIBRARY_DIR})

# E1AP ASN1
add_library(e1ap_asn1 STATIC e1ap.cc)
target_compile_options(e1ap_asn1 PRIVATE "-Os")
target_link_libraries(e1ap_asn1 asn1_utils)
INSTALL(TARGETS e1ap_asn1 DESTINATION ${LIBRARY_DIR})


