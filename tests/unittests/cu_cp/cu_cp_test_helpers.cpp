/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_cp_test_helpers.h"
#include "srsran/support/async/async_test_utils.h"

using namespace srsran;
using namespace srs_cu_cp;

cu_cp_test::cu_cp_test()
{
  test_logger.set_level(srslog::basic_levels::debug);
  cu_cp_logger.set_level(srslog::basic_levels::debug);
  srslog::init();

  // create CU-CP config
  cu_cp_configuration cfg;
  cfg.cu_cp_executor = &ctrl_worker;
  cfg.f1ap_notifier  = &f1ap_pdu_notifier;
  cfg.e1ap_notifier  = &e1ap_pdu_notifier;
  cfg.ngap_notifier  = &ngap_amf_notifier;

  cfg.ngap_config.ran_node_name = "srsgnb01";
  cfg.ngap_config.plmn          = "00101";
  cfg.ngap_config.tac           = 7;

  // create and start DUT
  cu_cp_obj = std::make_unique<cu_cp>(std::move(cfg));
  cu_cp_obj->start();
}

cu_cp_test::~cu_cp_test()
{
  // flush logger after each test
  srslog::flush();
  cu_cp_obj->stop();
}

bool cu_cp_test::check_minimal_paging_result()
{
  if (f1ap_pdu_notifier.last_f1ap_msg.pdu.type() != asn1::f1ap::f1ap_pdu_c::types::init_msg ||
      f1ap_pdu_notifier.last_f1ap_msg.pdu.init_msg().value.type() !=
          asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types::paging) {
    return false;
  }

  auto& paging_msg = f1ap_pdu_notifier.last_f1ap_msg.pdu.init_msg().value.paging();

  // check ue id idx value
  if (paging_msg->ue_id_idx_value.value.idx_len10().to_number() != (4211117727 % 1024)) {
    test_logger.error("UE ID idx value mismatch {} != {}",
                      paging_msg->ue_id_idx_value.value.idx_len10().to_number(),
                      (4211117727 % 1024));
    return false;
  }

  // check paging id
  if (paging_msg->paging_id.value.cn_ue_paging_id().five_g_s_tmsi().to_number() != 4211117727) {
    test_logger.error("Paging ID mismatch {} != {}",
                      paging_msg->paging_id.value.cn_ue_paging_id().five_g_s_tmsi().to_number(),
                      4211117727);
    return false;
  }

  // check paging cell list
  if (paging_msg->paging_cell_list.value.size() != 1) {
    test_logger.error("Paging cell list size mismatch {} != {}", paging_msg->paging_cell_list.value.size(), 1);
    return false;
  }
  auto& paging_cell_item = paging_msg->paging_cell_list.value[0].value().paging_cell_item();
  if (paging_cell_item.nr_cgi.nr_cell_id.to_number() != 12345678) {
    test_logger.error("NR CGI NCI mismatch {} != {}", paging_cell_item.nr_cgi.nr_cell_id.to_number(), 12345678);
    return false;
  }
  if (paging_cell_item.nr_cgi.plmn_id.to_string() != "00f110") {
    test_logger.error("NR CGI PLMN mismatch {} != 00f110", paging_cell_item.nr_cgi.plmn_id.to_string());
    return false;
  }

  return true;
}

bool cu_cp_test::check_paging_result()
{
  if (!check_minimal_paging_result()) {
    return false;
  }

  auto& paging_msg = f1ap_pdu_notifier.last_f1ap_msg.pdu.init_msg().value.paging();

  // check paging drx
  if (!paging_msg->paging_drx_present) {
    return false;
  }
  if (paging_msg->paging_drx.value.to_number() != 64) {
    test_logger.error("Paging DRX mismatch {} != {}", paging_msg->paging_drx.value.to_number(), 64);
    return false;
  }

  // check paging prio
  if (!paging_msg->paging_prio_present) {
    return false;
  }
  if (paging_msg->paging_prio.value.to_number() != 5) {
    test_logger.error("Paging prio mismatch {} != {}", paging_msg->paging_prio.value.to_number(), 5);
    return false;
  }

  // check paging origin
  if (!paging_msg->paging_origin_present) {
    return false;
  }
  if ((std::string)paging_msg->paging_origin.value.to_string() != "non-3gpp") {
    test_logger.error("Paging origin mismatch {} != non-3gpp", paging_msg->paging_origin.value.to_string());
    return false;
  }

  return true;
}
