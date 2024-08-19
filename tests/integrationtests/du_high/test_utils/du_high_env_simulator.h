/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "du_high_worker_manager.h"
#include "tests/test_doubles/f1u/dummy_f1u_du_gateway.h"
#include "tests/test_doubles/mac/dummy_mac_result_notifier.h"
#include "tests/test_doubles/mac/mac_test_messages.h"
#include "srsran/du_high/du_high.h"
#include "srsran/du_high/du_high_configuration.h"
#include "srsran/f1ap/common/f1ap_ue_id.h"
#include "srsran/scheduler/config/cell_config_builder_params.h"

namespace srsran {
namespace srs_du {

class dummy_f1c_test_client : public f1c_connection_client
{
public:
  /// Last messages sent to the CU.
  std::vector<f1ap_message> last_f1ap_msgs;

  dummy_f1c_test_client(task_executor& test_exec_);

  std::unique_ptr<f1ap_message_notifier>
  handle_du_connection_request(std::unique_ptr<f1ap_message_notifier> du_rx_pdu_notifier) override;

private:
  task_executor& test_exec;
};

bool is_ue_context_release_complete_valid(const f1ap_message& msg,
                                          gnb_du_ue_f1ap_id_t du_ue_id,
                                          gnb_cu_ue_f1ap_id_t cu_ue_id);

/// Parameters to set the DU-high environment simulator.
struct du_high_env_sim_params {
  unsigned                                  nof_cells = 1;
  std::optional<cell_config_builder_params> builder_params;
  std::optional<pucch_builder_params>       pucch_cfg;
};

class du_high_env_simulator
{
public:
  du_high_env_simulator(du_high_env_sim_params params = du_high_env_sim_params{});
  virtual ~du_high_env_simulator();

  bool add_ue(rnti_t rnti, du_cell_index_t cell_index = to_du_cell_index(0));

  /// Run the RRC setup procedure for the given RNTI from the moment the CU-CP sends an RRC Setup (via DL RRC Message
  /// Transfer) until the CU receives the RRC Setup Complete (via UL RRC Message Transfer).
  bool run_rrc_setup(rnti_t rnti);

  /// Run the RRC reject procedure for a given RNTI.
  bool run_rrc_reject(rnti_t rnti);

  /// Run the RRC Reestablishment procedure for the given RNTI from the moment the CU-CP sends an RRC Reestablishment
  /// (via DL RRC Message Transfer) until the CU receives the RRC Reestablishment Complete (via UL RRC Message
  /// Transfer).
  enum class reestablishment_stage { reest_complete, reconf_complete };
  [[nodiscard]] bool run_rrc_reestablishment(rnti_t                rnti,
                                             rnti_t                old_rnti,
                                             reestablishment_stage stop_at = reestablishment_stage::reconf_complete);

  bool run_ue_context_setup(rnti_t rnti);

  bool run_ue_context_release(rnti_t rnti, srb_id_t srb_id = srb_id_t::srb1);

  void run_slot();

  bool run_until(unique_function<bool()> condition, std::optional<unsigned> max_slot_count = std::nullopt);

  virtual void handle_slot_results(du_cell_index_t cell_index);

  du_high_worker_manager  workers;
  dummy_f1c_test_client   cu_notifier;
  srs_du::cu_up_simulator cu_up_sim;
  phy_test_dummy          phy;
  null_mac_pcap           mac_pcap;
  null_rlc_pcap           rlc_pcap;
  timer_manager           timers;

  du_high_configuration    du_high_cfg;
  std::unique_ptr<du_high> du_hi;

  slot_point next_slot;

  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");

protected:
  struct ue_sim_context {
    struct srb_context {
      uint32_t next_pdcp_sn = 0;
    };

    rnti_t                                rnti = rnti_t::INVALID_RNTI;
    std::optional<gnb_du_ue_f1ap_id_t>    du_ue_id;
    std::optional<gnb_cu_ue_f1ap_id_t>    cu_ue_id;
    du_cell_index_t                       pcell_index;
    std::array<srb_context, MAX_NOF_SRBS> srbs;
  };

  [[nodiscard]] bool
  await_dl_msg_sched(const ue_sim_context& u, lcid_t lcid, std::optional<unsigned> max_slot_count = std::nullopt);

  [[nodiscard]] bool
  send_dl_rrc_msg_and_await_ul_rrc_msg(const ue_sim_context& u, const f1ap_message& dl_msg, uint32_t rlc_ul_sn);

  std::unordered_map<rnti_t, ue_sim_context> ues;

  unsigned next_cu_ue_id = 0;
};

} // namespace srs_du
} // namespace srsran
