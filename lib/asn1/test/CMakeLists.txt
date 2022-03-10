add_executable(asn1_utils_test asn1_utils_test.cc)
target_link_libraries(asn1_utils_test asn1_utils)
add_test(asn1_utils_test asn1_utils_test)

#add_executable(rrc_nr_asn1_test rrc_nr_test.cc)
#target_link_libraries(rrc_nr_asn1_test asn1_utils rrc_nr_asn1)
#add_test(rrc_nr_asn1_test rrc_nr_asn1_test)

add_executable(asn1_e1ap_test asn1_e1ap_test.cc)
target_link_libraries(asn1_e1ap_test e1ap_asn1 srslog srsgnb_pcap)
add_test(asn1_e1ap_test asn1_e1ap_test)

add_executable(asn1_f1ap_test asn1_f1ap_test.cc)
target_link_libraries(asn1_f1ap_test f1ap_asn1 srslog srsgnb_pcap)
add_test(asn1_f1ap_test asn1_f1ap_test)
