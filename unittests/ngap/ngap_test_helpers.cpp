/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ngap_test_helpers.h"
#include "srsgnb/support/async/async_test_utils.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;
using namespace srs_cu_cp;

ngap_test::ngap_test() : ngap_ue_task_scheduler(timers)
{
  test_logger.set_level(srslog::basic_levels::debug);
  ngap_logger.set_level(srslog::basic_levels::debug);
  srslog::init();

  cfg.gnb_id        = 411;
  cfg.ran_node_name = "srsgnb01";
  cfg.plmn          = "00101";
  cfg.tac           = 7;

  ngap = create_ngc(cfg, ngap_ue_task_scheduler, ue_mng, msg_notifier, ctrl_worker);
}

ngap_test::~ngap_test()
{
  // flush logger after each test
  srslog::flush();
}

void ngap_test::create_ue(cu_cp_ue_id_t cu_cp_ue_id)
{
  // Inject UE creation at NGAP
  ngap->create_ngc_ue(get_du_index_from_cu_cp_ue_id(cu_cp_ue_id),
                      get_ue_index_from_cu_cp_ue_id(cu_cp_ue_id),
                      rrc_ue_notifier,
                      rrc_ue_notifier,
                      du_processor_notifier);

  // generate and inject valid initial ue message
  ngap_initial_ue_message msg = generate_initial_ue_message(cu_cp_ue_id);
  ngap->handle_initial_ue_message(msg);

  test_ues.emplace(cu_cp_ue_id);
  test_ues[cu_cp_ue_id].cu_cp_ue_id = cu_cp_ue_id;
  test_ues[cu_cp_ue_id].ran_ue_id =
      uint_to_ran_ue_id(msg_notifier.last_ngc_msg.pdu.init_msg().value.init_ue_msg()->ran_ue_ngap_id.value);
}

void ngap_test::run_dl_nas_transport(cu_cp_ue_id_t cu_cp_ue_id)
{
  auto& ue     = test_ues[cu_cp_ue_id];
  ue.amf_ue_id = uint_to_amf_ue_id(
      test_rgen::uniform_int<uint32_t>(amf_ue_id_to_uint(amf_ue_id_t::min), amf_ue_id_to_uint(amf_ue_id_t::max) - 1));

  ngc_message dl_nas_transport = generate_downlink_nas_transport_message(ue.amf_ue_id.value(), ue.ran_ue_id.value());
  ngap->handle_message(dl_nas_transport);
}

void ngap_test::run_ul_nas_transport(cu_cp_ue_id_t cu_cp_ue_id)
{
  ngap_ul_nas_transport_message ul_nas_transport = generate_ul_nas_transport_message(cu_cp_ue_id);
  ngap->handle_ul_nas_transport_message(ul_nas_transport);
}

void ngap_test::run_inital_context_setup(cu_cp_ue_id_t cu_cp_ue_id)
{
  auto& ue = test_ues[cu_cp_ue_id];

  ngc_message init_context_setup_request =
      generate_valid_initial_context_setup_request_message(ue.amf_ue_id.value(), ue.ran_ue_id.value());
  ngap->handle_message(init_context_setup_request);
}
