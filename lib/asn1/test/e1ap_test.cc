/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/asn1/e1ap.h"
#include "srsgnb/support/test_utils.h"
#include "srsgnb/pcap/e1ap_pcap.h"

using namespace std;
using namespace asn1;

#define JSON_OUTPUT 1

const char* log_id = "ASN1";

static srsgnb::e1ap_pcap pcap_writer;

void gnb_cu_up_e1_setup_test()
{
  auto& logger = srslog::fetch_basic_logger("ASN1", false);
  logger.set_level(srslog::basic_levels::debug);
  logger.set_hex_dump_max_size(-1);

  asn1::e1ap::e1_ap_pdu_c pdu;
  pdu.set_init_msg();
  pdu.init_msg().value.set(asn1::e1ap::e1_ap_elem_procs_o::init_msg_c::types::gnb_cu_up_e1_setup_request);
  pdu.init_msg().proc_code =ASN1_E1AP_ID_G_NB_CU_UP_E1_SETUP;
  pdu.init_msg().crit = crit_opts::reject;
   
  auto& setup_request = pdu.init_msg().value.gnb_cu_up_e1_setup_request();
  setup_request->cn_support.value = asn1::e1ap::cn_support_opts::options::both;

  asn1::e1ap::supported_plmns_item_s plmn;
  plmn.plmn_id.from_string("214002");
  setup_request->supported_plmns->push_back(plmn);

  setup_request->gnb_cu_up_id.value = 1;
  setup_request->gnb_cu_up_name_present = true;
  setup_request->gnb_cu_up_name.value.from_string("srs-cu-cp");

  uint8_t       buffer[1024];
  asn1::bit_ref bref(buffer, sizeof(buffer));
  TESTASSERT(pdu.pack(bref) == SRSASN_SUCCESS);
  
  pcap_writer.write_pdu(srsgnb::span<uint8_t>(buffer, bref.distance_bytes()));

  logger.info(
      buffer, bref.distance_bytes(), "Packed PDU ({} bytes):", bref.distance_bytes());

#if JSON_OUTPUT
  int               unpacked_len = bref.distance_bytes();
  asn1::json_writer json_writer1;
  pdu.to_json(json_writer1);
  logger.info(
      buffer, unpacked_len, "E1AP unpacked ({} B): \n {}", unpacked_len, json_writer1.to_string().c_str());
#endif
}

int main()
{
  auto& asn1_logger = srslog::fetch_basic_logger(log_id);
  asn1_logger.set_level(srslog::basic_levels::debug);
  asn1_logger.set_hex_dump_max_size(-1);

  pcap_writer.open("e1ap.pcap");

  // Start the log backend.
  srslog::init();

  gnb_cu_up_e1_setup_test();
 
  srslog::flush(); 

  pcap_writer.close();
  
  return 0;
}