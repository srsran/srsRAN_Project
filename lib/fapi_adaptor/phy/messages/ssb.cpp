/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/fapi_adaptor/phy/messages/ssb.h"
#include "srsgnb/srsvec/bit.h"

using namespace srsgnb;
using namespace fapi;
using namespace fapi_adaptor;

/// \brief Fills PHY timing information for the given \c fapi_pdu.
/// \return Packed BCH payload with the timing information filled, as per TS38.212 Section 7.1.1.
static uint32_t fill_phy_timing_info_in_bch_payload(const dl_ssb_pdu& fapi_pdu, uint32_t sfn, uint32_t hrf)
{
  // Move the BCH payload to the MSB.
  uint32_t payload = fapi_pdu.bch_payload.bch_payload << 8u;

  // Add the sfn.
  payload |= ((sfn & 0xf) << 4u);

  // Half radio frame bit.
  payload |= ((hrf & 0x1) << 3u);

  if (fapi_pdu.ssb_maintenance_v3.lmax == 64) {
    // Use the 6th, 5th and 4th bits of SS/PBCH block index.
    payload |= ((fapi_pdu.ssb_block_index >> 3u) & 7u);
  } else {
    // 3rd LSB set to MSB of SSB subcarrier offset. 2nd and 1st bits reserved.
    payload |= (((fapi_pdu.ssb_subcarrier_offset >> 7u) & 1u) << 2u);
  }

  return payload;
}

/// \brief Fills PHY full information for the given \c fapi_pdu.
/// \return Packed BCH payload, as per TS38.212 Section 7.1.1.
static uint32_t generate_bch_payload(const dl_ssb_pdu& fapi_pdu, uint32_t sfn, uint32_t hrf)
{
  const dl_ssb_phy_mib_pdu& mib     = fapi_pdu.bch_payload.phy_mib_pdu;
  uint32_t                  payload = 0;

  // MIB - 1 bit.
  // Leave to zero.

  // systemFrameNumber - 6 bits MSB.
  payload |= (((sfn >> 4u) & 0x3f) << 25u);

  // subCarrierSpacingCommon - 1 bit.
  uint32_t scs_flag = (fapi_pdu.ssb_maintenance_v3.scs == subcarrier_spacing::kHz15 ||
                       fapi_pdu.ssb_maintenance_v3.scs == subcarrier_spacing::kHz60)
                          ? 0
                          : 1;
  payload |= ((scs_flag & 0x1) << 24u);

  // ssb-SubcarrierOffset - 4 bits.
  payload |= ((fapi_pdu.ssb_subcarrier_offset & 0xf) << 20u);

  // dmrs-TypeA-Position - 1 bit.
  payload |= ((mib.dmrs_typeA_position & 0x1) << 19u);

  // pdcch-ConfigSIB1 - 8 bits.
  payload |= ((mib.pdcch_config_sib1 & 0xff) << 11u);

  // Barred - 1 bit.
  payload |= ((mib.cell_barred ? 0U : 1U) << 10u);

  // intraFreqReselection - 1 bit.
  payload |= ((mib.intrafreq_reselection ? 0U : 1U) << 9u);

  // Spare - 1 bit.
  // Leave to zero.

  // Add the sfn. - 4 bit LSB.
  payload |= ((sfn & 0xf) << 4u);

  // Half radio frame bit.
  payload |= ((hrf & 0x1) << 3u);

  if (fapi_pdu.ssb_maintenance_v3.lmax == 64) {
    // Use the 6th, 5th and 4th bits of SS/PBCH block index.
    payload |= ((fapi_pdu.ssb_block_index >> 3u) & 7u);
  } else {
    // 3rd LSB set to MSB of SSB subcarrier offset. 2nd and 1st bits reserved.
    payload |= (((fapi_pdu.ssb_subcarrier_offset >> 7u) & 1u) << 2u);
  }

  return payload;
}

/// Fills the BCH payload.
static void fill_bch_payload(span<uint8_t> dest, const dl_ssb_pdu& fapi_pdu, unsigned sfn, unsigned hrf)
{
  uint32_t payload = 0;
  switch (fapi_pdu.bch_payload_flag) {
    case bch_payload_type::mac_full:
      payload = fapi_pdu.bch_payload.bch_payload;
      break;
    case bch_payload_type::phy_timing_info:
      payload = fill_phy_timing_info_in_bch_payload(fapi_pdu, sfn, hrf);
      break;
    case bch_payload_type::phy_full:
      payload = generate_bch_payload(fapi_pdu, sfn, hrf);
      break;
    default:
      srsran_assert(0, "Invalid BCH payload flag");
      break;
  }
  srsvec::bit_unpack(dest, payload, dest.size());
}

/// Returns the beta pss from the given SSB pdu.
static float get_beta_pss(const dl_ssb_pdu& fapi_pdu)
{
  switch (fapi_pdu.beta_pss_profile_nr) {
    case beta_pss_profile_type::dB_0:
      return 0.f;
    case beta_pss_profile_type::dB_3:
      return 3.f;
    case beta_pss_profile_type::beta_pss_profile_sss:
      return fapi_pdu.ssb_maintenance_v3.beta_pss_profile_sss * 0.001f;
    default:
      // NOTE: We ensure that it never reaches here, as the fapi message should be validated.
      srsran_assert(0, "Invalid beta PSS profile");
      return 0;
  }
}

void srsgnb::fapi_adaptor::convert_ssb_fapi_to_phy(ssb_processor::pdu_t& proc_pdu,
                                                   const dl_ssb_pdu&     fapi_pdu,
                                                   uint16_t              sfn,
                                                   uint16_t              slot)
{
  proc_pdu.slot              = slot_point(static_cast<uint32_t>(fapi_pdu.ssb_maintenance_v3.scs), sfn, slot);
  proc_pdu.phys_cell_id      = fapi_pdu.phys_cell_id;
  proc_pdu.beta_pss          = get_beta_pss(fapi_pdu);
  proc_pdu.ssb_idx           = fapi_pdu.ssb_block_index;
  proc_pdu.L_max             = fapi_pdu.ssb_maintenance_v3.lmax;
  proc_pdu.subcarrier_offset = fapi_pdu.ssb_subcarrier_offset;
  proc_pdu.offset_to_pointA  = fapi_pdu.ssb_offset_pointA;
  proc_pdu.pattern_case      = fapi_pdu.ssb_maintenance_v3.case_type;

  // Calculate half radio frame.
  unsigned hrf = proc_pdu.slot.is_odd_hrf() ? 1 : 0;

  // Get the BCH payload.
  fill_bch_payload(proc_pdu.bch_payload, fapi_pdu, sfn, hrf);

  // :TODO: Implement the ports array when the beamforming is added.
  proc_pdu.ports = {0};
}
