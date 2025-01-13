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

#include "rrc_packed_test_messages.h"
#include "lib/rrc/ue/rrc_measurement_types_asn1_converters.h"
#include "srsran/asn1/rrc_nr/ho_prep_info.h"
#include "srsran/asn1/rrc_nr/meas_timing_cfg.h"

using namespace srsran;

byte_buffer srsran::create_ho_prep_info()
{
  ho_prep_info_s ho_prep;
  ho_prep.crit_exts.set_c1().set_ho_prep_info();

  // pack.
  byte_buffer   pdu;
  asn1::bit_ref bref{pdu};
  if (ho_prep.pack(bref) == asn1::SRSASN_SUCCESS) {
    return pdu;
  }
  return byte_buffer{};
}

byte_buffer srsran::create_meas_timing_cfg(uint32_t carrier_freq, subcarrier_spacing scs)
{
  asn1::rrc_nr::meas_timing_cfg_s asn1_meas_timing_cfg;
  auto&                           meas_timing_conf = asn1_meas_timing_cfg.crit_exts.set_c1().set_meas_timing_conf();
  asn1::rrc_nr::meas_timing_s     meas_timing_item;
  meas_timing_item.freq_and_timing_present                = true;
  meas_timing_item.freq_and_timing.carrier_freq           = carrier_freq;
  meas_timing_item.freq_and_timing.ssb_subcarrier_spacing = srs_cu_cp::subcarrier_spacing_to_rrc_asn1(scs);
  meas_timing_item.freq_and_timing.ssb_meas_timing_cfg.periodicity_and_offset.set_sf10() = 0;
  meas_timing_item.freq_and_timing.ssb_meas_timing_cfg.dur = asn1::rrc_nr::ssb_mtc_s::dur_opts::sf5;

  meas_timing_conf.meas_timing.push_back(meas_timing_item);

  // pack.
  byte_buffer   pdu;
  asn1::bit_ref bref{pdu};
  if (asn1_meas_timing_cfg.pack(bref) == asn1::SRSASN_SUCCESS) {
    return pdu;
  }
  return byte_buffer{};
}
