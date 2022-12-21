/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1_cu_test_helpers.h"

using namespace srsgnb;
using namespace srs_cu_cp;

f1ap_cu_test::f1ap_cu_test()
{
  test_logger.set_level(srslog::basic_levels::debug);
  f1ap_logger.set_level(srslog::basic_levels::debug);
  srslog::init();

  f1ap = create_f1ap(f1c_pdu_notifier, du_processor_notifier, f1c_du_mgmt_notifier);
}

f1ap_cu_test::~f1ap_cu_test()
{
  // flush logger after each test
  srslog::flush();
}

f1ap_ue_context_modification_request
srsgnb::srs_cu_cp::create_ue_context_modification_request(gnb_cu_ue_f1ap_id_t                    cu_ue_id,
                                                          gnb_du_ue_f1ap_id_t                    du_ue_id,
                                                          const std::initializer_list<drb_id_t>& drbs_to_add)
{
  f1c_message dummy_msg = generate_ue_context_modification_request(cu_ue_id, du_ue_id, drbs_to_add);

  f1ap_ue_context_modification_request req;
  req.msg = dummy_msg.pdu.init_msg().value.ue_context_mod_request();

  return req;
}
