/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "csi_rs_scheduler.h"

using namespace srsran;

/// CSI-RS locations within a slot. See TS 38.211 Table 7.4.1.5.3-1.
struct csi_rs_resource_mapping_info {
  uint8_t                  row;
  uint8_t                  nof_ports;
  csi_rs_freq_density_type density;
  csi_rs_cdm_type          cdm_type;
};
static const uint32_t nof_csi_rs_resource_mappings = 38;
static constexpr std::array<csi_rs_resource_mapping_info, nof_csi_rs_resource_mappings>
    csi_rs_resource_mapping_within_slot = {{
        // clang-format off
    {1,  1, csi_rs_freq_density_type::three, csi_rs_cdm_type::no_CDM},
    {2,  1, csi_rs_freq_density_type::one, csi_rs_cdm_type::no_CDM},
    {2,  1, csi_rs_freq_density_type::dot5_even_RB, csi_rs_cdm_type::no_CDM},
    {2,  1, csi_rs_freq_density_type::dot5_odd_RB, csi_rs_cdm_type::no_CDM},
    {3,  2, csi_rs_freq_density_type::one, csi_rs_cdm_type::fd_CDM2},
    {3,  2, csi_rs_freq_density_type::dot5_even_RB, csi_rs_cdm_type::fd_CDM2},
    {3,  2, csi_rs_freq_density_type::dot5_odd_RB, csi_rs_cdm_type::fd_CDM2},
    {4,  4, csi_rs_freq_density_type::one, csi_rs_cdm_type::fd_CDM2},
    {5,  4, csi_rs_freq_density_type::one, csi_rs_cdm_type::fd_CDM2},
    {6,  8, csi_rs_freq_density_type::one, csi_rs_cdm_type::fd_CDM2},
    {7,  8, csi_rs_freq_density_type::one, csi_rs_cdm_type::fd_CDM2},
    {8,  8, csi_rs_freq_density_type::one, csi_rs_cdm_type::cdm4_FD2_TD2},
    {9,  12, csi_rs_freq_density_type::one, csi_rs_cdm_type::fd_CDM2},
    {10,  12, csi_rs_freq_density_type::one, csi_rs_cdm_type::cdm4_FD2_TD2},
    {11,  16, csi_rs_freq_density_type::one, csi_rs_cdm_type::fd_CDM2},
    {11,  16, csi_rs_freq_density_type::dot5_even_RB, csi_rs_cdm_type::fd_CDM2},
    {11,  16, csi_rs_freq_density_type::dot5_odd_RB, csi_rs_cdm_type::fd_CDM2},
    {12,  16, csi_rs_freq_density_type::one, csi_rs_cdm_type::cdm4_FD2_TD2},
    {12,  16, csi_rs_freq_density_type::dot5_even_RB, csi_rs_cdm_type::cdm4_FD2_TD2},
    {12,  16, csi_rs_freq_density_type::dot5_odd_RB, csi_rs_cdm_type::cdm4_FD2_TD2},
    {13,  24, csi_rs_freq_density_type::one, csi_rs_cdm_type::fd_CDM2},
    {13,  24, csi_rs_freq_density_type::dot5_even_RB, csi_rs_cdm_type::fd_CDM2},
    {13,  24, csi_rs_freq_density_type::dot5_odd_RB, csi_rs_cdm_type::fd_CDM2},
    {14,  24, csi_rs_freq_density_type::one, csi_rs_cdm_type::cdm4_FD2_TD2},
    {14,  24, csi_rs_freq_density_type::dot5_even_RB, csi_rs_cdm_type::cdm4_FD2_TD2},
    {14,  24, csi_rs_freq_density_type::dot5_odd_RB, csi_rs_cdm_type::cdm4_FD2_TD2},
    {15,  24, csi_rs_freq_density_type::one, csi_rs_cdm_type::cdm8_FD2_TD4},
    {15,  24, csi_rs_freq_density_type::dot5_even_RB, csi_rs_cdm_type::cdm8_FD2_TD4},
    {15,  24, csi_rs_freq_density_type::dot5_odd_RB, csi_rs_cdm_type::cdm8_FD2_TD4},
    {16,  32, csi_rs_freq_density_type::one, csi_rs_cdm_type::fd_CDM2},
    {16,  32, csi_rs_freq_density_type::dot5_even_RB, csi_rs_cdm_type::fd_CDM2},
    {16,  32, csi_rs_freq_density_type::dot5_odd_RB, csi_rs_cdm_type::fd_CDM2},
    {17,  32, csi_rs_freq_density_type::one, csi_rs_cdm_type::cdm4_FD2_TD2},
    {17,  32, csi_rs_freq_density_type::dot5_even_RB, csi_rs_cdm_type::cdm4_FD2_TD2},
    {17,  32, csi_rs_freq_density_type::dot5_odd_RB, csi_rs_cdm_type::cdm4_FD2_TD2},
    {18,  32, csi_rs_freq_density_type::one, csi_rs_cdm_type::cdm8_FD2_TD4},
    {18,  32, csi_rs_freq_density_type::dot5_even_RB, csi_rs_cdm_type::cdm8_FD2_TD4},
    {18,  32, csi_rs_freq_density_type::dot5_odd_RB, csi_rs_cdm_type::cdm8_FD2_TD4}
        // clang-format on
    }};

static uint8_t
get_csi_rs_resource_mapping_row_number(uint8_t nof_ports, csi_rs_freq_density_type density, csi_rs_cdm_type cdm_type)
{
  for (const csi_rs_resource_mapping_info& info : csi_rs_resource_mapping_within_slot) {
    if (info.nof_ports == nof_ports and info.cdm_type == cdm_type and info.density == density) {
      return info.row;
    }
  }

  report_fatal_error("No CSI-RS resource mapping found for frequency density={}, CDM-type={}, nof. ports={}",
                     density,
                     cdm_type,
                     nof_ports);
}

static csi_rs_info build_csi_rs_info(const bwp_configuration& bwp_cfg, const nzp_csi_rs_resource& nzp_csi_rs_res)
{
  csi_rs_info csi_rs;

  csi_rs.bwp_cfg = &bwp_cfg;
  csi_rs.crbs    = {nzp_csi_rs_res.res_mapping.freq_band_start_rb,
                    nzp_csi_rs_res.res_mapping.freq_band_start_rb + nzp_csi_rs_res.res_mapping.freq_band_nof_rb};
  csi_rs.type    = srsran::csi_rs_type::CSI_RS_NZP;

  csi_rs.freq_domain = nzp_csi_rs_res.res_mapping.fd_alloc;
  csi_rs.row         = get_csi_rs_resource_mapping_row_number(
      nzp_csi_rs_res.res_mapping.nof_ports, nzp_csi_rs_res.res_mapping.freq_density, nzp_csi_rs_res.res_mapping.cdm);
  csi_rs.symbol0                      = nzp_csi_rs_res.res_mapping.first_ofdm_symbol_in_td;
  csi_rs.symbol1                      = nzp_csi_rs_res.res_mapping.first_ofdm_symbol_in_td2.has_value()
                                            ? *nzp_csi_rs_res.res_mapping.first_ofdm_symbol_in_td2
                                            : 2;
  csi_rs.cdm_type                     = nzp_csi_rs_res.res_mapping.cdm;
  csi_rs.freq_density                 = nzp_csi_rs_res.res_mapping.freq_density;
  csi_rs.scrambling_id                = nzp_csi_rs_res.scrambling_id;
  csi_rs.power_ctrl_offset_profile_nr = nzp_csi_rs_res.pwr_ctrl_offset;
  csi_rs.power_ctrl_offset_ss_profile_nr =
      nzp_csi_rs_res.pwr_ctrl_offset_ss_db.has_value() ? *nzp_csi_rs_res.pwr_ctrl_offset_ss_db : 0;

  return csi_rs;
}

csi_rs_scheduler::csi_rs_scheduler(const cell_configuration& cell_cfg_) : cell_cfg(cell_cfg_)
{
  if (cell_cfg.csi_meas_cfg.has_value()) {
    for (const auto& nzp_csi : cell_cfg.csi_meas_cfg->nzp_csi_rs_res_list) {
      cached_csi_rs.push_back(build_csi_rs_info(cell_cfg.dl_cfg_common.init_dl_bwp.generic_params, nzp_csi));
    }
  }
}

void csi_rs_scheduler::run_slot(cell_slot_resource_allocator& res_grid)
{
  if (cached_csi_rs.empty()) {
    return;
  }
  if (not cell_cfg.is_dl_enabled(res_grid.slot)) {
    return;
  }

  for (unsigned i = 0; i != cached_csi_rs.size(); ++i) {
    const nzp_csi_rs_resource& nzp_csi = cell_cfg.csi_meas_cfg->nzp_csi_rs_res_list[i];
    if ((res_grid.slot - *nzp_csi.csi_res_offset).to_uint() % (unsigned)*nzp_csi.csi_res_period == 0) {
      res_grid.result.dl.csi_rs.push_back(cached_csi_rs[i]);
    }
  }
}
