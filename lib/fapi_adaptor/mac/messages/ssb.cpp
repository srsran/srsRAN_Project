/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/fapi_adaptor/mac/messages/ssb.h"
#include "srsran/ran/ssb/pbch_mib_pack.h"

using namespace srsran;
using namespace fapi_adaptor;

/// \brief Converts the given \c ssb_pss_to_sss_epre value into a \c beta_pss_profile_type value.
///
/// This value corresponds to the \e betaPssProfileNR parameter as defined by FAPI in SCF-222 v4.0 Section 3.4.2.4.
/// \remark The MAC/Scheduler uses \c ssb_pss_to_sss_epre to prevent misusing 3GPP terminology.
static fapi::beta_pss_profile_type convert_to_beta_pss_profile_nr(ssb_pss_to_sss_epre value)
{
  switch (value) {
    case ssb_pss_to_sss_epre::dB_0:
      return fapi::beta_pss_profile_type::dB_0;
    case ssb_pss_to_sss_epre::dB_3:
      return fapi::beta_pss_profile_type::dB_3;
    default:
      break;
  }
  return fapi::beta_pss_profile_type::dB_0;
}

/// \brief Encodes the full BCH payload for the given \c fapi_pdu.
/// \return A packed BCH payload, as per TS38.212 Section 7.1.1.
static uint32_t generate_bch_payload(const dl_ssb_pdu& mac_pdu, uint32_t sfn, bool hrf, subcarrier_spacing scs_common)
{
  // Prepare message parameters.
  pbch_mib_message msg = {.sfn                   = sfn,
                          .hrf                   = hrf,
                          .scs_common            = scs_common,
                          .subcarrier_offset     = mac_pdu.subcarrier_offset,
                          .dmrs_typeA_pos        = mac_pdu.mib_data.dmrs_typeA_pos,
                          .pdcch_config_sib1     = mac_pdu.mib_data.pdcch_config_sib1,
                          .cell_barred           = mac_pdu.mib_data.cell_barred,
                          .intrafreq_reselection = mac_pdu.mib_data.intra_freq_reselection,
                          .ssb_block_index       = mac_pdu.ssb_index};

  // Generate payload.
  return pbch_mib_pack(msg);
}

void srsran::fapi_adaptor::convert_ssb_mac_to_fapi(fapi::dl_ssb_pdu& fapi_pdu,
                                                   const dl_ssb_pdu& mac_pdu,
                                                   slot_point        slot)
{
  fapi::dl_ssb_pdu_builder builder(fapi_pdu);

  convert_ssb_mac_to_fapi(builder, mac_pdu, slot);
}

void srsran::fapi_adaptor::convert_ssb_mac_to_fapi(fapi::dl_ssb_pdu_builder& builder,
                                                   const dl_ssb_pdu&         mac_pdu,
                                                   slot_point                slot)
{
  builder.set_basic_parameters(mac_pdu.pci,
                               convert_to_beta_pss_profile_nr(mac_pdu.pss_to_sss_epre),
                               mac_pdu.ssb_index,
                               mac_pdu.subcarrier_offset.to_uint(),
                               mac_pdu.offset_to_pointA);

  builder.set_maintenance_v3_basic_parameters(mac_pdu.ssb_case, mac_pdu.scs, mac_pdu.L_max);

  builder.set_bch_payload_phy_timing_info(generate_bch_payload(mac_pdu, slot.sfn(), slot.is_odd_hrf(), slot.scs()) >>
                                          8);
}
