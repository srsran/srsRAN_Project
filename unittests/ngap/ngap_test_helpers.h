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

#include "lib/cu_cp/ue_manager.h"
#include "ngap_test_messages.h"
#include "test_helpers.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/ngap/ngc.h"
#include "srsgnb/ngap/ngc_configuration.h"
#include "srsgnb/ngap/ngc_factory.h"
#include <gtest/gtest.h>

namespace srsgnb {
namespace srs_cu_cp {

/// Fixture class for NGC
class ngap_test : public ::testing::Test
{
protected:
  struct test_ue {
    cu_cp_ue_id_t         cu_cp_ue_id;
    optional<amf_ue_id_t> amf_ue_id;
    optional<ran_ue_id_t> ran_ue_id;
  };

  ngap_test();
  ~ngap_test() override;

  /// \brief Helper method to successfully create UE instance in NGAP.
  void create_ue(cu_cp_ue_id_t cu_cp_ue_id);

  /// \brief Helper method to successfully run DL NAS transport in NGAP.
  void run_dl_nas_transport(cu_cp_ue_id_t cu_cp_ue_id);

  /// \brief Helper method to successfully run UL NAS transport in NGAP.
  void run_ul_nas_transport(cu_cp_ue_id_t cu_cp_ue_id);

  /// \brief Helper method to successfully run Initial Context Setup in NGAP.
  void run_inital_context_setup(cu_cp_ue_id_t cu_cp_ue_id);

  srslog::basic_logger& ngap_logger = srslog::fetch_basic_logger("NGAP");
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");

  slotted_id_table<cu_cp_ue_id_t, test_ue, MAX_NOF_CU_UES> test_ues;

  ngc_configuration               cfg;
  timer_manager                   timers;
  ue_manager                      ue_mng;
  dummy_ngc_amf_notifier          msg_notifier;
  dummy_ngc_rrc_ue_notifier       rrc_ue_notifier;
  dummy_ngc_du_processor_notifier du_processor_notifier;
  dummy_ngc_ue_task_scheduler     ngap_ue_task_scheduler;
  std::unique_ptr<ngc_interface>  ngap;
};

} // namespace srs_cu_cp
} // namespace srsgnb
