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
#include "srsgnb/ran/prach/prach_configuration.h"

using namespace srsgnb;

std::vector<grant_info> srsgnb::get_pdcch_grant_info(pci_t pci, const pdcch_dl_information& pdcch)
{
  std::vector<grant_info> grants;

  const bwp_configuration&     bwp_cfg = *pdcch.ctx.bwp_cfg;
  const coreset_configuration& cs_cfg  = *pdcch.ctx.coreset_cfg;
  prb_index_list pdcch_prbs = cce_to_prb_mapping(bwp_cfg, cs_cfg, pci, pdcch.ctx.cces.aggr_lvl, pdcch.ctx.cces.ncce);
  for (unsigned prb : pdcch_prbs) {
    unsigned crb = prb_to_crb(bwp_cfg, prb);
    grants.push_back(grant_info{bwp_cfg.scs, ofdm_symbol_range{0U, (uint8_t)cs_cfg.duration}, {crb, crb + 1}});
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
    grants.push_back(grant_info{bwp_cfg.scs, ofdm_symbol_range{0U, (uint8_t)cs_cfg.duration}, {crb, crb + 1}});
  }
  return grants;
}

grant_info srsgnb::get_pdsch_grant_info(const bwp_downlink_common& bwp_cfg, const sib_information& sib)
{
  bwp_configuration coreset0_bwp_cfg = bwp_cfg.generic_params;
  coreset0_bwp_cfg.crbs              = bwp_cfg.pdcch_common.coreset0->coreset0_crbs();

  crb_interval crbs = prb_to_crb(coreset0_bwp_cfg, sib.pdsch_cfg.prbs.prbs());
  return grant_info{sib.pdsch_cfg.bwp_cfg->scs, sib.pdsch_cfg.symbols, crbs};
}

grant_info srsgnb::get_pdsch_grant_info(const rar_information& rar)
{
  bwp_configuration bwp_cfg = *rar.pdsch_cfg.bwp_cfg;
  bwp_cfg.crbs              = rar.pdsch_cfg.coreset_cfg->coreset0_crbs();
  crb_interval crbs         = prb_to_crb(bwp_cfg, rar.pdsch_cfg.prbs.prbs());
  return grant_info{rar.pdsch_cfg.bwp_cfg->scs, rar.pdsch_cfg.symbols, crbs};
}

grant_info srsgnb::get_pdsch_grant_info(const dl_paging_allocation& pg)
{
  crb_interval crbs = prb_to_crb(*pg.pdsch_cfg.bwp_cfg, pg.pdsch_cfg.prbs.prbs());
  return grant_info{pg.pdsch_cfg.bwp_cfg->scs, pg.pdsch_cfg.symbols, crbs};
}

grant_info srsgnb::get_pdsch_grant_info(const dl_msg_alloc& ue_grant)
{
  crb_interval crbs = prb_to_crb(*ue_grant.pdsch_cfg.bwp_cfg, ue_grant.pdsch_cfg.prbs.prbs());
  return grant_info{ue_grant.pdsch_cfg.bwp_cfg->scs, ue_grant.pdsch_cfg.symbols, crbs};
}

std::vector<test_grant_info> srsgnb::get_dl_grants(const cell_configuration& cell_cfg, const dl_sched_result& dl_res)
{
  std::vector<test_grant_info> grants;

  // Fill SSB.
  for (const ssb_information& ssb : dl_res.bc.ssb_info) {
    grants.emplace_back();
    grants.back().type  = test_grant_info::SSB;
    grants.back().rnti  = INVALID_RNTI;
    grants.back().grant = grant_info{cell_cfg.ssb_cfg.scs, ssb.symbols, ssb.crbs};
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
      grants.back().type  = test_grant_info::UL_PDCCH;
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
    grants.back().type  = test_grant_info::RAR;
    grants.back().rnti  = rar.pdsch_cfg.rnti;
    grants.back().grant = get_pdsch_grant_info(rar);
  }

  // Register UE PDSCHs.
  for (const dl_msg_alloc& ue_pdsch : dl_res.ue_grants) {
    grants.emplace_back();
    grants.back().type  = test_grant_info::UE_DL;
    grants.back().rnti  = ue_pdsch.pdsch_cfg.rnti;
    grants.back().grant = get_pdsch_grant_info(ue_pdsch);
  }

  for (const dl_paging_allocation& pg : dl_res.paging_grants) {
    grants.emplace_back();
    grants.back().type  = test_grant_info::PAGING;
    grants.back().rnti  = pg.pdsch_cfg.rnti;
    grants.back().grant = get_pdsch_grant_info(pg);
  }

  return grants;
}

std::vector<test_grant_info> srsgnb::get_ul_grants(const cell_configuration& cell_cfg, const ul_sched_result& ul_res)
{
  std::vector<test_grant_info> grants;

  // Fill PRACHs.
  if (not ul_res.prachs.empty()) {
    prach_configuration prach_cfg = prach_configuration_get(
        frequency_range::FR1,
        cell_cfg.paired_spectrum ? duplex_mode::FDD : duplex_mode::TDD,
        cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.prach_config_index);

    for (const prach_occasion_info& prach : ul_res.prachs) {
      ofdm_symbol_range symbols{prach.start_symbol, (uint8_t)(prach.start_symbol + prach_cfg.duration)};
      unsigned prb_start = cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.msg1_frequency_start;
      prb_interval prbs{prb_start, prb_start + 6}; // TODO: Derive nof RBs.
      crb_interval crbs = prb_to_crb(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params, prbs);
      grants.emplace_back();
      grants.back().type  = test_grant_info::PRACH;
      grants.back().rnti  = INVALID_RNTI;
      grants.back().grant = grant_info{cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs, symbols, crbs};
    }
  }

  // Fill PUSCHs.
  for (const ul_sched_info& pusch : ul_res.puschs) {
    const bwp_configuration& bwp_cfg = *pusch.pusch_cfg.bwp_cfg;
    crb_interval             crbs    = prb_to_crb(bwp_cfg, pusch.pusch_cfg.prbs.prbs());
    grants.emplace_back();
    grants.back().type  = test_grant_info::UE_UL;
    grants.back().rnti  = INVALID_RNTI;
    grants.back().grant = grant_info{pusch.pusch_cfg.bwp_cfg->scs, pusch.pusch_cfg.symbols, crbs};
  }

  // Fill PUCCHs.
  for (const pucch_info& pucch : ul_res.pucchs) {
    const bwp_configuration& bwp_cfg = *pucch.bwp_cfg;
    grants.emplace_back();
    grants.back().type = test_grant_info::PUCCH;
    grants.back().rnti = INVALID_RNTI;
    if (not pucch.resources.second_hop_prbs.empty()) {
      ofdm_symbol_range first_hop_symbols{pucch.resources.symbols.start(),
                                          pucch.resources.symbols.start() + pucch.resources.symbols.length() / 2};
      ofdm_symbol_range second_hop_symbols{pucch.resources.symbols.start() + pucch.resources.symbols.length() / 2,
                                           pucch.resources.symbols.stop()};

      unsigned crb_first_hop = prb_to_crb(bwp_cfg, pucch.resources.prbs.start());
      grants.back().grant = grant_info{bwp_cfg.scs, first_hop_symbols, crb_interval{crb_first_hop, crb_first_hop + 1}};
      unsigned crb_second_hop = prb_to_crb(bwp_cfg, pucch.resources.second_hop_prbs.start());
      // Add a second resource for Frequency Hopping.
      grants.emplace_back();
      grants.back().type = test_grant_info::PUCCH;
      grants.back().rnti = INVALID_RNTI;
      grants.back().grant =
          grant_info{bwp_cfg.scs, second_hop_symbols, crb_interval{crb_second_hop, crb_second_hop + 1}};
    } else {
      unsigned crb_first_hop = prb_to_crb(bwp_cfg, pucch.resources.prbs.start());
      grants.back().grant =
          grant_info{bwp_cfg.scs, pucch.resources.symbols, crb_interval{crb_first_hop, crb_first_hop + 1}};
    }
  }

  return grants;
}
