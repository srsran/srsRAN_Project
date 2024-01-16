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

#include "srsran/scheduler/config/scheduler_cell_config_validator.h"
#include "../support/dmrs_helpers.h"
#include "../support/pdsch/pdsch_default_time_allocation.h"
#include "../support/prbs_calculator.h"
#include "srsran/ran/duplex_mode.h"
#include "srsran/ran/prach/prach_configuration.h"
#include "srsran/ran/prach/prach_frequency_mapping.h"
#include "srsran/ran/prach/prach_helper.h"
#include "srsran/ran/prach/prach_preamble_information.h"
#include "srsran/scheduler/sched_consts.h"

using namespace srsran;
using namespace config_validators;

#define VERIFY(cond, ...)                                                                                              \
  if (not(cond)) {                                                                                                     \
    return error_type<std::string>(fmt::format(__VA_ARGS__));                                                          \
  }
#define HANDLE_CODE(cond)                                                                                              \
  {                                                                                                                    \
    auto ret = cond;                                                                                                   \
    if (ret.is_error()) {                                                                                              \
      return ret;                                                                                                      \
    }                                                                                                                  \
  }

static error_type<std::string> validate_pdcch_cfg_common(const sched_cell_configuration_request_message& msg)
{
  for (const auto& ss : msg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces) {
    bool cset_id_exits_in_common =
        msg.dl_cfg_common.init_dl_bwp.pdcch_common.common_coreset.has_value()
            ? ss.get_coreset_id() == msg.dl_cfg_common.init_dl_bwp.pdcch_common.common_coreset->id
            : false;
    bool cset_id_exits_in_cset0 =
        msg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0.has_value() ? ss.get_coreset_id() == 0 : false;
    VERIFY(cset_id_exits_in_common or cset_id_exits_in_cset0,
           "Coreset Id. {} indexed by SearchSpace Id. {} not found within the configured Common Coresets",
           ss.get_coreset_id(),
           ss.get_id());
  }

  return {};
}

static error_type<std::string> validate_rach_cfg_common(const sched_cell_configuration_request_message& msg,
                                                        const scheduler_expert_config&                  expert_cfg)
{
  VERIFY(msg.ul_cfg_common.init_ul_bwp.rach_cfg_common.has_value(),
         "Cells without RACH-ConfigCommon are not supported");
  const rach_config_common& rach_cfg_cmn = msg.ul_cfg_common.init_ul_bwp.rach_cfg_common.value();

  static const pdsch_mcs_table mcs_table = srsran::pdsch_mcs_table::qam64;
  const sch_mcs_description    mcs_descr = pdsch_mcs_get_config(mcs_table, expert_cfg.ra.rar_mcs_index);
  // See TS 38.214, 5.1.3.1, Modulation order and target code rate determination.
  VERIFY((unsigned)mcs_descr.modulation < (unsigned)modulation_scheme::QAM64,
         "Modulation order for PDSCH scheduled with RA-RNTI cannot be > 2");

  duplex_mode dplx_mode = band_helper::get_duplex_mode(msg.dl_carrier.band);

  // Check PRACH config index.
  auto code = prach_helper::prach_config_index_is_valid(rach_cfg_cmn.rach_cfg_generic.prach_config_index, dplx_mode);
  if (code.is_error()) {
    return code;
  }

  const prach_configuration prach_cfg =
      prach_configuration_get(frequency_range::FR1,
                              msg.tdd_ul_dl_cfg_common.has_value() ? duplex_mode::TDD : duplex_mode::FDD,
                              rach_cfg_cmn.rach_cfg_generic.prach_config_index);
  VERIFY(prach_cfg.format < prach_format_type::invalid, "Invalid PRACH format");

  subcarrier_spacing pusch_scs = msg.ul_cfg_common.init_ul_bwp.generic_params.scs;

  // Check if the PRACH preambles fall into UL slots
  if (msg.tdd_ul_dl_cfg_common.has_value()) {
    auto ret = prach_helper::prach_fits_in_tdd_pattern(
        pusch_scs, rach_cfg_cmn.rach_cfg_generic.prach_config_index, *msg.tdd_ul_dl_cfg_common);
    if (ret.is_error()) {
      std::string s = fmt::format("PRACH configuration index {} not supported with current TDD pattern.",
                                  rach_cfg_cmn.rach_cfg_generic.prach_config_index);
      if (ret.error().empty()) {
        return s + fmt::format(" Cause: PRACH configuration is not valid");
      } else {
        return s + fmt::format(" Cause: Slot indexes used for PRACH {} fall outside TDD UL slots", ret.error());
      }
    }
  }

  // Check zero correlation zone validity.
  code = prach_helper::zero_correlation_zone_is_valid(rach_cfg_cmn.rach_cfg_generic.zero_correlation_zone_config,
                                                      rach_cfg_cmn.rach_cfg_generic.prach_config_index,
                                                      dplx_mode);
  if (code.is_error()) {
    return code;
  }

  prach_subcarrier_spacing prach_scs      = is_long_preamble(prach_cfg.format)
                                                ? get_prach_preamble_long_info(prach_cfg.format).scs
                                                : to_ra_subcarrier_spacing(pusch_scs);
  const unsigned           prach_nof_prbs = prach_frequency_mapping_get(prach_scs, pusch_scs).nof_rb_ra;
  for (unsigned id_fd_ra = 0; id_fd_ra != rach_cfg_cmn.rach_cfg_generic.msg1_fdm; ++id_fd_ra) {
    uint8_t      prb_start  = rach_cfg_cmn.rach_cfg_generic.msg1_frequency_start + id_fd_ra * prach_nof_prbs;
    prb_interval prach_prbs = {prb_start, prb_start + prach_nof_prbs};

    for (const auto& pucch_region : msg.pucch_guardbands) {
      if (pucch_region.prbs.overlaps(prach_prbs))
        fmt::print("Warning: Configured PRACH occasion collides with PUCCH RBs ({} intersects {}). Some interference "
                   "between PUCCH and PRACH is expected.\n",
                   pucch_region.prbs,
                   prach_prbs);
      break;
    }
  }

  return {};
}

static error_type<std::string> validate_pucch_cfg_common(const sched_cell_configuration_request_message& msg)
{
  for (const auto& pucch_guard : msg.pucch_guardbands) {
    VERIFY(msg.ul_cfg_common.init_ul_bwp.generic_params.crbs.contains(pucch_guard.prbs),
           "PUCCH guardbands={} fall outside of the initial BWP RBs={}",
           pucch_guard.prbs,
           msg.ul_cfg_common.init_ul_bwp.generic_params.crbs);
  }

  return {};
}

static error_type<std::string> validate_sib1_cfg(const sched_cell_configuration_request_message& msg,
                                                 const scheduler_expert_config&                  expert_cfg)
{
  static const unsigned        nof_layers = 1;
  static const unsigned        nof_oh_prb = 0;
  static const pdsch_mcs_table mcs_table  = srsran::pdsch_mcs_table::qam64;

  // TODO: Revise the value set for time_resource in case of partial slots where nof. OFDM symbols maybe be less.
  static const unsigned time_resource = 0;
  const auto&           pdsch_td_res_alloc_list =
      get_si_rnti_pdsch_time_domain_list(msg.dl_cfg_common.init_dl_bwp.generic_params.cp, msg.dmrs_typeA_pos);
  const ofdm_symbol_range sib1_symbols = pdsch_td_res_alloc_list[time_resource].symbols;

  const dmrs_information    dmrs_info = make_dmrs_info_common(pdsch_td_res_alloc_list, 0, msg.pci, msg.dmrs_typeA_pos);
  const sch_mcs_description mcs_descr = pdsch_mcs_get_config(mcs_table, expert_cfg.si.sib1_mcs_index);
  // See TS 38.214, 5.1.3.1, Modulation order and target code rate determination.
  VERIFY((unsigned)mcs_descr.modulation < (unsigned)modulation_scheme::QAM64,
         "Modulation order for PDSCH scheduled with SI-RNTI cannot be > 2");
  const sch_prbs_tbs sib1_prbs_tbs = get_nof_prbs(prbs_calculator_sch_config{msg.sib1_payload_size,
                                                                             (unsigned)sib1_symbols.length(),
                                                                             calculate_nof_dmrs_per_rb(dmrs_info),
                                                                             nof_oh_prb,
                                                                             mcs_descr,
                                                                             nof_layers});

  VERIFY(sib1_prbs_tbs.nof_prbs <= msg.dl_cfg_common.init_dl_bwp.generic_params.crbs.length(),
         "Not enough initial DL BWP PRBs ({} > {}) to send SIB1, given the chosen MCS={} and SIB1 payload size={}. "
         "Consider increasing SIB1 MCS or decrease the SIB1 payload size.",
         msg.dl_cfg_common.init_dl_bwp.generic_params.crbs.length(),
         sib1_prbs_tbs.nof_prbs,
         expert_cfg.si.sib1_mcs_index,
         msg.sib1_payload_size);

  return {};
}

static error_type<std::string> validate_paging_cfg(const scheduler_expert_config& expert_cfg)
{
  static const pdsch_mcs_table mcs_table = srsran::pdsch_mcs_table::qam64;
  const sch_mcs_description    mcs_descr = pdsch_mcs_get_config(mcs_table, expert_cfg.pg.paging_mcs_index);
  // See TS 38.214, 5.1.3.1, Modulation order and target code rate determination.
  VERIFY((unsigned)mcs_descr.modulation < (unsigned)modulation_scheme::QAM64,
         "Modulation order for PDSCH scheduled with P-RNTI cannot be > 2");

  return {};
}

/// \brief Validates \c sched_cell_configuration_request_message used to add a cell.
/// \param[in] msg scheduler cell configuration message to be validated.
/// \return In case an invalid parameter is detected, returns a string containing an error message.
error_type<std::string> srsran::config_validators::validate_sched_cell_configuration_request_message(
    const sched_cell_configuration_request_message& msg,
    const scheduler_expert_config&                  expert_cfg)
{
  VERIFY(msg.cell_index < MAX_NOF_DU_CELLS, "cell index={} is not valid", msg.cell_index);

  const auto& dl_lst = msg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list;
  for (const auto& pdsch : dl_lst) {
    VERIFY(pdsch.k0 <= SCHEDULER_MAX_K0, "k0={} value exceeds maximum supported k0", pdsch.k0);
  }

  if (msg.ul_cfg_common.init_ul_bwp.pusch_cfg_common.has_value()) {
    const auto& ul_lst = msg.ul_cfg_common.init_ul_bwp.pusch_cfg_common.value().pusch_td_alloc_list;
    for (const auto& pusch : ul_lst) {
      VERIFY(pusch.k2 <= SCHEDULER_MAX_K2, "k2={} value exceeds maximum supported k2", pusch.k2);
    }
  }

  HANDLE_CODE(validate_pdcch_cfg_common(msg));

  HANDLE_CODE(validate_rach_cfg_common(msg, expert_cfg));

  HANDLE_CODE(validate_pucch_cfg_common(msg));

  HANDLE_CODE(validate_sib1_cfg(msg, expert_cfg));

  HANDLE_CODE(validate_paging_cfg(expert_cfg));

  // TODO: Validate other parameters.
  return {};
}
