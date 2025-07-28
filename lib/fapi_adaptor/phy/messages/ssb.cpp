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

#include "srsran/fapi_adaptor/phy/messages/ssb.h"
#include "srsran/ran/ssb/pbch_mib_pack.h"
#include "srsran/srsvec/bit.h"

using namespace srsran;
using namespace fapi_adaptor;

/// Unpacks the contents of the BCH payload.
static void unpack_bch_payload(span<uint8_t> dest, const fapi::dl_ssb_pdu& fapi_pdu)
{
  report_error_if_not(fapi_pdu.bch_payload_flag == fapi::bch_payload_type::phy_timing_info, "Invalid BCH payload flag");
  srsvec::bit_unpack(dest, fapi_pdu.bch_payload.bch_payload, dest.size());
}

/// Returns the coefficient \f$\beta_{PSS}\f$ from the given SSB PDU (see TS38.213, Section 4.1).
static float convert_to_beta_pss(const fapi::dl_ssb_pdu& fapi_pdu)
{
  switch (fapi_pdu.beta_pss_profile_nr) {
    case fapi::beta_pss_profile_type::dB_0:
      return 0.F;
    case fapi::beta_pss_profile_type::dB_3:
      return 3.F;
    default:
      // NOTE: Unreachable code as the FAPI message should have been validated.
      srsran_assert(0, "Invalid beta PSS profile");
      return 0;
  }
}

void srsran::fapi_adaptor::convert_ssb_fapi_to_phy(ssb_processor::pdu_t&   proc_pdu,
                                                   const fapi::dl_ssb_pdu& fapi_pdu,
                                                   uint16_t                sfn,
                                                   uint16_t                slot,
                                                   subcarrier_spacing      scs_common)
{
  srsran_assert(scs_common != subcarrier_spacing::kHz240, "Invalid value for common subcarrier spacing");
  srsran_assert(fapi_pdu.ssb_maintenance_v3.scs != subcarrier_spacing::kHz60,
                "Invalid value for SSB subcarrier spacing");

  proc_pdu.slot              = slot_point(fapi_pdu.ssb_maintenance_v3.scs, sfn, slot);
  proc_pdu.phys_cell_id      = fapi_pdu.phys_cell_id;
  proc_pdu.beta_pss          = convert_to_beta_pss(fapi_pdu);
  proc_pdu.ssb_idx           = fapi_pdu.ssb_block_index;
  proc_pdu.L_max             = fapi_pdu.ssb_maintenance_v3.L_max;
  proc_pdu.subcarrier_offset = fapi_pdu.ssb_subcarrier_offset;
  proc_pdu.offset_to_pointA  = fapi_pdu.ssb_offset_pointA;
  proc_pdu.pattern_case      = fapi_pdu.ssb_maintenance_v3.case_type;
  proc_pdu.common_scs        = scs_common;

  unpack_bch_payload(proc_pdu.mib_payload, fapi_pdu);

  // Use only a single port for SSB.
  proc_pdu.ports = {0};
}
