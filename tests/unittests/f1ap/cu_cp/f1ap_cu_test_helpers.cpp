/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1ap_cu_test_helpers.h"
#include "srsran/support/async/async_test_utils.h"

using namespace srsran;
using namespace srs_cu_cp;

f1ap_cu_test::f1ap_cu_test()
{
  test_logger.set_level(srslog::basic_levels::debug);
  f1ap_logger.set_level(srslog::basic_levels::debug);
  srslog::init();

  f1ap = create_f1ap(f1ap_pdu_notifier, du_processor_notifier, f1ap_du_mgmt_notifier, ctrl_worker);
}

f1ap_cu_test::~f1ap_cu_test()
{
  // flush logger after each test
  srslog::flush();
}

f1ap_cu_test::test_ue& f1ap_cu_test::create_ue(gnb_du_ue_f1ap_id_t du_ue_id)
{
  f1ap_message msg = generate_init_ul_rrc_message_transfer(du_ue_id, to_rnti(0x4601), {0x1, 0x2, 0x3, 0x4});
  f1ap->handle_message(msg);
  ue_index_t ue_index = *du_processor_notifier.last_created_ue_index;
  test_ues.emplace(ue_index);
  test_ues[ue_index].ue_index = ue_index;
  test_ues[ue_index].du_ue_id = du_ue_id;
  return test_ues[ue_index];
}

void f1ap_cu_test::run_ue_context_setup(ue_index_t ue_index)
{
  test_ue& u = test_ues[ue_index];

  f1ap_ue_context_setup_request req = create_ue_context_setup_request(ue_index, {});

  // Start procedure in CU.
  async_task<f1ap_ue_context_setup_response>         t = f1ap->handle_ue_context_setup_request(req);
  lazy_task_launcher<f1ap_ue_context_setup_response> t_launcher(t);

  if (not u.cu_ue_id.has_value()) {
    u.cu_ue_id = int_to_gnb_cu_ue_f1ap_id(
        this->f1ap_pdu_notifier.last_f1ap_msg.pdu.init_msg().value.ue_context_setup_request()->gnb_cu_ue_f1ap_id.value);
  }

  // Handle response from DU.
  f1ap_message response = generate_ue_context_setup_response(*u.cu_ue_id, *u.du_ue_id);
  f1ap->handle_message(response);

  srsran_assert(t.ready(), "The procedure should have completed by now");
}

f1ap_ue_context_setup_request
srsran::srs_cu_cp::create_ue_context_setup_request(ue_index_t                             ue_index,
                                                   const std::initializer_list<drb_id_t>& drbs_to_add)
{
  f1ap_ue_context_setup_request req;
  req.ue_index = ue_index;

  f1ap_message dummy_msg = generate_ue_context_setup_request(int_to_gnb_cu_ue_f1ap_id(0), int_to_gnb_du_ue_f1ap_id(0));
  req.msg                = dummy_msg.pdu.init_msg().value.ue_context_setup_request();

  return req;
}
