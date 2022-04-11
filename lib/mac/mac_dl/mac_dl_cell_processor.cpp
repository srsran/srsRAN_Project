
#include "mac_dl_cell_processor.h"
#include "dci_encoder.h"
#include "pdu_encoder.h"
#include "srsgnb/mac/mac_cell_result.h"
#include "srsgnb/support/async/execute_on.h"
#include "ssb_encoder.h"

using namespace srsgnb;

mac_dl_cell_processor::mac_dl_cell_processor(mac_common_config_t&          cfg_,
                                             const mac_cell_configuration& cell_cfg_,
                                             sched_interface&              sched_,
                                             mac_dl_ue_manager&            ue_mng_) :
  cfg(cfg_),
  logger(cfg.logger),
  cell_cfg(cell_cfg_),
  cell_exec(*cfg.dl_execs[cell_cfg.cell_index]),
  phy_cell(cfg.phy_notifier.get_cell(cell_cfg.cell_index)),
  sched_obj(sched_),
  ue_mng(ue_mng_)
{}

async_task<void> mac_dl_cell_processor::start()
{
  return dispatch_and_resume_on(cell_exec, cfg.ctrl_exec, [this]() { state = cell_state::active; });
}

async_task<void> mac_dl_cell_processor::stop()
{
  return dispatch_and_resume_on(cell_exec, cfg.ctrl_exec, [this]() { state = cell_state::inactive; });
}

void mac_dl_cell_processor::handle_slot_indication(slot_point sl_tx)
{
  // Change execution context to DL executors.
  cell_exec.execute([this, sl_tx]() { handle_slot_indication_impl(sl_tx); });
}

void mac_dl_cell_processor::handle_slot_indication_impl(slot_point sl_tx)
{
  if (state != cell_state::active) {
    // TODO: Report empty result to PHY.
    return;
  }

  // Generate DL scheduling result for provided slot and cell.
  const dl_sched_result* dl_res = sched_obj.get_dl_sched(sl_tx, cell_cfg.cell_index);
  if (dl_res == nullptr) {
    logger.warning("Unable to compute scheduling result for slot={}, cell={}", sl_tx, cell_cfg.cell_index);
    return;
  }

  // Assemble MAC DL scheduling request that is going to be passed to the PHY.
  mac_dl_sched_result mac_dl_res;
  assemble_dl_sched_request(mac_dl_res, cell_cfg.cell_index, *dl_res);

  // Send DL sched result to PHY.
  phy_cell.on_new_downlink_scheduler_results(mac_dl_res);

  // Start assembling Slot Data Result.
  mac_dl_data_result data_res;
  assemble_dl_data_request(cell_cfg.cell_index, *dl_res, data_res);

  // Send DL Data to PHY.
  phy_cell.on_new_downlink_data(data_res);

  // Send UL sched result to PHY.
  const ul_sched_result* ul_res = sched_obj.get_ul_sched(sl_tx, cell_cfg.cell_index);
  mac_ul_sched_result    mac_ul_res;
  mac_ul_res.ul_res = ul_res;
  phy_cell.on_new_uplink_scheduler_results(mac_ul_res);
}

void mac_dl_cell_processor::assemble_dl_sched_request(mac_dl_sched_result&   mac_res,
                                                      du_cell_index_t        cell_index,
                                                      const dl_sched_result& dl_res)
{
  // Pass scheduler output directly to PHY.
  mac_res.dl_res = &dl_res;

  // TODO Assemble SSB scheduling info and additional SSB/MIB parameters to pass to PHY
  for (auto& ssb : dl_res.bc.ssb_info) {
    mac_res.ssb_pdu.emplace_back();
    ssb_helper.assemble_ssb(mac_res.ssb_pdu.back(), cell_cfg, ssb);
  }

  // Encode PDCCH DCI payloads.
  for (const auto& sib : dl_res.bc.sibs) {
    mac_res.pdcch_pdus.emplace_back();
    encode_dci(cell_cfg, sib, mac_res.pdcch_pdus.back().dci_payload);
  }
  for (const rar_information& rar : dl_res.rar_grants) {
    mac_res.pdcch_pdus.emplace_back();
    encode_dci(cell_cfg, rar, mac_res.pdcch_pdus.back().dci_payload);
  }
  for (const dl_msg_alloc& data : dl_res.ue_grants) {
    mac_res.pdcch_pdus.emplace_back();
    encode_dci(cell_cfg, data, mac_res.pdcch_pdus.back().dci_payload);
  }
}

void mac_dl_cell_processor::assemble_dl_data_request(du_cell_index_t        cell_index,
                                                     const dl_sched_result& dl_res,
                                                     mac_dl_data_result&    data_res)
{
  data_res.slot = dl_res.slot_value;

  // Assemble scheduled SIBs' payload.
  for (const sib_information& sib : dl_res.bc.sibs) {
    data_res.pdus.emplace_back();
    encode_sib_pdu(cell_cfg, sib, data_res.pdus.back());
  }

  // Assemble scheduled RARs' payload.
  for (const rar_information& rar : dl_res.rar_grants) {
    data_res.pdus.emplace_back();
    // call MAC encoder.
    encode_rar_pdu(cell_cfg, rar, data_res.pdus.back());
  }

  // Assemble data grants.
  for (const dl_msg_alloc& grant : dl_res.ue_grants) {
    for (const dl_msg_tb_info& tb_info : grant.tbs) {
      for (const dl_msg_lc_info& bearer_alloc : tb_info.lc_lst) {
        // Fetch RLC Bearer.
        mac_sdu_tx_builder* bearer = ue_mng.get_bearer(grant.crnti, bearer_alloc.lcid);
        srsran_sanity_check(bearer != nullptr, "Scheduler is allocating inexistent bearers");

        // Assemble MAC SDU.
        data_res.pdus.emplace_back();
        byte_buffer& sdu = data_res.pdus.back();
        sdu.resize(bearer_alloc.sched_bytes);
        bearer->on_new_tx_sdu(sdu);
      }
    }
  }
}

const ssb_assembler& mac_dl_cell_processor::get_ssb_configuration() const
{
  return ssb_helper;
}

void mac_dl_cell_processor::set_ssb_configuration(const mac_cell_configuration& mac_cell_cfg)
{
  ssb_helper.set_ssb_configuration(mac_cell_cfg);
}