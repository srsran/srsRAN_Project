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

#include "srsran/fapi_adaptor/phy/messages/csi_rs.h"
#include "srsran/ran/csi_rs/csi_rs_config_helpers.h"
#include "srsran/ran/csi_rs/frequency_allocation_type.h"
#include "srsran/ran/precoding/precoding_codebooks.h"
#include "srsran/srsvec/bit.h"

using namespace srsran;
using namespace fapi_adaptor;

/// Translates the \c nzp_csi_rs_epre_to_ssb enum to a linear amplitude value.
static float translate_amplitude(fapi::nzp_csi_rs_epre_to_ssb power)
{
  switch (power) {
    case fapi::nzp_csi_rs_epre_to_ssb::dB_minus_3:
      return 0.5F;
    case fapi::nzp_csi_rs_epre_to_ssb::dB0:
      return 1.F;
    case fapi::nzp_csi_rs_epre_to_ssb::dB3:
      return 2.F;
    case fapi::nzp_csi_rs_epre_to_ssb::dB6:
      return 4.F;
    case fapi::nzp_csi_rs_epre_to_ssb::L1_use_profile_sss:
    default:
      return 1.F;
  }
}

void srsran::fapi_adaptor::convert_csi_rs_fapi_to_phy(nzp_csi_rs_generator::config_t& proc_pdu,
                                                      const fapi::dl_csi_rs_pdu&      fapi_pdu,
                                                      uint16_t                        sfn,
                                                      uint16_t                        slot,
                                                      uint16_t                        cell_bandwidth_prb)
{
  proc_pdu.slot = slot_point(fapi_pdu.scs, sfn, slot);
  proc_pdu.cp   = fapi_pdu.cp;

  proc_pdu.start_rb = fapi_pdu.start_rb;
  proc_pdu.nof_rb   = std::min(fapi_pdu.num_rbs, static_cast<uint16_t>(cell_bandwidth_prb - fapi_pdu.start_rb));
  proc_pdu.csi_rs_mapping_table_row = fapi_pdu.row;
  csi_rs::convert_freq_domain(proc_pdu.freq_allocation_ref_idx, fapi_pdu.freq_domain, fapi_pdu.row);

  proc_pdu.symbol_l0     = fapi_pdu.symb_L0;
  proc_pdu.symbol_l1     = fapi_pdu.symb_L1;
  proc_pdu.cdm           = fapi_pdu.cdm_type;
  proc_pdu.freq_density  = fapi_pdu.freq_density;
  proc_pdu.scrambling_id = fapi_pdu.scramb_id;

  proc_pdu.amplitude = translate_amplitude(fapi_pdu.power_control_offset_ss_profile_nr);

  unsigned nof_ports = csi_rs::get_nof_csi_rs_ports(fapi_pdu.row);
  proc_pdu.precoding = precoding_configuration::make_wideband(make_identity(nof_ports));
}

void srsran::fapi_adaptor::get_csi_rs_pattern_from_fapi_pdu(csi_rs_pattern&            pattern,
                                                            const fapi::dl_csi_rs_pdu& fapi_pdu,
                                                            uint16_t                   cell_bandwidth_prb)
{
  // Fill the CSI-RS pattern configuration.
  csi_rs_pattern_configuration config;

  config.start_rb = fapi_pdu.start_rb;
  config.nof_rb   = std::min(fapi_pdu.num_rbs, static_cast<uint16_t>(cell_bandwidth_prb - fapi_pdu.start_rb));
  config.csi_rs_mapping_table_row = fapi_pdu.row;
  csi_rs::convert_freq_domain(config.freq_allocation_ref_idx, fapi_pdu.freq_domain, fapi_pdu.row);

  config.symbol_l0    = fapi_pdu.symb_L0;
  config.symbol_l1    = fapi_pdu.symb_L1;
  config.cdm          = fapi_pdu.cdm_type;
  config.freq_density = fapi_pdu.freq_density;

  // Get the CSI-RS pattern.
  pattern = get_csi_rs_pattern(config);
}
