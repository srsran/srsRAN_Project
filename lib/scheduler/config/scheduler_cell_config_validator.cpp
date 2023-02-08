/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/scheduler/config/scheduler_cell_config_validator.h"
#include "../support/dmrs_helpers.h"
#include "../support/prbs_calculator.h"
#include "srsgnb/ran/duplex_mode.h"
#include "srsgnb/ran/prach/prach_configuration.h"
#include "srsgnb/ran/prach/prach_frequency_mapping.h"
#include "srsgnb/ran/prach/prach_preamble_information.h"
#include "srsgnb/scheduler/sched_consts.h"

using namespace srsgnb;
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

static error_type<std::string> validate_rach_cfg_common(const sched_cell_configuration_request_message& msg)
{
  VERIFY(msg.ul_cfg_common.init_ul_bwp.rach_cfg_common.has_value(),
         "Cells without RACH-ConfigCommon are not supported");
  const rach_config_common& rach_cfg_cmn = msg.ul_cfg_common.init_ul_bwp.rach_cfg_common.value();

  const prach_configuration prach_cfg =
      prach_configuration_get(frequency_range::FR1,
                              msg.tdd_ul_dl_cfg_common.has_value() ? duplex_mode::TDD : duplex_mode::FDD,
                              rach_cfg_cmn.rach_cfg_generic.prach_config_index);
  VERIFY(prach_cfg.format.is_long_preamble(), "Short PRACH preamble formats not supported");
  VERIFY(prach_cfg.duration > 0, "Invalid PRACH duration {}", prach_cfg.duration);

  const prach_preamble_information info = get_prach_preamble_long_info(prach_cfg.format);

  const unsigned nof_td_occasions = prach_cfg.format.is_long_preamble() ? 1 : prach_cfg.nof_occasions_within_slot;
  const unsigned prach_nof_prbs =
      prach_frequency_mapping_get(info.scs, msg.ul_cfg_common.init_ul_bwp.generic_params.scs).nof_rb_ra;
  for (unsigned i = 0; i != nof_td_occasions; ++i) {
    unsigned          start_symbol = prach_cfg.starting_symbol + i * prach_cfg.duration;
    ofdm_symbol_range symbols      = {start_symbol, start_symbol + prach_cfg.duration};
    ofdm_symbol_range valid_symbol_range{0, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP};
    VERIFY(valid_symbol_range.contains(symbols),
           "PRACH config not supported. PRACH symbols {} fall outside slot boundary",
           symbols);

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
  static pdsch_mcs_table         mcs_table  = srsgnb::pdsch_mcs_table::qam64;
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

  VERIFY(
      sib1_prbs_tbs.nof_prbs <= msg.dl_cfg_common.init_dl_bwp.generic_params.crbs.length(),
      "Not enough initial DL BWP PRBs ({} > {}) to send SIB1, given the chosen MCS={} and SIB1 payload size={} bytes. "
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
error_type<std::string> srsgnb::config_validators::validate_sched_cell_configuration_request_message(
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

  HANDLE_CODE(validate_rach_cfg_common(msg));

  HANDLE_CODE(validate_pucch_cfg_common(msg));

  HANDLE_CODE(validate_sib1_cfg(msg, expert_cfg));

  // TODO: Validate other parameters.
  return {};
}
