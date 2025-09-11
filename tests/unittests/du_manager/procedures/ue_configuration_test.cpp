/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

/// \file
/// \brief Unit tests for the DU UE configuration procedure.

#include "du_manager_procedure_test_helpers.h"
#include "lib/du/du_high/du_manager/procedures/ue_configuration_procedure.h"
#include "tests/test_doubles/pdcp/pdcp_pdu_generator.h"
#include "tests/test_doubles/rrc/rrc_packed_test_messages.h"
#include "srsran/asn1/rrc_nr/cell_group_config.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/mac/config/mac_config_helpers.h"
#include "srsran/rlc/rlc_srb_config_factory.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_du;

class ue_config_tester : public du_manager_proc_tester, public ::testing::Test
{
protected:
  ue_config_tester() :
    du_manager_proc_tester(std::vector<du_cell_config>{config_helpers::make_default_du_cell_config()})
  {
    test_ue = &create_ue(to_du_ue_index(test_rgen::uniform_int<unsigned>(0, MAX_DU_UE_INDEX)));
  }

  void start_procedure(const f1ap_ue_context_update_request& req, const std::vector<drb_id_t>& failed_drbs = {})
  {
    auto& next_cfg = this->cell_res_alloc.next_context_update_result;
    for (srb_id_t srb_id : req.srbs_to_setup) {
      next_cfg.srbs.emplace(srb_id);
      auto& new_srb   = next_cfg.srbs[srb_id];
      new_srb.srb_id  = srb_id;
      new_srb.rlc_cfg = make_default_srb_rlc_config();
      new_srb.mac_cfg = make_default_srb_mac_lc_config(srb_id_to_lcid(srb_id));
    }
    for (const f1ap_drb_to_setup& drb : req.drbs_to_setup) {
      if (std::find(failed_drbs.begin(), failed_drbs.end(), drb.drb_id) != failed_drbs.end()) {
        // This DRB config will fail.
        this->cell_res_alloc.next_config_resp.failed_drbs.push_back(drb.drb_id);
        continue;
      }
      auto& new_drb            = next_cfg.drbs.emplace(drb.drb_id);
      new_drb.drb_id           = drb.drb_id;
      new_drb.lcid             = uint_to_lcid(3 + (unsigned)drb.drb_id);
      new_drb.rlc_cfg          = make_default_srb_rlc_config();
      new_drb.mac_cfg          = make_default_drb_mac_lc_config();
      new_drb.pdcp_sn_len      = drb.pdcp_sn_len;
      new_drb.s_nssai          = drb.qos_info.s_nssai;
      new_drb.qos              = drb.qos_info.drb_qos;
      new_drb.f1u.t_notify     = 100;
      new_drb.f1u.warn_on_drop = false;
    }

    proc = launch_async<ue_configuration_procedure>(req, ue_mng, params);
    proc_launcher.emplace(proc);
  }

  void mac_finishes_ue_config(du_ue_index_t ue_index, bool result)
  {
    this->mac.wait_ue_reconf.result.result   = result;
    this->mac.wait_ue_reconf.result.ue_index = this->ue_mng.ues.begin()->ue_index;
    this->mac.wait_ue_reconf.ready_ev.set();
  }

  void check_du_to_cu_rrc_container(const f1ap_ue_context_update_request& req,
                                    const byte_buffer&                    container,
                                    const du_ue_resource_config*          reest_context = nullptr,
                                    bool                                  verbose       = false)
  {
    ASSERT_FALSE(container.empty());
    asn1::rrc_nr::cell_group_cfg_s cell_group;
    {
      asn1::cbit_ref bref(container);
      ASSERT_TRUE(cell_group.unpack(bref) == asn1::SRSASN_SUCCESS);
      if (verbose) {
        asn1::json_writer js;
        cell_group.to_json(js);
        fmt::print("UE Configuration produced CellGroup: {}\n", js.to_string());
      }
    }

    if (reest_context == nullptr) {
      ASSERT_EQ(cell_group.rlc_bearer_to_add_mod_list.size(),
                req.srbs_to_setup.size() + req.drbs_to_setup.size() + req.drbs_to_mod.size());

      for (srb_id_t srb_id : req.srbs_to_setup) {
        auto srb_it =
            std::find_if(cell_group.rlc_bearer_to_add_mod_list.begin(),
                         cell_group.rlc_bearer_to_add_mod_list.end(),
                         [srb_id](const auto& b) {
                           return b.served_radio_bearer.type().value ==
                                      asn1::rrc_nr::rlc_bearer_cfg_s::served_radio_bearer_c_::types::srb_id and
                                  b.served_radio_bearer.srb_id() == srb_id_to_uint(srb_id);
                         });
        ASSERT_NE(srb_it, cell_group.rlc_bearer_to_add_mod_list.end());
        ASSERT_EQ(srb_it->lc_ch_id, srb_id_to_lcid(srb_id));
        ASSERT_TRUE(srb_it->mac_lc_ch_cfg_present);
        ASSERT_TRUE(srb_it->rlc_cfg_present);
        ASSERT_FALSE(srb_it->reestablish_rlc_present);
      }
      for (const f1ap_drb_to_modify& drb : req.drbs_to_mod) {
        auto drb_it =
            std::find_if(cell_group.rlc_bearer_to_add_mod_list.begin(),
                         cell_group.rlc_bearer_to_add_mod_list.end(),
                         [&drb](const auto& b) {
                           return b.served_radio_bearer.type().value ==
                                      asn1::rrc_nr::rlc_bearer_cfg_s::served_radio_bearer_c_::types::drb_id and
                                  b.served_radio_bearer.drb_id() == drb_id_to_uint(drb.drb_id);
                         });
        ASSERT_NE(drb_it, cell_group.rlc_bearer_to_add_mod_list.end());
        ASSERT_FALSE(is_srb(uint_to_lcid(drb_it->lc_ch_id)));
        ASSERT_TRUE(drb_it->mac_lc_ch_cfg_present);
        ASSERT_TRUE(drb_it->rlc_cfg_present);
        ASSERT_FALSE(drb_it->reestablish_rlc_present);
      }
    } else {
      ASSERT_TRUE(req.srbs_to_setup.empty());
      ASSERT_TRUE(req.drbs_to_setup.empty());
      unsigned nof_srb_reest = reest_context->srbs.size() - 1; // SRB1 has already been reestablished.
      ASSERT_EQ(cell_group.rlc_bearer_to_add_mod_list.size(), nof_srb_reest + req.drbs_to_mod.size());

      // Check SRBs/DRBs to reestablish in the UE
      for (const auto& bearer : cell_group.rlc_bearer_to_add_mod_list) {
        ASSERT_FALSE(bearer.mac_lc_ch_cfg_present);
        ASSERT_FALSE(bearer.rlc_cfg_present);
        ASSERT_TRUE(bearer.reestablish_rlc_present);
        if (bearer.served_radio_bearer.type().value ==
            asn1::rrc_nr::rlc_bearer_cfg_s::served_radio_bearer_c_::types::srb_id) {
          // SRB case
          srb_id_t srb_id = to_srb_id(uint_to_lcid(bearer.served_radio_bearer.srb_id()));
          ASSERT_NE(srb_id, srb_id_t::srb1);
          ASSERT_TRUE(reest_context->srbs.contains(srb_id));
        } else {
          // DRB case
          drb_id_t drb_id = uint_to_drb_id(bearer.served_radio_bearer.drb_id());
          ASSERT_TRUE(reest_context->drbs.contains(drb_id));
          auto it = std::find_if(
              req.drbs_to_mod.begin(), req.drbs_to_mod.end(), [drb_id](const auto& d) { return d.drb_id == drb_id; });
          ASSERT_NE(it, req.drbs_to_mod.end());
        }
      }
    }

    for (const f1ap_drb_to_setup& drb : req.drbs_to_setup) {
      auto drb_it = std::find_if(cell_group.rlc_bearer_to_add_mod_list.begin(),
                                 cell_group.rlc_bearer_to_add_mod_list.end(),
                                 [&drb](const auto& b) {
                                   return b.served_radio_bearer.type().value ==
                                              asn1::rrc_nr::rlc_bearer_cfg_s::served_radio_bearer_c_::types::drb_id and
                                          b.served_radio_bearer.drb_id() == drb_id_to_uint(drb.drb_id);
                                 });
      ASSERT_NE(drb_it, cell_group.rlc_bearer_to_add_mod_list.end());
      ASSERT_FALSE(is_srb(uint_to_lcid(drb_it->lc_ch_id)));
      ASSERT_TRUE(drb_it->mac_lc_ch_cfg_present);
      ASSERT_TRUE(drb_it->rlc_cfg_present);
      ASSERT_FALSE(drb_it->reestablish_rlc_present);
    }

    ASSERT_EQ(cell_group.rlc_bearer_to_release_list.size(), req.drbs_to_rem.size());
    for (unsigned i = 0; i != req.drbs_to_rem.size(); ++i) {
      uint8_t lcid = LCID_MIN_DRB - 1 + (unsigned)req.drbs_to_rem[i];
      ASSERT_EQ(lcid, cell_group.rlc_bearer_to_release_list[i]);
    }

    if (reest_context != nullptr) {
      ASSERT_TRUE(cell_group.sp_cell_cfg_present);
      ASSERT_TRUE(cell_group.sp_cell_cfg.sp_cell_cfg_ded_present);
      ASSERT_TRUE(cell_group.sp_cell_cfg.sp_cell_cfg_ded.ul_cfg_present);
      ASSERT_TRUE(cell_group.sp_cell_cfg.sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg_present);
      ASSERT_EQ(cell_group.sp_cell_cfg.sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.type().value,
                asn1::setup_release_opts::setup);
    } else {
      ASSERT_FALSE(cell_group.sp_cell_cfg.sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg_present);
    }
  }

  du_ue*                                                             test_ue = nullptr;
  async_task<f1ap_ue_context_update_response>                        proc;
  std::optional<lazy_task_launcher<f1ap_ue_context_update_response>> proc_launcher;
};

TEST_F(ue_config_tester, when_du_manager_receives_ue_config_request_then_mac_and_f1ap_get_request_to_reconfigure_ue)
{
  // Start Procedure.
  start_procedure(create_f1ap_ue_context_update_request(test_ue->ue_index, {srb_id_t::srb2}, {drb_id_t::drb1}));

  // Check MAC received request to update UE configuration with valid params.
  ASSERT_TRUE(this->mac.last_ue_reconf_msg.has_value());
  ASSERT_EQ(this->mac.last_ue_reconf_msg->ue_index, test_ue->ue_index);
  ASSERT_NE(this->ue_mng.find_ue(this->mac.last_ue_reconf_msg->ue_index), nullptr);
  ASSERT_EQ(this->mac.last_ue_reconf_msg->bearers_to_addmod.size(), 2);
  ASSERT_EQ(this->mac.last_ue_reconf_msg->bearers_to_addmod[0].lcid, LCID_SRB2);
  ASSERT_NE(this->mac.last_ue_reconf_msg->bearers_to_addmod[0].dl_bearer, nullptr);
  ASSERT_NE(this->mac.last_ue_reconf_msg->bearers_to_addmod[0].ul_bearer, nullptr);
  ASSERT_FALSE(is_srb(this->mac.last_ue_reconf_msg->bearers_to_addmod[1].lcid));
  ASSERT_NE(this->mac.last_ue_reconf_msg->bearers_to_addmod[1].dl_bearer, nullptr);
  ASSERT_NE(this->mac.last_ue_reconf_msg->bearers_to_addmod[1].ul_bearer, nullptr);

  // Check F1AP received request to update UE configuration with valid params.
  ASSERT_TRUE(this->f1ap.last_ue_config.has_value());
  ASSERT_EQ(this->f1ap.last_ue_config->ue_index, test_ue->ue_index);
  ASSERT_EQ(this->f1ap.last_ue_config->f1c_bearers_to_add.size(), 1);
  ASSERT_EQ(this->f1ap.last_ue_config->f1c_bearers_to_add[0].srb_id, srb_id_t::srb2);
  ASSERT_NE(this->f1ap.last_ue_config->f1c_bearers_to_add[0].rx_sdu_notifier, nullptr);
}

TEST_F(ue_config_tester, when_du_manager_completes_ue_configuration_procedure_then_response_has_rrc_container)
{
  // Start Procedure.
  f1ap_ue_context_update_request req =
      create_f1ap_ue_context_update_request(test_ue->ue_index, {srb_id_t::srb2}, {drb_id_t::drb1});
  start_procedure(req);

  // MAC finishes config.
  ASSERT_FALSE(proc.ready());
  mac_finishes_ue_config(test_ue->ue_index, true);

  // Check DU manager response to UE Config Request from F1AP.
  ASSERT_TRUE(proc.ready());
  f1ap_ue_context_update_response resp = proc.get();
  ASSERT_TRUE(resp.result);
  ASSERT_NO_FATAL_FAILURE(check_du_to_cu_rrc_container(req, resp.cell_group_cfg, nullptr, true));
}

TEST_F(ue_config_tester, when_du_manager_finishes_processing_ue_config_request_then_mac_rlc_f1c_bearers_are_connected)
{
  static const std::array<uint8_t, 2> dummy_rlc_header = {0x80, 0x0};
  byte_buffer                         test_payload     = test_helpers::create_pdcp_pdu(
      pdcp_sn_size::size12bits, /* is_srb = */ true, 0, test_rgen::uniform_int<unsigned>(3, 100), 0);

  // Run UE Configuration Procedure to completion.
  configure_ue(create_f1ap_ue_context_update_request(test_ue->ue_index, {srb_id_t::srb2}, {}));

  // Forward MAC Rx SDU through SRB2 (UL).
  // > Add dummy RLC data header.
  byte_buffer mac_rx_sdu = byte_buffer::create(dummy_rlc_header).value();
  // > Append data buffer.
  ASSERT_TRUE(mac_rx_sdu.append(test_payload.copy()));
  // > Push MAC Rx SDU through MAC logical channel.
  mac.last_ue_reconf_msg->bearers_to_addmod[0].ul_bearer->on_new_sdu({mac_rx_sdu.copy()});
  // > Check arrival of F1-C Tx SDU to F1-C bearer.
  ASSERT_EQ(test_payload, f1ap.f1ap_ues[test_ue->ue_index].f1c_bearers[srb_id_t::srb2].last_tx_sdu);

  // Forward F1-C Rx SDU through SRB2 (DL).
  // > Create data buffer.
  byte_buffer f1c_rx_sdu = test_payload.copy();
  // > Push F1-C Rx SDU through F1-C bearer Rx SDU notifier.
  f1ap.last_ue_config->f1c_bearers_to_add[0].rx_sdu_notifier->on_new_sdu(std::move(f1c_rx_sdu));
  // > Check arrival of MAC Tx SDU to MAC logical channel.
  std::vector<uint8_t> mac_tx_sdu(test_payload.length() + dummy_rlc_header.size());
  size_t               nwritten = mac.last_ue_reconf_msg->bearers_to_addmod[0].dl_bearer->on_new_tx_sdu(mac_tx_sdu);
  byte_buffer          extracted_payload =
      byte_buffer::create(mac_tx_sdu.begin() + dummy_rlc_header.size(), mac_tx_sdu.begin() + nwritten).value();
  ASSERT_EQ(test_payload, extracted_payload)
      << fmt::format("Byte buffers do not match:\n{}\n{}\n", test_payload, extracted_payload);
}

TEST_F(ue_config_tester, when_du_manager_finishes_processing_ue_config_request_then_mac_rlc_f1u_bearers_are_connected)
{
  static const std::array<uint8_t, 2> dummy_rlc_header = {0x80, 0x0};
  byte_buffer                         test_payload     = test_helpers::create_pdcp_pdu(
      pdcp_sn_size::size12bits, /* is_srb = */ false, 0, test_rgen::uniform_int<unsigned>(3, 100), 0);

  // Run UE Configuration Procedure to completion.
  configure_ue(create_f1ap_ue_context_update_request(test_ue->ue_index, {}, {drb_id_t::drb1}));
  srs_du::f1u_gw_bearer_dummy* bearer = f1u_gw.f1u_bearers.begin()->second.begin()->second;

  // Forward MAC Rx SDU through DRB1 (UL).
  // > Add dummy RLC data header.
  byte_buffer mac_sdu = byte_buffer::create(dummy_rlc_header).value();
  // > Append data buffer.
  ASSERT_TRUE(mac_sdu.append(test_payload.copy()));
  // > Push MAC Rx SDU through MAC logical channel.
  mac.last_ue_reconf_msg->bearers_to_addmod[0].ul_bearer->on_new_sdu({mac_sdu.copy()});
  // > Check arrival of F1-U Tx SDU to F1-U bearer.
  auto last_sdu = bearer->last_sdu->t_pdu->deep_copy().value();
  ASSERT_EQ(test_payload, last_sdu);

  // Forward F1-U Rx SDU through DRB1 (DL).
  // > Create data buffer.
  nru_dl_message rx_sdu;
  rx_sdu.t_pdu = test_payload.copy();
  // > Push F1-U Rx SDU through F1-U bearer Rx SDU notifier.
  bearer->du_rx.on_new_pdu(std::move(rx_sdu));

  // > Check arrival of MAC Tx SDU to MAC logical channel.
  std::vector<uint8_t> mac_tx_sdu(test_payload.length() + dummy_rlc_header.size());
  unsigned             nwritten = mac.last_ue_reconf_msg->bearers_to_addmod[0].dl_bearer->on_new_tx_sdu(mac_tx_sdu);
  byte_buffer          extracted_payload =
      byte_buffer::create(mac_tx_sdu.begin() + dummy_rlc_header.size(), mac_tx_sdu.begin() + nwritten).value();
  ASSERT_EQ(test_payload, extracted_payload);
}

TEST_F(ue_config_tester, when_f1u_gw_fails_to_create_bearer_then_drb_is_included_in_failed_list)
{
  this->f1u_gw.next_bearer_is_created = false;

  // Start Procedure.
  start_procedure(create_f1ap_ue_context_update_request(test_ue->ue_index, {srb_id_t::srb2}, {drb_id_t::drb1}));

  // Check MAC received request to update UE configuration without the DRB that could not be created.
  ASSERT_TRUE(this->mac.last_ue_reconf_msg.has_value());
  ASSERT_EQ(this->mac.last_ue_reconf_msg->ue_index, test_ue->ue_index);
  ASSERT_EQ(this->mac.last_ue_reconf_msg->bearers_to_addmod.size(), 1);
  ASSERT_EQ(this->mac.last_ue_reconf_msg->bearers_to_addmod[0].lcid, LCID_SRB2);
  ASSERT_NE(this->mac.last_ue_reconf_msg->bearers_to_addmod[0].dl_bearer, nullptr);
  ASSERT_NE(this->mac.last_ue_reconf_msg->bearers_to_addmod[0].ul_bearer, nullptr);

  // Check F1AP received request to update UE configuration with valid params.
  ASSERT_TRUE(this->f1ap.last_ue_config.has_value());
  ASSERT_EQ(this->f1ap.last_ue_config->ue_index, test_ue->ue_index);
  ASSERT_EQ(this->f1ap.last_ue_config->f1c_bearers_to_add.size(), 1);
  ASSERT_EQ(this->f1ap.last_ue_config->f1c_bearers_to_add[0].srb_id, srb_id_t::srb2);

  // MAC finishes config.
  ASSERT_FALSE(proc.ready());
  mac_finishes_ue_config(test_ue->ue_index, true);

  // Check DU manager response to UE Config Request from F1AP with failed DRB.
  ASSERT_TRUE(proc.ready());
  f1ap_ue_context_update_response resp = proc.get();
  ASSERT_TRUE(resp.result);
  ASSERT_EQ(resp.drbs_setup.size(), 0);
  ASSERT_EQ(resp.failed_drbs_setups.size(), 1);
  ASSERT_EQ(resp.failed_drbs_setups[0].drb_id, drb_id_t::drb1);
}

TEST_F(ue_config_tester, when_config_is_invalid_of_drb_to_create_then_drb_is_included_in_failed_list)
{
  // Start Procedure.
  f1ap_ue_context_update_request req =
      create_f1ap_ue_context_update_request(test_ue->ue_index, {srb_id_t::srb2}, {drb_id_t::drb1});
  start_procedure(req, {drb_id_t::drb1});

  // Check MAC received request to update UE configuration without the DRB that could not be created.
  ASSERT_TRUE(this->mac.last_ue_reconf_msg.has_value());
  ASSERT_EQ(this->mac.last_ue_reconf_msg->ue_index, test_ue->ue_index);
  ASSERT_EQ(this->mac.last_ue_reconf_msg->bearers_to_addmod.size(), 1);
  ASSERT_EQ(this->mac.last_ue_reconf_msg->bearers_to_addmod[0].lcid, LCID_SRB2);

  // Check F1AP received request to update UE configuration with valid params.
  ASSERT_TRUE(this->f1ap.last_ue_config.has_value());
  ASSERT_EQ(this->f1ap.last_ue_config->ue_index, test_ue->ue_index);
  ASSERT_EQ(this->f1ap.last_ue_config->f1c_bearers_to_add.size(), 1);
  ASSERT_EQ(this->f1ap.last_ue_config->f1c_bearers_to_add[0].srb_id, srb_id_t::srb2);

  // MAC finishes config.
  ASSERT_FALSE(proc.ready());
  mac_finishes_ue_config(test_ue->ue_index, true);

  // Check DU manager response to UE Config Request from F1AP with failed DRB.
  ASSERT_TRUE(proc.ready());
  f1ap_ue_context_update_response resp = proc.get();
  ASSERT_TRUE(resp.result);
  ASSERT_EQ(resp.drbs_setup.size(), 0);
  ASSERT_EQ(resp.failed_drbs_setups.size(), 1);
  ASSERT_EQ(resp.failed_drbs_setups[0].drb_id, drb_id_t::drb1);
}

TEST_F(ue_config_tester, when_config_is_empty_then_procedure_avoids_configuring_other_layers_and_returns_success)
{
  // Start Procedure.
  f1ap_ue_context_update_request req = create_f1ap_ue_context_update_request(test_ue->ue_index, {}, {});
  start_procedure(req);

  // MAC and F1AP should not receive commands to apply UE configuration update.
  ASSERT_FALSE(this->mac.last_ue_reconf_msg.has_value());
  ASSERT_FALSE(this->f1ap.last_ue_config.has_value());

  // Procedure completes with failure.
  ASSERT_TRUE(proc.ready());
  f1ap_ue_context_update_response resp = proc.get();
  ASSERT_TRUE(resp.result);
  ASSERT_TRUE(resp.cell_group_cfg.empty());
  ASSERT_TRUE(resp.drbs_setup.empty());
  ASSERT_TRUE(resp.failed_drbs_setups.empty());
}

TEST_F(ue_config_tester, when_drbs_are_released_then_they_are_added_in_rrc_container)
{
  // Run procedure to create DRB1.
  f1ap_ue_context_update_request req =
      create_f1ap_ue_context_update_request(test_ue->ue_index, {srb_id_t::srb2}, {drb_id_t::drb1});
  f1ap_ue_context_update_response res = this->configure_ue(req);
  ASSERT_TRUE(res.result);

  // Run procedure to delete DRB1.
  req             = create_f1ap_ue_context_update_request(test_ue->ue_index, {}, {});
  req.drbs_to_rem = {drb_id_t::drb1};
  res             = this->configure_ue(req);

  ASSERT_TRUE(res.result);
  ASSERT_NO_FATAL_FAILURE(check_du_to_cu_rrc_container(req, res.cell_group_cfg, nullptr, true));
}

TEST_F(ue_config_tester, when_drb_to_be_released_does_not_exist_then_request_is_ignored)
{
  // Run procedure to create SRB2 and DRB1.
  f1ap_ue_context_update_request req =
      create_f1ap_ue_context_update_request(test_ue->ue_index, {srb_id_t::srb2}, {drb_id_t::drb1});
  f1ap_ue_context_update_response res = this->configure_ue(req);
  ASSERT_TRUE(res.result);

  // Run procedure to delete non-existent DRB.
  req             = create_f1ap_ue_context_update_request(test_ue->ue_index, {}, {});
  req.drbs_to_rem = {drb_id_t::drb2};
  res             = this->configure_ue(req);
  ASSERT_TRUE(res.result);
  auto req_no_drb_release = req;
  req_no_drb_release.drbs_to_rem.clear();
  ASSERT_NO_FATAL_FAILURE(check_du_to_cu_rrc_container(req_no_drb_release, res.cell_group_cfg, nullptr, true));
}

TEST_F(ue_config_tester,
       when_reestablishment_is_signalled_then_bearers_are_marked_as_reestablishRLC_and_cell_config_are_sent)
{
  // Mark UE as reestablishing.
  test_ue->reestablished_cfg_pending = std::make_unique<du_ue_resource_config>();
  test_ue->reestablished_cfg_pending->srbs.emplace(srb_id_t::srb1);
  du_ue_srb_config& old_srb1 = test_ue->reestablished_cfg_pending->srbs[srb_id_t::srb1];
  old_srb1.srb_id            = srb_id_t::srb1;
  old_srb1.rlc_cfg           = make_default_srb_rlc_config();
  old_srb1.mac_cfg           = make_default_drb_mac_lc_config();
  test_ue->reestablished_cfg_pending->srbs.emplace(srb_id_t::srb2);
  du_ue_srb_config& old_srb2                       = test_ue->reestablished_cfg_pending->srbs[srb_id_t::srb2];
  old_srb2.srb_id                                  = srb_id_t::srb2;
  old_srb2.rlc_cfg                                 = make_default_srb_rlc_config();
  old_srb2.mac_cfg                                 = make_default_drb_mac_lc_config();
  du_ue_drb_config& old_bearer                     = test_ue->reestablished_cfg_pending->drbs.emplace(drb_id_t::drb1);
  old_bearer.drb_id                                = drb_id_t::drb1;
  old_bearer.lcid                                  = LCID_MIN_DRB;
  old_bearer.rlc_cfg                               = make_default_srb_rlc_config();
  old_bearer.mac_cfg                               = make_default_drb_mac_lc_config();
  old_bearer.qos.qos_desc.get_nondyn_5qi().five_qi = uint_to_five_qi(9);
  du_ue_resource_config reest_cfg_copy             = *test_ue->reestablished_cfg_pending;

  // Run procedure to modify DRB1.
  f1ap_ue_context_update_request req =
      create_f1ap_ue_context_update_request(test_ue->ue_index, {}, {}, {drb_id_t::drb1});
  f1ap_ue_context_update_response res = this->configure_ue(req);
  ASSERT_TRUE(res.result);
  ASSERT_NO_FATAL_FAILURE(check_du_to_cu_rrc_container(req, res.cell_group_cfg, &reest_cfg_copy, true));
}

TEST_F(ue_config_tester,
       when_existing_srb_is_included_in_ue_context_setup_then_it_does_not_get_configured_in_mac_and_f1ap)
{
  f1ap_ue_context_update_request req =
      create_f1ap_ue_context_update_request(test_ue->ue_index, {srb_id_t::srb1, srb_id_t::srb2}, {drb_id_t::drb1});
  f1ap_ue_context_update_response res = this->configure_ue(req);

  // SRB1 logical channel was not updated in MAC, as it already existed.
  ASSERT_TRUE(this->mac.last_ue_reconf_msg.has_value());
  ASSERT_EQ(this->mac.last_ue_reconf_msg->bearers_to_addmod.size(), 2);
  ASSERT_EQ(this->mac.last_ue_reconf_msg->bearers_to_addmod[0].lcid, LCID_SRB2);
  ASSERT_EQ(this->mac.last_ue_reconf_msg->bearers_to_addmod[1].lcid, LCID_MIN_DRB);

  // SRB1 was not re-created.
  ASSERT_TRUE(this->f1ap.last_ue_config.has_value());
  ASSERT_EQ(this->f1ap.last_ue_config->f1c_bearers_to_add.size(), 1);
  ASSERT_EQ(this->f1ap.last_ue_config->f1c_bearers_to_add[0].srb_id, srb_id_t::srb2);

  ASSERT_TRUE(res.result);
  req.srbs_to_setup.erase(req.srbs_to_setup.begin()); // Remove SRB1 for the checks.
  ASSERT_NO_FATAL_FAILURE(check_du_to_cu_rrc_container(req, res.cell_group_cfg, nullptr, true));
}
