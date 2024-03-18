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

#include "srsran/fapi_adaptor/phy/messages/ssb.h"
#include "srsran/srsvec/bit.h"

using namespace srsran;
using namespace fapi_adaptor;

/// \brief Adds the PHY timing information to the BCH payload for the given \c fapi_pdu.
/// \return A packed BCH payload with the timing information, as per TS38.212 Section 7.1.1.
static uint32_t fill_phy_timing_info_in_bch_payload(const fapi::dl_ssb_pdu& fapi_pdu, uint32_t sfn, bool hrf)
{
  // Move the BCH payload to the MSB.
  uint32_t payload = fapi_pdu.bch_payload.bch_payload << 8U;

  // Add the SFN.
  payload |= ((sfn & 0xfU) << 4U);

  // Add the half radio frame bit.
  payload |= (hrf << 3U);

  if (fapi_pdu.ssb_maintenance_v3.L_max == 64) {
    // Pack the 6th, 5th and 4th bits of SS/PBCH block index.
    payload |= ((fapi_pdu.ssb_block_index >> 3U) & 7U);
  } else {
    // 3rd LSB set to MSB of the SSB subcarrier offset. 2nd and 1st bits are reserved.
    payload |= (((fapi_pdu.ssb_subcarrier_offset >> 7U) & 1U) << 2U);
  }

  return payload;
}

/// \brief Encodes the full BCH payload for the given \c fapi_pdu.
/// \return A packed BCH payload, as per TS38.212 Section 7.1.1.
static uint32_t
generate_bch_payload(const fapi::dl_ssb_pdu& fapi_pdu, uint32_t sfn, bool hrf, subcarrier_spacing scs_common)
{
  const fapi::dl_ssb_phy_mib_pdu& mib     = fapi_pdu.bch_payload.phy_mib_pdu;
  uint32_t                        payload = 0;

  // CHOICE in BCCH-BCH-MessageType - 1 bit.
  // Set to zero.

  // Pack MIB.

  // systemFrameNumber - 6 bits MSB.
  payload |= (((sfn >> 4U) & 0x3fU) << 25U);

  // subCarrierSpacingCommon - 1 bit.
  bool scs_flag = (scs_common == subcarrier_spacing::kHz30 || scs_common == subcarrier_spacing::kHz120);
  payload |= (scs_flag << 24U);

  // ssb-SubcarrierOffset - 4 bits.
  payload |= ((fapi_pdu.ssb_subcarrier_offset & 0xfU) << 20U);

  // dmrs-TypeA-Position - 1 bit.
  payload |= ((static_cast<std::underlying_type_t<fapi::dmrs_typeA_pos>>(mib.dmrs_typeA_position) & 0x1U) << 19U);

  // pdcch-ConfigSIB1 - 8 bits.
  payload |= ((mib.pdcch_config_sib1 & 0xffU) << 11U);

  // Barred - 1 bit.
  payload |= ((mib.cell_barred ? 0U : 1U) << 10U);

  // intraFreqReselection - 1 bit.
  payload |= ((mib.intrafreq_reselection ? 0U : 1U) << 9U);

  // Spare - 1 bit.
  // Leave to zero.

  // Add the SFN. - 4 bit LSB.
  payload |= ((sfn & 0xfU) << 4U);

  // Half radio frame bit.
  payload |= (hrf << 3U);

  if (fapi_pdu.ssb_maintenance_v3.L_max == 64) {
    // Pack the 6th, 5th and 4th bits of SS/PBCH block index.
    payload |= ((fapi_pdu.ssb_block_index >> 3U) & 7U);
  } else {
    // 3rd LSB set to MSB of the SSB subcarrier offset. 2nd and 1st bits are reserved.
    payload |= (((fapi_pdu.ssb_subcarrier_offset >> 7U) & 1U) << 2U);
  }

  return payload;
}

/// \brief Fills the contents of the BCH payload.
///
/// The BCH packing procedure is selected based on the field \c bch_payload_type of \c fapi_pdu, as per TS38.212
/// Section 7.1.1 and references therein.
static void fill_bch_payload(span<uint8_t>           dest,
                             const fapi::dl_ssb_pdu& fapi_pdu,
                             uint16_t                sfn,
                             bool                    hrf,
                             subcarrier_spacing      scs_common)
{
  uint32_t payload = 0;
  switch (fapi_pdu.bch_payload_flag) {
    case fapi::bch_payload_type::mac_full:
      payload = fapi_pdu.bch_payload.bch_payload;
      break;
    case fapi::bch_payload_type::phy_timing_info:
      payload = fill_phy_timing_info_in_bch_payload(fapi_pdu, sfn, hrf);
      break;
    case fapi::bch_payload_type::phy_full:
      payload = generate_bch_payload(fapi_pdu, sfn, hrf, scs_common);
      break;
    default:
      srsran_assert(0, "Invalid BCH payload flag");
      break;
  }
  srsvec::bit_unpack(dest, payload, dest.size());
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

  // Fill in the BCH payload.
  fill_bch_payload(proc_pdu.bch_payload, fapi_pdu, sfn, proc_pdu.slot.is_odd_hrf(), scs_common);

  // :TODO: Implement the ports array when beamforming is added.
  proc_pdu.ports = {0};
}
