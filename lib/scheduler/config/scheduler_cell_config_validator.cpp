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

#include "srsran/scheduler/config/scheduler_cell_config_validator.h"
#include "../support/dmrs_helpers.h"
#include "../support/prbs_calculator.h"
#include "srsran/ran/duplex_mode.h"
#include "srsran/ran/prach/prach_configuration.h"
#include "srsran/ran/prach/prach_frequency_mapping.h"
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
    bool cset_id_exits_in_common = msg.dl_cfg_common.init_dl_bwp.pdcch_common.common_coreset.has_value()
                                       ? ss.cs_id == msg.dl_cfg_common.init_dl_bwp.pdcch_common.common_coreset->id
                                       : false;
    bool cset_id_exits_in_cset0 =
        msg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0.has_value() ? ss.cs_id == 0 : false;
    VERIFY(cset_id_exits_in_common or cset_id_exits_in_cset0,
           "Coreset Id. {} indexed by SearchSpace Id. {} not found within the configured Common Coresets",
           ss.cs_id,
           ss.id);
  }

  return {};
}

static error_type<std::string> validate_rach_cfg_common(const sched_cell_configuration_request_message& msg)
{
  VERIFY(msg.ul_cfg_common.init_ul_bwp.rach_cfg_common.has_value(),
         "Cells without RACH-ConfigCommon are not supported");
  const rach_config_common& rach_cfg_cmn = msg.ul_cfg_common.init_ul_bwp.rach_cfg_common.value();

  const prach_configuration prach_cfg =
      prach_configuration_get(frequency_range::FR1,
                              msg.tdd_ul_dl_cfg_common.has_value() ? duplex_mode::TDD : duplex_mode::FDD,
                              rach_cfg_cmn.rach_cfg_generic.prach_config_index);
  VERIFY(prach_cfg.format < prach_format_type::invalid, "Invalid PRACH format");

  subcarrier_spacing           pusch_scs           = msg.ul_cfg_common.init_ul_bwp.generic_params.scs;
  prach_symbols_slots_duration prach_duration_info = get_prach_duration_info(prach_cfg, pusch_scs);
  prach_subcarrier_spacing     prach_scs           = is_long_preamble(prach_cfg.format)
                                                         ? get_prach_preamble_long_info(prach_cfg.format).scs
                                                         : to_ra_subcarrier_spacing(pusch_scs);

  // Check if the PRACH preambles fall into UL slots
  if (msg.tdd_ul_dl_cfg_common.has_value()) {
    const cell_configuration cell_cfg{msg};
    // For each subframe with PRACH, check if all slots are UL.
    for (unsigned subframe_idx : prach_cfg.subframe) {
      // There are configuration for which the PRACH starts in an odd slot within the subframe
      // (for numerologies > mu(SCS 15kHz)); the addition of start_slot_pusch_scs compensate for this.
      const unsigned start_slot_idx =
          subframe_idx * (1U << to_numerology_value(pusch_scs)) + prach_duration_info.start_slot_pusch_scs;
      for (unsigned sl = 0; sl < prach_duration_info.prach_length_slots; ++sl) {
        VERIFY(cell_cfg.is_fully_ul_enabled(slot_point{to_numerology_value(pusch_scs), sl + start_slot_idx}),
               "PRACH configuration index {} not supported with current TDD pattern. PRACH fall outside UL slots",
               rach_cfg_cmn.rach_cfg_generic.prach_config_index);
      }
    }
  }

  const unsigned prach_nof_prbs = prach_frequency_mapping_get(prach_scs, pusch_scs).nof_rb_ra;
  for (unsigned id_fd_ra = 0; id_fd_ra != rach_cfg_cmn.rach_cfg_generic.msg1_fdm; ++id_fd_ra) {
    uint8_t      prb_start  = rach_cfg_cmn.rach_cfg_generic.msg1_frequency_start + id_fd_ra * prach_nof_prbs;
    prb_interval prach_prbs = {prb_start, prb_start + prach_nof_prbs};

    for (const auto& pucch_region : msg.pucch_guardbands) {
      VERIFY(not pucch_region.prbs.overlaps(prach_prbs),
             "Configured PRACH occasion collides with PUCCH RBs ({} intersects {})",
             pucch_region.prbs,
             prach_prbs);
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
  static const unsigned          nof_layers = 1;
  static pdsch_mcs_table         mcs_table  = srsran::pdsch_mcs_table::qam64;
  static const unsigned          nof_oh_prb = 0;
  static const ofdm_symbol_range sib1_symbols{2, 14};

  dmrs_information dmrs_info =
      make_dmrs_info_common(msg.dl_cfg_common.init_dl_bwp.pdsch_common, 0, msg.pci, msg.dmrs_typeA_pos);
  sch_mcs_description mcs_descr     = pdsch_mcs_get_config(mcs_table, expert_cfg.si.sib1_mcs_index);
  sch_prbs_tbs        sib1_prbs_tbs = get_nof_prbs(prbs_calculator_sch_config{msg.sib1_payload_size,
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

/// \brief Validates \c sched_cell_configuration_request_message used to add a cell.
/// \param[in] msg scheduler cell configuration message to be validated.
/// \return In case an invalid parameter is detected, returns a string containing an error message.
error_type<std::string> srsran::config_validators::validate_sched_cell_configuration_request_message(
    const sched_cell_configuration_request_message& msg,
    const scheduler_expert_config&                  expert_cfg)
{
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

  HANDLE_CODE(validate_rach_cfg_common(msg));

  HANDLE_CODE(validate_pucch_cfg_common(msg));

  HANDLE_CODE(validate_sib1_cfg(msg, expert_cfg));

  // TODO: Validate other parameters.
  return {};
}
