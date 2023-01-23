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

  const prach_preamble_information info = get_prach_preamble_long_info(prach_cfg.format);

  const unsigned nof_td_occasions = prach_cfg.format.is_long_preamble() ? 1 : prach_cfg.nof_occasions_within_slot;
  const unsigned prach_nof_prbs =
      prach_frequency_mapping_get(info.scs, msg.ul_cfg_common.init_ul_bwp.generic_params.scs).nof_rb_ra;
  for (unsigned i = 0; i != nof_td_occasions; ++i) {
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

/// \brief Validates \c sched_cell_configuration_request_message used to add a cell.
/// \param[in] msg scheduler cell configuration message to be validated.
/// \return In case an invalid parameter is detected, returns a string containing an error message.
error_type<std::string> srsgnb::config_validators::validate_sched_cell_configuration_request_message(
    const sched_cell_configuration_request_message& msg)
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

  // TODO: Validate other parameters.
  return {};
}
