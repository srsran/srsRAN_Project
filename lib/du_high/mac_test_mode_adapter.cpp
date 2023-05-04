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

#include "mac_test_mode_adapter.h"
#include "srsran/scheduler/harq_id.h"

using namespace srsran;

static const unsigned TEST_UE_DL_BUFFER_STATE_UPDATE_SIZE = 100000;

static mac_rx_data_indication create_test_pdu_with_bsr(slot_point sl_rx, rnti_t test_rnti, harq_id_t harq_id)
{
  // - 8-bit R/LCID MAC subheader.
  // - MAC CE with Short BSR.
  //
  // |   |   |   |   |   |   |   |   |
  // | R | R |         LCID          |  Octet 1
  // |  LCG ID   |    Buffer Size    |  Octet 2
  return mac_rx_data_indication{
      sl_rx, to_du_cell_index(0), mac_rx_pdu_list{mac_rx_pdu{test_rnti, 0, harq_id, byte_buffer{0x3d, 0x1f}}}};
}

/// \brief Adapter for the MAC SDU TX builder that auto fills the DL buffer state update.
class test_ue_mac_sdu_tx_builder_adapter : public mac_sdu_tx_builder
{
  static constexpr unsigned TX_SDU_MAX_SIZE = 5000;

public:
  test_ue_mac_sdu_tx_builder_adapter() { tx_sdu.append(std::vector<uint8_t>(TX_SDU_MAX_SIZE, 0)); }

  byte_buffer_slice_chain on_new_tx_sdu(unsigned nof_bytes) override
  {
    // Return empty MAC SDU so that the MAC PDU is padded.
    return byte_buffer_slice_chain{};
  }
  unsigned on_buffer_state_update() override { return TEST_UE_DL_BUFFER_STATE_UPDATE_SIZE; }

private:
  byte_buffer tx_sdu = {};
};

/// \brief Adapter of MAC cell for testing purposes. It automatically forces ACK/CRC=OK for the test UE.
class test_cell_adapter : public mac_cell_control_information_handler
{
public:
  test_cell_adapter(const srs_du::du_test_config::test_ue_config& test_ue_cfg_,
                    mac_cell_control_information_handler&         adapted_,
                    mac_pdu_handler&                              pdu_handler_) :
    test_ue_cfg(test_ue_cfg_), adapted(adapted_), pdu_handler(pdu_handler_)
  {
  }

  void handle_crc(const mac_crc_indication_message& msg) override
  {
    mac_crc_indication_message msg_copy = msg;
    for (auto& crc : msg_copy.crcs) {
      if (crc.rnti != test_ue_cfg.rnti) {
        // It is not the test UE.
        continue;
      }

      // Force CRC=OK for test UE.
      crc.tb_crc_success = true;

      if (test_ue_cfg.pusch_active) {
        // In case of PUSCH test mode is enabled, push a BSR.
        pdu_handler.handle_rx_data_indication(
            create_test_pdu_with_bsr(msg.sl_rx, test_ue_cfg.rnti, to_harq_id(crc.harq_id)));
      }
    }

    return adapted.handle_crc(msg_copy);
  }

  void handle_uci(const mac_uci_indication_message& msg) override
  {
    mac_uci_indication_message msg_copy = msg;
    for (auto& uci : msg_copy.ucis) {
      if (uci.rnti != test_ue_cfg.rnti) {
        // It is not the test UE.
        continue;
      }

      // In case of test UE, set HARQ-Info always equal to ACK.
      if (variant_holds_alternative<mac_uci_pdu::pucch_f0_or_f1_type>(uci.pdu)) {
        auto& f0 = variant_get<mac_uci_pdu::pucch_f0_or_f1_type>(uci.pdu);
        if (f0.harq_info.has_value()) {
          for (uci_pucch_f0_or_f1_harq_values& harq : f0.harq_info->harqs) {
            harq = uci_pucch_f0_or_f1_harq_values::ack;
          }
        }
      } else if (variant_holds_alternative<mac_uci_pdu::pusch_type>(uci.pdu)) {
        auto& pusch = variant_get<mac_uci_pdu::pusch_type>(uci.pdu);
        if (pusch.harq_info.has_value()) {
          pusch.harq_info->harq_status = srsran::uci_pusch_or_pucch_f2_3_4_detection_status::crc_pass;
          pusch.harq_info->payload.fill(0, pusch.harq_info->payload.size(), true);
        }
      } else if (variant_holds_alternative<mac_uci_pdu::pucch_f2_or_f3_or_f4_type>(uci.pdu)) {
        auto& f2 = variant_get<mac_uci_pdu::pucch_f2_or_f3_or_f4_type>(uci.pdu);
        if (f2.harq_info.has_value()) {
          f2.harq_info->harq_status = uci_pusch_or_pucch_f2_3_4_detection_status::crc_pass;
          f2.harq_info->payload.fill(0, f2.harq_info->payload.size(), true);
        }
      }
    }

    adapted.handle_uci(msg_copy);
  }

private:
  const srs_du::du_test_config::test_ue_config& test_ue_cfg;
  mac_cell_control_information_handler&         adapted;
  mac_pdu_handler&                              pdu_handler;
};

mac_test_mode_adapter::mac_test_mode_adapter(std::unique_ptr<mac_interface>                mac_ptr_,
                                             const srs_du::du_test_config::test_ue_config& test_ue_cfg) :
  test_ue(test_ue_cfg), mac_adapted(std::move(mac_ptr_))
{
}

mac_test_mode_adapter::~mac_test_mode_adapter() = default;

mac_cell_control_information_handler& mac_test_mode_adapter::get_control_info_handler(du_cell_index_t cell_index)
{
  if (cell_info_handler.size() <= cell_index) {
    cell_info_handler.resize(cell_index + 1);
  }
  if (cell_info_handler[cell_index] == nullptr) {
    cell_info_handler[cell_index] = std::make_unique<test_cell_adapter>(
        test_ue, mac_adapted->get_control_info_handler(cell_index), mac_adapted->get_pdu_handler(cell_index));
  }
  return *cell_info_handler[cell_index];
}

void mac_test_mode_adapter::handle_dl_buffer_state_update_required(const mac_dl_buffer_state_indication_message& dl_bs)
{
  mac_dl_buffer_state_indication_message dl_bs_copy = dl_bs;
  if (test_ue_index == dl_bs.ue_index and test_ue.pdsch_active) {
    // It is the test UE. Set a positive DL buffer state if PDSCH is set to "activated".
    dl_bs_copy.bs = TEST_UE_DL_BUFFER_STATE_UPDATE_SIZE;
  }
  mac_adapted->get_ue_control_info_handler().handle_dl_buffer_state_update_required(dl_bs_copy);
}

std::vector<mac_logical_channel_config>
mac_test_mode_adapter::adapt_bearers(const std::vector<mac_logical_channel_config>& orig_bearers)
{
  static test_ue_mac_sdu_tx_builder_adapter dl_adapter;

  auto test_ue_adapted_bearers = orig_bearers;
  for (auto& bearer : test_ue_adapted_bearers) {
    if (bearer.lcid != lcid_t::LCID_SRB0) {
      if (test_ue.pdsch_active) {
        bearer.dl_bearer = &dl_adapter;
      }
    }
  }

  return test_ue_adapted_bearers;
}

async_task<mac_ue_create_response_message>
mac_test_mode_adapter::handle_ue_create_request(const mac_ue_create_request_message& cfg)
{
  mac_ue_create_request_message cfg_adapted = cfg;
  if (cfg_adapted.crnti == test_ue.rnti) {
    // It is the test UE.

    // Save UE index.
    test_ue_index = cfg_adapted.ue_index;

    // Add adapters to the UE config bearers before passing it to MAC.
    cfg_adapted.bearers = adapt_bearers(cfg.bearers);
  }

  return launch_async([this, cfg_adapted](coro_context<async_task<mac_ue_create_response_message>>& ctx) mutable {
    CORO_BEGIN(ctx);

    // Create the UE in mac instance.
    CORO_AWAIT_VALUE(mac_ue_create_response_message ret,
                     mac_adapted->get_ue_configurator().handle_ue_create_request(cfg_adapted));

    if (test_ue.rnti == cfg_adapted.crnti) {
      // It is the test UE.

      if (test_ue.pdsch_active) {
        // Update DL buffer state automatically.
        handle_dl_buffer_state_update_required({ret.ue_index, lcid_t::LCID_SRB1, TEST_UE_DL_BUFFER_STATE_UPDATE_SIZE});
      }

      if (test_ue.pusch_active) {
        // In case of PUSCH test mode is enabled, push a BSR.
        mac_adapted->get_pdu_handler(cfg_adapted.cell_index)
            .handle_rx_data_indication(create_test_pdu_with_bsr(slot_point{0, 0}, test_ue.rnti, to_harq_id(0)));
      }
    }
    CORO_RETURN(ret);
  });
}

async_task<mac_ue_reconfiguration_response_message>
mac_test_mode_adapter::handle_ue_reconfiguration_request(const mac_ue_reconfiguration_request_message& cfg)
{
  if (cfg.crnti == test_ue.rnti) {
    // If it is the test UE.
    auto cfg_adapted = cfg;

    // Add adapters to the UE config bearers before passing it to MAC.
    cfg_adapted.bearers_to_addmod = adapt_bearers(cfg.bearers_to_addmod);

    return mac_adapted->get_ue_configurator().handle_ue_reconfiguration_request(cfg_adapted);
  }

  // otherwise, just forward config.
  return mac_adapted->get_ue_configurator().handle_ue_reconfiguration_request(cfg);
}

async_task<mac_ue_delete_response_message>
mac_test_mode_adapter::handle_ue_delete_request(const mac_ue_delete_request_message& cfg)
{
  return mac_adapted->get_ue_configurator().handle_ue_delete_request(cfg);
}

void mac_test_mode_adapter::handle_ul_ccch_msg(du_ue_index_t ue_index, byte_buffer pdu)
{
  mac_adapted->get_ue_configurator().handle_ul_ccch_msg(ue_index, std::move(pdu));
}
