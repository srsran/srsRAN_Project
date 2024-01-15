/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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

    dummy_ngap_rrc_ue_notifier rrc_ue_notifier;
  };

  ngap_test();
  ~ngap_test() override;

  /// \brief Helper method to successfully create UE instance in NGAP.
  ue_index_t create_ue(rnti_t rnti = rnti_t::MIN_CRNTI);

  /// \brief Helper method to successfully run DL NAS transport in NGAP.
  void run_dl_nas_transport(ue_index_t ue_index);

  /// \brief Helper method to successfully run UL NAS transport in NGAP.
  void run_ul_nas_transport(ue_index_t ue_index);

  /// \brief Helper method to successfully run Initial Context Setup in NGAP.
  void run_inital_context_setup(ue_index_t ue_index);

  /// \brief Helper method to successfully run PDU Session Resource Setup in NGAP
  void run_pdu_session_resource_setup(ue_index_t ue_index, pdu_session_id_t pdu_session_id);

  // Manually add existing PDU sessions to UP manager
  void add_pdu_session_to_up_manager(ue_index_t       ue_index,
                                     pdu_session_id_t pdu_session_id,
                                     drb_id_t         drb_id,
                                     qos_flow_id_t    qos_flow_id);

  /// \brief Manually tick timers.
  void tick();

  srslog::basic_logger& ngap_logger = srslog::fetch_basic_logger("NGAP");
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");

  std::unordered_map<ue_index_t, test_ue> test_ues;

  ngap_configuration                                cfg;
  timer_manager                                     timers;
  ue_manager                                        ue_mng{{}, {}};
  dummy_ngap_amf_notifier                           msg_notifier;
  std::unique_ptr<dummy_ngap_du_processor_notifier> du_processor_notifier;
  dummy_ngap_cu_cp_paging_notifier                  cu_cp_paging_notifier;
  dummy_ngap_ue_task_scheduler                      ngap_ue_task_scheduler;
  manual_task_worker                                ctrl_worker{128};
  std::unique_ptr<ngap_interface>                   ngap;
};

} // namespace srs_cu_cp
} // namespace srsran
