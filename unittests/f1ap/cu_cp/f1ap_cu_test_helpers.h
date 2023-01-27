/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../common/f1ap_cu_test_messages.h"
#include "../common/test_helpers.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/f1ap/common/f1ap_common.h"
#include "srsgnb/f1ap/cu_cp/f1ap_cu.h"
#include "srsgnb/f1ap/cu_cp/f1ap_cu_factory.h"
#include "srsgnb/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

namespace srsgnb {
namespace srs_cu_cp {

/// Reusable notifier class that a) stores the received du_index for test inspection and b)
/// calls the registered DU handler (if any). The handler can be added upon construction
/// or later via the attach_handler() method.
class dummy_f1c_du_management_notifier : public f1c_du_management_notifier
{
public:
  void attach_handler(cu_cp_du_handler* handler_) { handler = handler_; };
  void on_du_remove_request_received(du_index_t idx) override
  {
    logger.info("Received a du remove request for du {}", idx);
    last_du_idx = idx; // store idx

    if (handler != nullptr) {
      logger.info("Forwarding remove request");
      handler->handle_du_remove_request(idx);
    }
  }

  du_index_t last_du_idx;

private:
  srslog::basic_logger& logger  = srslog::fetch_basic_logger("TEST");
  cu_cp_du_handler*     handler = nullptr;
};

/// \brief Creates a dummy UE CONTEXT SETUP REQUEST.
f1ap_ue_context_setup_request create_ue_context_setup_request(ue_index_t                             ue_index,
                                                              const std::initializer_list<drb_id_t>& drbs_to_add);

/// Fixture class for F1AP
class f1ap_cu_test : public ::testing::Test
{
protected:
  struct test_ue {
    ue_index_t                    ue_index;
    optional<gnb_cu_ue_f1ap_id_t> cu_ue_id;
    optional<gnb_du_ue_f1ap_id_t> du_ue_id;
  };

  f1ap_cu_test();
  ~f1ap_cu_test() override;

  /// \brief Helper method to successfully create UE instance in F1AP.
  test_ue& create_ue(gnb_du_ue_f1ap_id_t du_ue_id);

  /// \brief Helper method to run F1AP CU UE Context Setup procedure to completion for a given UE.
  void run_ue_context_setup(ue_index_t ue_index);

  srslog::basic_logger& f1ap_logger = srslog::fetch_basic_logger("F1AP");
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");

  slotted_id_table<ue_index_t, test_ue, MAX_NOF_UES> test_ues;

  dummy_f1c_pdu_notifier           f1c_pdu_notifier;
  dummy_f1c_du_processor_notifier  du_processor_notifier;
  dummy_f1c_du_management_notifier f1c_du_mgmt_notifier;
  manual_task_worker               ctrl_worker{128};
  std::unique_ptr<f1ap_cu>         f1ap;
};

} // namespace srs_cu_cp
} // namespace srsgnb
