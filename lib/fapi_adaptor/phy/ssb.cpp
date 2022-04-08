/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/fapi_adaptor/phy/ssb.h"
#include "srsgnb/srsvec/bit.h"

using namespace srsgnb;
using namespace fapi;
using namespace fapi_adaptor;

/// Fill PHY timing information for the given fapi_pdu. Returns a BCH payload with the timing information filled, as per
/// TS38.212 Section 7.1.1.
static uint32_t fill_phy_timing_info_in_bch_payload(const dl_ssb_pdu& fapi_pdu, uint32_t sfn)
{
  // Move the BCH payload to the MSB.
  uint32_t payload = fapi_pdu.bch_payload.bch_payload << 8u;

  // Add the sfn.
  payload |= ((sfn & 0xF) << 4u);

  // Half radio frame bit.
  // :TODO: Add bit.

  if (fapi_pdu.ssb_maintenance_v3.lmax == 64) {
    // Use the 6th, 5th and 4th bits of SS/PBCH block index.
    payload |= ((fapi_pdu.ssb_block_index >> 3u) & 7u);
  } else {
    // 3rd LSB set to MSB of SSB subcarrier offset. 2nd and 1st bits reserved.
    payload |= (((fapi_pdu.ssb_subcarrier_offset >> 7u) & 1u) << 2u);
  }

  return payload;
}

/// Fills the bch payload.
static void fill_bch_payload(span<uint8_t> dest, const dl_ssb_pdu& fapi_pdu, unsigned sfn)
{
  switch (fapi_pdu.bch_payload_flag) {
    case bch_payload_type::mac_full:
      return srsvec::bit_unpack(fapi_pdu.bch_payload.bch_payload, dest, dest.size());
    case bch_payload_type::phy_timing_info: {
      uint32_t payload = fill_phy_timing_info_in_bch_payload(fapi_pdu, sfn);
      return srsvec::bit_unpack(payload, dest, dest.size());
    }
    case bch_payload_type::phy_full:
      // :TODO: phy generates full PBCH payload. When the PHY function is added, call that function here to generate the
      // BCH payload.
      break;
    default:
      srsran_assert(0, "Invalid BCH payload flag");
      break;
  }
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
                                                   uint16_t              slot,
                                                   subcarrier_spacing    numerology)
{
  proc_pdu.slot                  = slot_point(static_cast<uint32_t>(numerology), sfn, slot);
  proc_pdu.phys_cell_id          = fapi_pdu.phys_cell_id;
  proc_pdu.beta_pss              = get_beta_pss(fapi_pdu);
  proc_pdu.ssb_idx               = fapi_pdu.ssb_block_index;
  proc_pdu.L_max                 = fapi_pdu.ssb_maintenance_v3.lmax;
  proc_pdu.ssb_subcarrier_offset = fapi_pdu.ssb_subcarrier_offset;
  proc_pdu.ssb_offset_pointA     = fapi_pdu.ssb_offset_pointA;
  proc_pdu.pattern_case          = fapi_pdu.ssb_maintenance_v3.case_type;

  // Get the BCH payload.
  fill_bch_payload({proc_pdu.bch_payload.begin(), proc_pdu.bch_payload.end()}, fapi_pdu, sfn);

  // :TODO: Implement the ports array when the beamforming is added.
}
