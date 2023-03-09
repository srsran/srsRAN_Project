/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "lib/cu_cp/ue_manager_impl.h"
#include "ngap_test_messages.h"
#include "test_helpers.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ngap/ngap.h"
#include "srsran/ngap/ngap_configuration.h"
#include "srsran/ngap/ngap_factory.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

namespace srsran {
namespace srs_cu_cp {

/// Fixture class for NGAP
class ngap_test : public ::testing::Test
{
protected:
  class test_ue
  {
  public:
    test_ue(ue_index_t ue_index_) : ue_index(ue_index_) {}

    ue_index_t            ue_index = ue_index_t::invalid;
    optional<amf_ue_id_t> amf_ue_id;
    optional<ran_ue_id_t> ran_ue_id;
  };

  ngap_test();
  ~ngap_test() override;

  /// \brief Helper method to successfully create UE instance in NGAP.
  void create_ue(ue_index_t ue_index);

  /// \brief Helper method to successfully run DL NAS transport in NGAP.
  void run_dl_nas_transport(ue_index_t ue_index);

  /// \brief Helper method to successfully run UL NAS transport in NGAP.
  void run_ul_nas_transport(ue_index_t ue_index);

  /// \brief Helper method to successfully run Initial Context Setup in NGAP.
  void run_inital_context_setup(ue_index_t ue_index);

  /// \brief Manually tick timers.
  void tick();

  srslog::basic_logger& ngap_logger = srslog::fetch_basic_logger("NGAP");
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");

  std::unordered_map<ue_index_t, test_ue> test_ues;

  ngap_configuration               cfg;
  timer_manager2                   timers;
  dummy_ngap_ue_manager            ue_mng;
  dummy_ngap_amf_notifier          msg_notifier;
  dummy_ngap_rrc_ue_notifier       rrc_ue_notifier;
  dummy_ngap_du_processor_notifier du_processor_notifier;
  dummy_ngap_cu_cp_paging_notifier cu_cp_paging_notifier;
  dummy_ngap_ue_task_scheduler     ngap_ue_task_scheduler;
  manual_task_worker               ctrl_worker{128};
  std::unique_ptr<ngap_interface>  ngap;
};

} // namespace srs_cu_cp
} // namespace srsran
