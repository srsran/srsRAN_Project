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

#include "srsran/fapi_adaptor/mac/messages/ssb.h"

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

void srsran::fapi_adaptor::convert_ssb_mac_to_fapi(fapi::dl_ssb_pdu& fapi_pdu, const srsran::dl_ssb_pdu& mac_pdu)
{
  fapi::dl_ssb_pdu_builder builder(fapi_pdu);

  convert_ssb_mac_to_fapi(builder, mac_pdu);
}

void srsran::fapi_adaptor::convert_ssb_mac_to_fapi(fapi::dl_ssb_pdu_builder& builder, const srsran::dl_ssb_pdu& mac_pdu)
{
  builder.set_basic_parameters(mac_pdu.pci,
                               convert_to_beta_pss_profile_nr(mac_pdu.pss_to_sss_epre),
                               mac_pdu.ssb_index,
                               mac_pdu.subcarrier_offset.to_uint(),
                               mac_pdu.offset_to_pointA);

  builder.set_maintenance_v3_basic_parameters(mac_pdu.ssb_case, mac_pdu.scs, mac_pdu.L_max);

  const ssb_mib_data_pdu& mib_pdu = mac_pdu.mib_data;
  builder.set_bch_payload_phy_full(
      mib_pdu.dmrs_typeA_pos, mib_pdu.pdcch_config_sib1, mib_pdu.cell_barred, mib_pdu.intra_freq_reselection);
}
