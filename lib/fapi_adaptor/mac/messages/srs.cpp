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

#include "srsran/fapi_adaptor/mac/messages/srs.h"
#include "srsran/scheduler/result/srs_info.h"

using namespace srsran;
using namespace fapi_adaptor;

void fapi_adaptor::convert_srs_mac_to_fapi(fapi::ul_srs_pdu& fapi_pdu, const srs_info& mac_pdu)
{
  fapi::ul_srs_pdu_builder builder(fapi_pdu);

  convert_srs_mac_to_fapi(builder, mac_pdu);
}

void fapi_adaptor::convert_srs_mac_to_fapi(fapi::ul_srs_pdu_builder& builder, const srs_info& mac_pdu)
{
  static constexpr unsigned handle = 0;

  // Basic parameters.
  builder.set_basic_parameters(mac_pdu.crnti, handle);

  // BWP parameters.
  const bwp_configuration& bwp = *mac_pdu.bwp_cfg;
  builder.set_bwp_parameters(bwp.crbs.length(), bwp.crbs.start(), bwp.scs, bwp.cp);

  // Frequency parameters.
  builder.set_frequency_params(
      mac_pdu.freq_position, mac_pdu.freq_shift, mac_pdu.freq_hopping, mac_pdu.group_or_seq_hopping);

  // Comb parameters.
  builder.set_comb_params(mac_pdu.tx_comb, mac_pdu.comb_offset);

  // Timing parameters.
  builder.set_timing_params(mac_pdu.symbols.start(), mac_pdu.t_srs_period, mac_pdu.t_offset);

  // Rest of the parameters.
  builder.set_srs_params(mac_pdu.nof_antenna_ports,
                         mac_pdu.symbols.length(),
                         mac_pdu.nof_repetitions,
                         mac_pdu.config_index,
                         mac_pdu.sequence_id,
                         mac_pdu.bw_index,
                         mac_pdu.cyclic_shift,
                         mac_pdu.resource_type);

  // Add the requested reports.
  builder.set_report_params(mac_pdu.normalized_channel_iq_matrix_requested, mac_pdu.positioning_report_requested);
}
