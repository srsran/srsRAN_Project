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

#include "scheduler_output_test_helpers.h"
#include "lib/scheduler/support/pdcch/pdcch_mapping.h"
#include "srsran/ran/pdcch/cce_to_prb_mapping.h"
#include "srsran/ran/prach/prach_configuration.h"

using namespace srsran;

std::vector<grant_info> srsran::get_pdcch_grant_info(pci_t pci, const pdcch_dl_information& pdcch)
{
  std::vector<grant_info> grants;

  const bwp_configuration&     bwp_cfg = *pdcch.ctx.bwp_cfg;
  const coreset_configuration& cs_cfg  = *pdcch.ctx.coreset_cfg;
  prb_index_list               pdcch_prbs =
      pdcch_helper::cce_to_prb_mapping(bwp_cfg, cs_cfg, pci, pdcch.ctx.cces.aggr_lvl, pdcch.ctx.cces.ncce);
  for (unsigned prb : pdcch_prbs) {
    unsigned crb = prb_to_crb(bwp_cfg, prb);
    grants.push_back(grant_info{bwp_cfg.scs, ofdm_symbol_range{0U, (uint8_t)cs_cfg.duration}, {crb, crb + 1}});
  }
  return grants;
}

std::vector<grant_info> srsran::get_pdcch_grant_info(const pdcch_ul_information& pdcch)
{
  std::vector<grant_info> grants;

  const bwp_configuration&     bwp_cfg = *pdcch.ctx.bwp_cfg;
  const coreset_configuration& cs_cfg  = *pdcch.ctx.coreset_cfg;
  prb_index_list               pdcch_prbs =
      pdcch_helper::cce_to_prb_mapping(bwp_cfg, cs_cfg, MAX_PCI + 1, pdcch.ctx.cces.aggr_lvl, pdcch.ctx.cces.ncce);
  for (unsigned prb : pdcch_prbs) {
    unsigned crb = prb_to_crb(bwp_cfg, prb);
    grants.push_back(grant_info{bwp_cfg.scs, ofdm_symbol_range{0U, (uint8_t)cs_cfg.duration}, {crb, crb + 1}});
  }
  return grants;
}

static grant_info get_common_pdsch_grant_info(const bwp_downlink_common& bwp_cfg, const pdsch_information& pdsch)
{
  crb_interval cs0_crbs = bwp_cfg.pdcch_common.coreset0->coreset0_crbs();
  crb_interval crbs     = {pdsch.rbs.type1().start() + cs0_crbs.start(), pdsch.rbs.type1().stop() + cs0_crbs.start()};
  return grant_info{bwp_cfg.generic_params.scs, pdsch.symbols, crbs};
}

grant_info srsran::get_pdsch_grant_info(const bwp_downlink_common& bwp_cfg, const sib_information& sib)
{
  return get_common_pdsch_grant_info(bwp_cfg, sib.pdsch_cfg);
}

grant_info srsran::get_pdsch_grant_info(const bwp_downlink_common& bwp_cfg, const rar_information& rar)
{
  return get_common_pdsch_grant_info(bwp_cfg, rar.pdsch_cfg);
}

grant_info srsran::get_pdsch_grant_info(const bwp_downlink_common& bwp_cfg, const dl_paging_allocation& pg)
{
  // See TS 38.212, section 7.3.1.2.1. DCI Format 1_0.
  return get_common_pdsch_grant_info(bwp_cfg, pg.pdsch_cfg);
}

grant_info srsran::get_pdsch_grant_info(const bwp_downlink_common& bwp_cfg, const dl_msg_alloc& ue_grant)
{
  const vrb_interval vrbs   = ue_grant.pdsch_cfg.rbs.type1();
  unsigned           ref_rb = 0;
  if (ue_grant.pdsch_cfg.ss_set_type != search_space_set_type::ue_specific and
      ue_grant.pdsch_cfg.dci_fmt == dci_dl_format::f1_0) {
    ref_rb = ue_grant.pdsch_cfg.coreset_cfg->get_coreset_start_crb();
  } else {
    ref_rb = ue_grant.pdsch_cfg.bwp_cfg->crbs.start();
  }
  crb_interval crbs = {vrbs.start() + ref_rb, vrbs.stop() + ref_rb};
  return grant_info{ue_grant.pdsch_cfg.bwp_cfg->scs, ue_grant.pdsch_cfg.symbols, crbs};
}

std::vector<test_grant_info> srsran::get_dl_grants(const cell_configuration& cell_cfg, const dl_sched_result& dl_res)
{
  std::vector<test_grant_info> grants;

  // Fill SSB.
  for (const ssb_information& ssb : dl_res.bc.ssb_info) {
    grants.emplace_back();
    grants.back().type  = test_grant_info::SSB;
    grants.back().rnti  = rnti_t::INVALID_RNTI;
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
    grants.back().grant = get_pdsch_grant_info(cell_cfg.dl_cfg_common.init_dl_bwp, rar);
  }

  // Register UE PDSCHs.
  for (const dl_msg_alloc& ue_pdsch : dl_res.ue_grants) {
    grants.emplace_back();
    grants.back().type  = test_grant_info::UE_DL;
    grants.back().rnti  = ue_pdsch.pdsch_cfg.rnti;
    grants.back().grant = get_pdsch_grant_info(cell_cfg.dl_cfg_common.init_dl_bwp, ue_pdsch);
  }

  for (const dl_paging_allocation& pg : dl_res.paging_grants) {
    grants.emplace_back();
    grants.back().type  = test_grant_info::PAGING;
    grants.back().rnti  = pg.pdsch_cfg.rnti;
    grants.back().grant = get_pdsch_grant_info(cell_cfg.dl_cfg_common.init_dl_bwp, pg);
  }

  return grants;
}

std::vector<test_grant_info> srsran::get_ul_grants(const cell_configuration& cell_cfg, const ul_sched_result& ul_res)
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
      grants.back().rnti  = rnti_t::INVALID_RNTI;
      grants.back().grant = grant_info{cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs, symbols, crbs};
    }
  }

  // Fill PUSCHs.
  for (const ul_sched_info& pusch : ul_res.puschs) {
    const bwp_configuration& bwp_cfg = *pusch.pusch_cfg.bwp_cfg;
    prb_interval             prbs    = {pusch.pusch_cfg.rbs.type1().start(), pusch.pusch_cfg.rbs.type1().stop()};
    crb_interval             crbs    = prb_to_crb(bwp_cfg, prbs);
    grants.emplace_back();
    grants.back().type  = test_grant_info::UE_UL;
    grants.back().rnti  = rnti_t::INVALID_RNTI;
    grants.back().grant = grant_info{pusch.pusch_cfg.bwp_cfg->scs, pusch.pusch_cfg.symbols, crbs};
  }

  // Fill PUCCHs.
  for (const pucch_info& pucch : ul_res.pucchs) {
    const bwp_configuration& bwp_cfg = *pucch.bwp_cfg;
    grants.emplace_back();
    grants.back().type = test_grant_info::PUCCH;
    grants.back().rnti = rnti_t::INVALID_RNTI;
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
      grants.back().rnti = rnti_t::INVALID_RNTI;
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
