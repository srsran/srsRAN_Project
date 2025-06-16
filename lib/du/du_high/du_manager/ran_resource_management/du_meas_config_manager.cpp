/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "du_meas_config_manager.h"
#include "du_ue_resource_config.h"
#include "srsran/asn1/rrc_nr/meas_cfg.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;
using namespace srs_du;
using namespace asn1::rrc_nr;

static bool unpack_meas_cfg(meas_cfg_s& meas_cfg, const byte_buffer& container)
{
  asn1::cbit_ref bref{container};
  return meas_cfg.unpack(bref) == asn1::SRSASN_SUCCESS;
}

// Creates a measurement gap based on a SSB MTC config.
static meas_gap_config create_meas_gap(subcarrier_spacing scs, const ssb_mtc_s& smtc1)
{
  meas_gap_config meas_gap;

  // Determine measGap Length.
  switch (smtc1.dur.value) {
    case ssb_mtc_s::dur_opts::sf1:
      meas_gap.mgl = scs != subcarrier_spacing::kHz15 ? meas_gap_length::ms1dot5 : meas_gap_length::ms3;
      break;
    case ssb_mtc_s::dur_opts::sf2:
      meas_gap.mgl = meas_gap_length::ms3;
    case ssb_mtc_s::dur_opts::sf3:
      meas_gap.mgl = meas_gap_length::ms4;
      break;
    case ssb_mtc_s::dur_opts::sf4:
    case ssb_mtc_s::dur_opts::sf5:
      meas_gap.mgl = scs != subcarrier_spacing::kHz15 ? meas_gap_length::ms5dot5 : meas_gap_length::ms6;
      break;
    default:
      report_fatal_error("Invalid SSB MTC duration");
  }

  // Determine measGap Periodicity and Offset.
  switch (smtc1.periodicity_and_offset.type().value) {
    case ssb_mtc_s::periodicity_and_offset_c_::types_opts::sf5:
      meas_gap.offset = smtc1.periodicity_and_offset.sf5();
      meas_gap.mgrp   = meas_gap_repetition_period::ms20;
      break;
    case ssb_mtc_s::periodicity_and_offset_c_::types_opts::sf10:
      meas_gap.offset = smtc1.periodicity_and_offset.sf10();
      meas_gap.mgrp   = meas_gap_repetition_period::ms20;
      break;
    case ssb_mtc_s::periodicity_and_offset_c_::types_opts::sf20:
      meas_gap.offset = smtc1.periodicity_and_offset.sf20();
      meas_gap.mgrp   = meas_gap_repetition_period::ms20;
      break;
    case ssb_mtc_s::periodicity_and_offset_c_::types_opts::sf40:
      meas_gap.offset = smtc1.periodicity_and_offset.sf40();
      meas_gap.mgrp   = meas_gap_repetition_period::ms40;
      break;
    case ssb_mtc_s::periodicity_and_offset_c_::types_opts::sf80:
      meas_gap.offset = smtc1.periodicity_and_offset.sf80();
      meas_gap.mgrp   = meas_gap_repetition_period::ms80;
      break;
    case ssb_mtc_s::periodicity_and_offset_c_::types_opts::sf160:
      meas_gap.offset = smtc1.periodicity_and_offset.sf160();
      meas_gap.mgrp   = meas_gap_repetition_period::ms160;
      break;
    default:
      report_fatal_error("Invalid SSB MTC periodicity_and_offset");
  }

  return meas_gap;
}

du_meas_config_manager::du_meas_config_manager(span<const du_cell_config> cell_cfg_list_) :
  cell_cfg_list(cell_cfg_list_), logger(srslog::fetch_basic_logger("DU-MNG"))
{
}

void du_meas_config_manager::update(du_ue_resource_config& ue_cfg, const byte_buffer& packed_meas_cfg)
{
  if (packed_meas_cfg.empty()) {
    return;
  }

  meas_cfg_s meas_cfg;
  if (not unpack_meas_cfg(meas_cfg, packed_meas_cfg)) {
    logger.error("Failed to unpack meas config. Discarding it...");
    return;
  }

  const du_cell_config& pcell_common = cell_cfg_list[ue_cfg.cell_group.cells[0].serv_cell_cfg.cell_index];

  for (const auto& asn1measobj : meas_cfg.meas_obj_to_add_mod_list) {
    if (asn1measobj.meas_obj.type().value != meas_obj_to_add_mod_s::meas_obj_c_::types_opts::meas_obj_nr) {
      logger.warning("Ignoring measObject of type {}. Cause: Unsupported", asn1measobj.meas_obj.type().to_string());
      continue;
    }
    const auto& asn1nr = asn1measobj.meas_obj.meas_obj_nr();

    if (not asn1nr.ssb_freq_present or not asn1nr.smtc1_present) {
      logger.info("Ignoring measObject of type {}. Cause: Lack of a SSB frequency or SMTC1 config",
                  asn1measobj.meas_obj.type().to_string());
      continue;
    }

    if (asn1nr.ssb_freq == pcell_common.dl_cfg_common.freq_info_dl.absolute_frequency_ssb) {
      // Same frequency. No need for measGap.
      continue;
    }

    // Create measGap.
    ue_cfg.meas_gap = create_meas_gap(pcell_common.scs_common, asn1nr.smtc1);
  }
}
