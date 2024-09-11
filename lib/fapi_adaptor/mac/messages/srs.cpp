/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/fapi_adaptor/mac/messages/srs.h"
#include "srsran/scheduler/scheduler_slot_handler.h"

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
  static constexpr unsigned time_start_position = 0;
  builder.set_timing_params(time_start_position, mac_pdu.t_srs_period, mac_pdu.t_offset);

  // Rest of the parameters.
  builder.set_srs_params(mac_pdu.nof_antenna_ports,
                         mac_pdu.symbols.length(),
                         mac_pdu.nof_repetitions,
                         mac_pdu.config_index,
                         mac_pdu.sequence_id,
                         mac_pdu.bw_index,
                         mac_pdu.cyclic_shift,
                         mac_pdu.resource_type);
}