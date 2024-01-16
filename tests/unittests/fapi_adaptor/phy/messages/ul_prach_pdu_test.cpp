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

#include "../../../fapi/validators/helpers.h"
#include "srsran/fapi_adaptor/phy/messages/prach.h"
#include "srsran/phy/support/prach_buffer_context.h"
#include <gtest/gtest.h>
#include <numeric>

using namespace srsran;
using namespace fapi_adaptor;
using namespace unittest;

TEST(FapiPhyUlPrachPduAdaptorTest, valid_pdu_pass)
{
  fapi::ul_prach_pdu fapi_pdu = build_valid_ul_prach_pdu();

  // As it's only one PRACH config TLV with one Fd occassion, modify the values from the PRACH FAPI PDU.
  fapi_pdu.index_fd_ra                           = 0;
  fapi_pdu.maintenance_v3.prach_res_config_index = 0;

  // Creation of the multi-PRACH config TLV. Used random values from the range of the document SCF-222 v4.0
  // section 3.3.2.4 in table PRACH configuration table.
  fapi::prach_config prach;
  prach.prach_res_config_index = 0;
  prach.prach_sequence_length  = fapi::prach_sequence_length_type::long_sequence;
  prach.prach_scs              = prach_subcarrier_spacing::kHz15;
  prach.prach_ul_bwp_pusch_scs = subcarrier_spacing::kHz15;
  prach.restricted_set         = restricted_set_config::UNRESTRICTED;
  prach.prach_config_index     = 1;
  prach.prach_format           = prach_format_type::zero;
  prach.num_prach_td_occasions = 1;
  prach.num_preambles          = 1;
  prach.ssb_per_rach           = 1;
  prach.start_preamble_index   = 1;

  prach.fd_occasions.emplace_back();
  fapi::prach_fd_occasion_config& ocass = prach.fd_occasions.back();
  ocass.prach_root_sequence_index       = 430;
  ocass.num_root_sequences              = 2;
  ocass.prach_freq_offset               = 9;
  ocass.prach_guardband_offset          = 21;
  ocass.prach_zero_corr_conf            = 3;
  ocass.unused_root_sequences.emplace_back(3);
  unsigned             sfn     = 1;
  unsigned             slot_id = 2;
  unsigned             sector  = 0;
  slot_point           slot(to_numerology_value(prach.prach_ul_bwp_pusch_scs), sfn, slot_id);
  fapi::carrier_config carrier_cfg;
  carrier_cfg.ul_grid_size = {25, 50, 100, 150, 170};
  carrier_cfg.num_rx_ant   = 4;

  std::vector<uint8_t> prach_rx_ports(carrier_cfg.num_rx_ant);
  std::iota(prach_rx_ports.begin(), prach_rx_ports.end(), 0);

  prach_buffer_context context;
  convert_prach_fapi_to_phy(context, fapi_pdu, prach, carrier_cfg, prach_rx_ports, sfn, slot_id, sector);

  ASSERT_EQ(static_cast<unsigned>(fapi_pdu.prach_format), static_cast<unsigned>(context.format));
  ASSERT_EQ(fapi_pdu.prach_start_symbol, context.start_symbol);
  ASSERT_EQ(fapi_pdu.num_prach_ocas, context.nof_td_occasions);
  const fapi::ul_prach_maintenance_v3& v3 = fapi_pdu.maintenance_v3;
  ASSERT_EQ(v3.num_fd_ra, context.nof_fd_occasions);
  ASSERT_EQ(v3.start_preamble_index, context.start_preamble_index);
  ASSERT_EQ(v3.num_preamble_indices, context.nof_preamble_indices);
  ASSERT_EQ(static_cast<unsigned>(prach.restricted_set), static_cast<unsigned>(context.restricted_set));
  ASSERT_EQ(static_cast<unsigned>(prach.prach_ul_bwp_pusch_scs), static_cast<unsigned>(context.pusch_scs));
  ASSERT_EQ(ocass.prach_root_sequence_index, context.root_sequence_index);
  ASSERT_EQ(ocass.prach_freq_offset, context.rb_offset);
  ASSERT_EQ(ocass.prach_zero_corr_conf, context.zero_correlation_zone);
  ASSERT_EQ(slot, context.slot);
  ASSERT_EQ(sector, context.sector);
  ASSERT_EQ(span<const uint8_t>(prach_rx_ports), span<const uint8_t>(context.ports));
  ASSERT_EQ(carrier_cfg.ul_grid_size[to_numerology_value(prach.prach_ul_bwp_pusch_scs)], context.nof_prb_ul_grid);
}
