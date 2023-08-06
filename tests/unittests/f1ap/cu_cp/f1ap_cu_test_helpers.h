/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "../common/f1ap_cu_test_messages.h"
#include "../common/test_helpers.h"
#include "tests/test_doubles/f1ap/f1c_test_local_gateway.h"
#include "srsran/cu_cp/cu_cp.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/f1ap/common/f1ap_common.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"
#include "srsran/f1ap/cu_cp/f1ap_cu_factory.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

namespace srsran {
namespace srs_cu_cp {

/// \brief Generate a random gnb_cu_ue_f1ap_id
gnb_cu_ue_f1ap_id_t generate_random_gnb_cu_ue_f1ap_id();

/// \brief Reusable F1-C gateway test class for CU-CP unit tests. This class includes:
/// a) Requests a new DU connection to the CU-CP.
/// b) Logs and stores the last transmitted/received PDU by/from the CU-CP.
class dummy_cu_cp_f1c_gateway
{
public:
  dummy_cu_cp_f1c_gateway() : logger(srslog::fetch_basic_logger("TEST")) {}

  void attach_cu_cp_du_repo(srs_cu_cp::du_repository& cu_cp_du_mng_)
  {
    local_f1c_gw.attach_cu_cp_du_repo(cu_cp_du_mng_);
  }

  void request_new_du_connection()
  {
    class sink_f1ap_message_notifier : public f1ap_message_notifier
    {
    public:
      void on_new_message(const f1ap_message& msg) override {}
    };

    auto notifier = local_f1c_gw.handle_du_connection_request(std::make_unique<sink_f1ap_message_notifier>());
    if (notifier != nullptr) {
      du_tx_notifiers.push_back(std::move(notifier));
    }
  }

  void remove_du_connection(size_t connection_idx) { du_tx_notifiers.erase(du_tx_notifiers.begin() + connection_idx); }

  span<const f1ap_message> last_rx_pdus(size_t connection_idx) const
  {
    return local_f1c_gw.get_last_cu_cp_rx_pdus(connection_idx);
  }
  span<const f1ap_message> last_tx_pdus(size_t connection_idx) const
  {
    return local_f1c_gw.get_last_cu_cp_tx_pdus(connection_idx);
  }

  void push_cu_cp_rx_pdu(size_t du_connectin_idx, const f1ap_message& msg)
  {
    du_tx_notifiers[du_connectin_idx]->on_new_message(msg);
  }

  size_t nof_connections() const { return du_tx_notifiers.size(); }

private:
  srslog::basic_logger&  logger;
  f1c_test_local_gateway local_f1c_gw;

  std::vector<std::unique_ptr<f1ap_message_notifier>> du_tx_notifiers;
};

class dummy_f1ap_rrc_message_notifier : public srs_cu_cp::f1ap_rrc_message_notifier
{
public:
  dummy_f1ap_rrc_message_notifier() = default;
  void on_new_rrc_message(asn1::unbounded_octstring<true> rrc_container) override
  {
    logger.info("Received RRC message");
    last_rrc_container = rrc_container;
  };

  asn1::unbounded_octstring<true> last_rrc_container;

private:
  srslog::basic_logger& logger = srslog::fetch_basic_logger("TEST");
};

class dummy_f1ap_du_processor_notifier : public srs_cu_cp::f1ap_du_processor_notifier
{
public:
  dummy_f1ap_du_processor_notifier() : logger(srslog::fetch_basic_logger("TEST")) {}

  srs_cu_cp::du_index_t get_du_index() override { return srs_cu_cp::du_index_t::min; }

  void on_f1_setup_request_received(const srs_cu_cp::f1ap_f1_setup_request& msg) override
  {
    logger.info("Received F1SetupRequest");
    last_f1_setup_request_msg.gnb_du_id          = msg.gnb_du_id;
    last_f1_setup_request_msg.gnb_du_name        = msg.gnb_du_name;
    last_f1_setup_request_msg.gnb_du_rrc_version = msg.gnb_du_rrc_version;
    for (const auto& served_cell : msg.gnb_du_served_cells_list) {
      srs_cu_cp::cu_cp_du_served_cells_item served_cell_item;
      served_cell_item.served_cell_info.nr_cgi          = served_cell.served_cell_info.nr_cgi;
      served_cell_item.served_cell_info.nr_pci          = served_cell.served_cell_info.nr_pci;
      served_cell_item.served_cell_info.five_gs_tac     = served_cell.served_cell_info.five_gs_tac;
      served_cell_item.served_cell_info.cfg_eps_tac     = served_cell.served_cell_info.cfg_eps_tac;
      served_cell_item.served_cell_info.served_plmns    = served_cell.served_cell_info.served_plmns;
      served_cell_item.served_cell_info.nr_mode_info    = served_cell.served_cell_info.nr_mode_info;
      served_cell_item.served_cell_info.meas_timing_cfg = served_cell.served_cell_info.meas_timing_cfg.copy();

      if (served_cell.gnb_du_sys_info.has_value()) {
        srs_cu_cp::cu_cp_gnb_du_sys_info gnb_du_sys_info;
        gnb_du_sys_info.mib_msg  = served_cell.gnb_du_sys_info.value().mib_msg.copy();
        gnb_du_sys_info.sib1_msg = served_cell.gnb_du_sys_info.value().sib1_msg.copy();

        served_cell_item.gnb_du_sys_info = gnb_du_sys_info;
      }

      last_f1_setup_request_msg.gnb_du_served_cells_list.push_back(served_cell_item);
    }
  }

  ue_index_t on_new_ue_index_required() override
  {
    logger.info("Requested to allocate a new ue index.");
    return allocate_ue_index();
  }

  srs_cu_cp::ue_creation_complete_message on_create_ue(const srs_cu_cp::cu_cp_ue_creation_message& msg) override
  {
    logger.info("Received UeCreationRequest");
    last_ue_creation_msg.ue_index               = msg.ue_index;
    last_ue_creation_msg.cgi                    = msg.cgi;
    last_ue_creation_msg.tac                    = msg.tac;
    last_ue_creation_msg.du_to_cu_rrc_container = msg.du_to_cu_rrc_container.copy();
    last_ue_creation_msg.c_rnti                 = msg.c_rnti;

    srs_cu_cp::ue_creation_complete_message ret = {};
    ret.ue_index                                = msg.ue_index;
    for (uint32_t i = 0; i < MAX_NOF_SRBS; i++) {
      ret.srbs[i] = rx_notifier.get();
    }
    return ret;
  }

  void on_delete_ue(ue_index_t ue_index) override
  {
    // Not implemented.
  }

  ue_index_t allocate_ue_index()
  {
    ue_index_t ue_index = srs_cu_cp::ue_index_t::invalid;
    if (ue_id < srs_cu_cp::MAX_NOF_UES_PER_DU) {
      ue_index              = srs_cu_cp::uint_to_ue_index(ue_id);
      last_created_ue_index = ue_index;
      ue_id++;
    }

    return ue_index;
  }

  void on_du_initiated_ue_context_release_request(const srs_cu_cp::f1ap_ue_context_release_request& req) override
  {
    logger.info("Received UEContextReleaseRequest");
    // TODO
  }

  void set_ue_id(uint16_t ue_id_) { ue_id = ue_id_; }

  srs_cu_cp::f1ap_f1_setup_request                 last_f1_setup_request_msg;
  srs_cu_cp::cu_cp_ue_creation_message             last_ue_creation_msg;
  optional<srs_cu_cp::ue_index_t>                  last_created_ue_index;
  std::unique_ptr<dummy_f1ap_rrc_message_notifier> rx_notifier = std::make_unique<dummy_f1ap_rrc_message_notifier>();

private:
  srslog::basic_logger& logger;
  uint16_t              ue_id = ue_index_to_uint(srs_cu_cp::ue_index_t::min);
};

/// Reusable notifier class that a) stores the received du_index for test inspection and b)
/// calls the registered DU handler (if any). The handler can be added upon construction
/// or later via the attach_handler() method.
class dummy_f1ap_du_management_notifier : public f1ap_du_management_notifier
{
public:
  void attach_handler(du_repository* handler_) { handler = handler_; };

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
  du_repository*        handler = nullptr;
};

/// \brief Creates a dummy UE CONTEXT SETUP REQUEST.
f1ap_ue_context_setup_request create_ue_context_setup_request(const std::initializer_list<drb_id_t>& drbs_to_add);

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
  test_ue& run_ue_context_setup();

  srslog::basic_logger& f1ap_logger = srslog::fetch_basic_logger("CU-CP-F1");
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");

  slotted_id_table<ue_index_t, test_ue, MAX_NOF_UES_PER_DU> test_ues;

  dummy_f1ap_pdu_notifier           f1ap_pdu_notifier;
  dummy_f1ap_du_processor_notifier  du_processor_notifier;
  dummy_f1ap_du_management_notifier f1ap_du_mgmt_notifier;
  manual_task_worker                ctrl_worker{128};
  std::unique_ptr<f1ap_cu>          f1ap;
};

} // namespace srs_cu_cp
} // namespace srsran
