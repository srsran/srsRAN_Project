/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "mac_cell_processor.h"
#include "sib_pdu_assembler.h"
#include "srsgnb/mac/mac_cell_result.h"
#include "srsgnb/support/async/execute_on.h"

using namespace srsgnb;

mac_cell_processor::mac_cell_processor(mac_common_config_t&             cfg_,
                                       const mac_cell_creation_request& cell_cfg_req_,
                                       mac_scheduler&                   sched_,
                                       mac_dl_ue_manager&               ue_mng_) :
  cfg(cfg_),
  logger(cfg.logger),
  cell_cfg(cell_cfg_req_),
  cell_exec(cfg.dl_exec_mapper.executor(cell_cfg.cell_index)),
  phy_cell(cfg.phy_notifier.get_cell(cell_cfg.cell_index)),
  ssb_helper(cell_cfg_req_),
  sib_assembler(cell_cfg_req_.bcch_dl_sch_payload),
  rar_assembler(cell_cfg_req_),
  sched_obj(sched_),
  ue_mng(ue_mng_)
{
}

async_task<void> mac_cell_processor::start()
{
  return dispatch_and_resume_on(cell_exec, cfg.ctrl_exec, [this]() { state = cell_state::active; });
}

async_task<void> mac_cell_processor::stop()
{
  return dispatch_and_resume_on(cell_exec, cfg.ctrl_exec, [this]() { state = cell_state::inactive; });
}

void mac_cell_processor::handle_slot_indication(slot_point sl_tx)
{
  // Change execution context to DL executors.
  cell_exec.execute([this, sl_tx]() { handle_slot_indication_impl(sl_tx); });
}

void mac_cell_processor::handle_crc(const mac_crc_indication_message& msg)
{
  ul_crc_indication ind{};
  ind.cell_index = cell_cfg.cell_index;
  ind.crcs.resize(msg.crcs.size());
  for (unsigned i = 0; i != ind.crcs.size(); ++i) {
    ind.crcs[i].ue_index       = ue_mng.get_ue_index(msg.crcs[i].rnti);
    ind.crcs[i].harq_id        = msg.crcs[i].harq_id;
    ind.crcs[i].tb_crc_success = msg.crcs[i].tb_crc_success;
  }
  sched_obj.handle_crc_indication(ind);
}

void mac_cell_processor::handle_slot_indication_impl(slot_point sl_tx)
{
  mac_dl_sched_result mac_dl_res{};

  if (state != cell_state::active) {
    phy_cell.on_new_downlink_scheduler_results(mac_dl_res);
    return;
  }

  // Generate DL scheduling result for provided slot and cell.
  const sched_result* sl_res = sched_obj.slot_indication(sl_tx, cell_cfg.cell_index);
  if (sl_res == nullptr) {
    logger.warning("Unable to compute scheduling result for slot={}, cell={}", sl_tx, cell_cfg.cell_index);
    phy_cell.on_new_downlink_scheduler_results(mac_dl_res);
    return;
  }

  // Assemble MAC DL scheduling request that is going to be passed to the PHY.
  assemble_dl_sched_request(mac_dl_res, sl_tx, cell_cfg.cell_index, sl_res->dl);

  // Send DL sched result to PHY.
  phy_cell.on_new_downlink_scheduler_results(mac_dl_res);

  // Start assembling Slot Data Result.
  mac_dl_data_result data_res;
  assemble_dl_data_request(data_res, sl_tx, cell_cfg.cell_index, sl_res->dl);

  // Send DL Data to PHY.
  phy_cell.on_new_downlink_data(data_res);

  // Send UL sched result to PHY.
  mac_ul_sched_result mac_ul_res{};
  mac_ul_res.slot   = sl_tx;
  mac_ul_res.ul_res = &sl_res->ul;
  phy_cell.on_new_uplink_scheduler_results(mac_ul_res);

  // Update DL buffer state for the allocated logical channels.
  update_logical_channel_dl_buffer_states(sl_res->dl);
}

/// Encodes DL DCI.
static dci_payload encode_dci(const pdcch_dl_information& pdcch)
{
  switch (pdcch.dci.type) {
    case dci_dl_rnti_config_type::si_f1_0:
      return dci_1_0_si_rnti_pack(pdcch.dci.si_f1_0);
    case dci_dl_rnti_config_type::ra_f1_0:
      return dci_1_0_ra_rnti_pack(pdcch.dci.ra_f1_0);
    case dci_dl_rnti_config_type::ue_f1_0:
    default:
      srsgnb_terminate("Invalid DCI format");
  }
}

void mac_cell_processor::assemble_dl_sched_request(mac_dl_sched_result&   mac_res,
                                                   slot_point             sl_tx,
                                                   du_cell_index_t        cell_index,
                                                   const dl_sched_result& dl_res)
{
  // Pass scheduler output directly to PHY.
  mac_res.dl_res = &dl_res;
  mac_res.slot   = sl_tx;

  // Assemble SSB scheduling info and additional SSB/MIB parameters to pass to PHY.
  for (auto& ssb : dl_res.bc.ssb_info) {
    mac_res.ssb_pdu.emplace_back();
    ssb_helper.assemble_ssb(mac_res.ssb_pdu.back(), ssb);
  }

  // Encode PDCCH DCI payloads.
  for (const pdcch_dl_information& pdcch : dl_res.dl_pdcchs) {
    mac_res.pdcch_pdus.push_back(encode_dci(pdcch));
  }
}

void mac_cell_processor::assemble_dl_data_request(mac_dl_data_result&    data_res,
                                                  slot_point             sl_tx,
                                                  du_cell_index_t        cell_index,
                                                  const dl_sched_result& dl_res)
{
  static const unsigned MIN_MAC_SDU_SIZE = 3;

  data_res.slot = sl_tx;
  // Assemble scheduled BCCH-DL-SCH message containing SIBs' payload.
  for (const sib_information& sib_info : dl_res.bc.sibs) {
    srsgnb_assert(not data_res.sib1_pdus.full(), "No SIB1 added as SIB1 list in MAC DL data results is already full");
    span<const uint8_t> payload = sib_assembler.encode_sib_pdu(sib_info.pdsch_cfg.codewords[0].tb_size_bytes);
    data_res.sib1_pdus.emplace_back(payload);
  }

  // Assemble scheduled RARs' subheaders and payloads.
  for (const rar_information& rar : dl_res.rar_grants) {
    data_res.rar_pdus.emplace_back(rar_assembler.encode_rar_pdu(rar));
  }

  // Assemble data grants.
  for (const dl_msg_alloc& grant : dl_res.ue_grants) {
    for (const dl_msg_tb_info& tb_info : grant.tb_list) {
      for (const dl_msg_lc_info& subpdu : tb_info.subpdus) {
        if (subpdu.lcid.is_sdu()) {
          // Encode MAC SDU.

          // Fetch RLC Bearer.
          mac_sdu_tx_builder* bearer = ue_mng.get_bearer(grant.pdsch_cfg.rnti, subpdu.lcid.to_lcid());
          srsgnb_sanity_check(bearer != nullptr, "Scheduler is allocating inexistent bearers");

          unsigned rem_bytes = subpdu.sched_bytes;
          while (rem_bytes > MIN_MAC_SDU_SIZE) {
            // Assemble MAC Tx SDU.
            byte_buffer_slice_chain sdu = bearer->on_new_tx_sdu(subpdu.sched_bytes);
            if (sdu.empty()) {
              // TODO: Handle.
              break;
            }

            // TODO: Add subheader.

            data_res.ue_pdus.emplace_back(std::move(sdu));
            rem_bytes -= sdu.length();
          }
        } else {
          // Encode MAC CE.

          // TODO: Encode subPDU.
        }
      }
    }
  }
}

void mac_cell_processor::update_logical_channel_dl_buffer_states(const dl_sched_result& dl_res)
{
  for (const dl_msg_alloc& grant : dl_res.ue_grants) {
    for (const dl_msg_tb_info& tb_info : grant.tb_list) {
      for (const dl_msg_lc_info& subpdu : tb_info.subpdus) {
        if (not subpdu.lcid.is_sdu()) {
          continue;
        }

        // Fetch RLC Bearer.
        mac_sdu_tx_builder* bearer = ue_mng.get_bearer(grant.pdsch_cfg.rnti, subpdu.lcid.to_lcid());
        srsgnb_sanity_check(bearer != nullptr, "Scheduler is allocating inexistent bearers");

        // Update DL BSR for the allocated logical channel.
        dl_buffer_state_indication_message bs{};
        bs.ue_index = ue_mng.get_ue_index(grant.pdsch_cfg.rnti);
        bs.rnti     = grant.pdsch_cfg.rnti;
        bs.lcid     = subpdu.lcid.to_lcid();
        bs.bs       = bearer->on_buffer_state_update();
        sched_obj.handle_dl_buffer_state_update_indication(bs);
      }
    }
  }
}
