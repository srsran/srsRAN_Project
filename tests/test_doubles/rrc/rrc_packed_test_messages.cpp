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
#include "rrc_test_messages.h"
#include "srsran/asn1/rrc_nr/meas_timing_cfg.h"

using namespace srsran;

byte_buffer srsran::test_helpers::create_meas_timing_cfg(uint32_t carrier_freq, subcarrier_spacing scs)
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

byte_buffer srsran::test_helpers::create_packed_sib1()
{
  asn1::rrc_nr::sib1_s sib1 = create_sib1();

  byte_buffer   pdu;
  asn1::bit_ref bref{pdu};
  if (sib1.pack(bref) == asn1::SRSASN_SUCCESS) {
    return pdu;
  }
  return byte_buffer{};
}

std::string srsran::test_helpers::create_sib1_hex_string()
{
  return asn1::octet_string_helper::to_hex_string(create_packed_sib1());
}

byte_buffer srsran::test_helpers::create_cell_group_config()
{
  return make_byte_buffer(
             "5c00b001117aec701061e0007c0204683c080004120985950001ff000000000306e10840003c02ca00418000001034c0809285000"
             "71c4800089aabe420e0008206102860e4e2450b183470f204400002140002002d40004322140006322d4000801214000a012d4000"
             "c31214000e312d4001004214001206410050190481606414060190581a06418070190681e0641c080b104022074148125c95250c0"
             "a1c47456789bc9a074820f177e060870000000e3ff5988d5355ca0580000865d8000000000000000000308ad54500470010020820"
             "00e21005c400e0204108001c4200b8401c080841000388401708038181042000710802e18070402104000e21005c3000800000082"
             "18081018201c160001c71000000080100020180020240088029800008c40089c7001800")
      .value();
}
