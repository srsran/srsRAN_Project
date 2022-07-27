/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "scheduler_output_test_helpers.h"
#include "lib/scheduler/pdcch_scheduling/pdcch_config_helpers.h"
#include "srsgnb/ran/pdcch/cce_to_prb_mapping.h"

using namespace srsgnb;

std::vector<grant_info> srsgnb::get_pdcch_grant_info(pci_t pci, const pdcch_dl_information& pdcch)
{
  std::vector<grant_info> grants;

  const bwp_configuration&     bwp_cfg = *pdcch.ctx.bwp_cfg;
  const coreset_configuration& cs_cfg  = *pdcch.ctx.coreset_cfg;
  prb_index_list pdcch_prbs = cce_to_prb_mapping(bwp_cfg, cs_cfg, pci, pdcch.ctx.cces.aggr_lvl, pdcch.ctx.cces.ncce);
  for (unsigned prb : pdcch_prbs) {
    unsigned crb = prb_to_crb(bwp_cfg, prb);
    grants.push_back(grant_info{
        grant_info::channel::cch, bwp_cfg.scs, ofdm_symbol_range{0U, (uint8_t)cs_cfg.duration}, {crb, crb + 1}});
  }
  return grants;
}

std::vector<grant_info> srsgnb::get_pdcch_grant_info(const pdcch_ul_information& pdcch)
{
  std::vector<grant_info> grants;

  const bwp_configuration&     bwp_cfg = *pdcch.ctx.bwp_cfg;
  const coreset_configuration& cs_cfg  = *pdcch.ctx.coreset_cfg;
  prb_index_list               pdcch_prbs =
      cce_to_prb_mapping(bwp_cfg, cs_cfg, MAX_PCI + 1, pdcch.ctx.cces.aggr_lvl, pdcch.ctx.cces.ncce);
  for (unsigned prb : pdcch_prbs) {
    unsigned crb = prb_to_crb(bwp_cfg, prb);
    grants.push_back(grant_info{
        grant_info::channel::cch, bwp_cfg.scs, ofdm_symbol_range{0U, (uint8_t)cs_cfg.duration}, {crb, crb + 1}});
  }
  return grants;
}

grant_info srsgnb::get_pdsch_grant_info(const bwp_downlink_common& bwp_cfg, const sib_information& sib)
{
  bwp_configuration coreset0_bwp_cfg = bwp_cfg.generic_params;
  coreset0_bwp_cfg.crbs              = bwp_cfg.pdcch_common.coreset0->coreset0_crbs();

  crb_interval crbs = prb_to_crb(coreset0_bwp_cfg, sib.pdsch_cfg.prbs.prbs());
  return grant_info{grant_info::channel::sch, sib.pdsch_cfg.bwp_cfg->scs, sib.pdsch_cfg.symbols, crbs};
}

grant_info srsgnb::get_pdsch_grant_info(const rar_information& rar)
{
  crb_interval crbs = prb_to_crb(*rar.pdsch_cfg.bwp_cfg, rar.pdsch_cfg.prbs.prbs());
  return grant_info{grant_info::channel::sch, rar.pdsch_cfg.bwp_cfg->scs, rar.pdsch_cfg.symbols, crbs};
}

grant_info srsgnb::get_pdsch_grant_info(const dl_msg_alloc& ue_grant)
{
  crb_interval crbs = prb_to_crb(*ue_grant.pdsch_cfg.bwp_cfg, ue_grant.pdsch_cfg.prbs.prbs());
  return grant_info{grant_info::channel::sch, ue_grant.pdsch_cfg.bwp_cfg->scs, ue_grant.pdsch_cfg.symbols, crbs};
}

std::vector<test_grant_info> srsgnb::get_dl_grants(const cell_configuration& cell_cfg, const dl_sched_result& dl_res)
{
  std::vector<test_grant_info> grants;

  // Fill SSB.
  for (const ssb_information& ssb : dl_res.bc.ssb_info) {
    grants.emplace_back();
    grants.back().type  = test_grant_info::SSB;
    grants.back().rnti  = INVALID_RNTI;
    grants.back().grant = grant_info{grant_info::channel::ssb, cell_cfg.ssb_cfg.scs, ssb.symbols, ssb.crbs};
  }

  // Fill DL PDCCHs.
  for (const pdcch_dl_information& pdcch : dl_res.dl_pdcchs) {
    std::vector<grant_info> grant_res_list = get_pdcch_grant_info(cell_cfg.pci, pdcch);
    for (const grant_info& grant : grant_res_list) {
      grants.emplace_back();
      grants.back().type  = test_grant_info::DL_PDCCH;
      grants.back().rnti  = pdcch.ctx.rnti;
      grants.back().grant = grant;
    }
  }

  // Fill UL PDCCHs.
  for (const pdcch_ul_information& pdcch : dl_res.ul_pdcchs) {
    std::vector<grant_info> grant_res_list = get_pdcch_grant_info(pdcch);
    for (const grant_info& grant : grant_res_list) {
      grants.emplace_back();
      grants.back().type  = test_grant_info::DL_PDCCH;
      grants.back().rnti  = pdcch.ctx.rnti;
      grants.back().grant = grant;
    }
  }

  // Fill SIB1 PDSCH.
  for (const sib_information& sib : dl_res.bc.sibs) {
    grants.emplace_back();
    grants.back().type  = test_grant_info::SIB;
    grants.back().rnti  = sib.pdsch_cfg.rnti;
    grants.back().grant = get_pdsch_grant_info(cell_cfg.dl_cfg_common.init_dl_bwp, sib);
  }

  // Register RAR PDSCHs.
  for (const rar_information& rar : dl_res.rar_grants) {
    grants.emplace_back();
    grants.back().type  = test_grant_info::SIB;
    grants.back().rnti  = rar.pdsch_cfg.rnti;
    grants.back().grant = get_pdsch_grant_info(rar);
  }

  // Register UE PDSCHs.
  for (const dl_msg_alloc& ue_pdsch : dl_res.ue_grants) {
    grants.emplace_back();
    grants.back().type  = test_grant_info::SIB;
    grants.back().rnti  = ue_pdsch.pdsch_cfg.rnti;
    grants.back().grant = get_pdsch_grant_info(ue_pdsch);
  }

  return grants;
}
