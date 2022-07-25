/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../../fapi/validators/helpers.h"
#include "srsgnb/fapi_adaptor/phy/messages/prach.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace fapi;
using namespace fapi_adaptor;
using namespace unittest;

TEST(FAPI_PHY_UL_PRACH_PDU_ADAPTOR_TEST, valid_pdu_pass)
{
  ul_prach_pdu fapi_pdu = build_valid_ul_prach_pdu();

  // As it's only one PRACH config TLV with one Fd occassion, modify the values from the PRACH FAPI PDU.
  fapi_pdu.index_fd_ra                           = 0;
  fapi_pdu.maintenance_v3.prach_res_config_index = 0;

  // Creation of the multi-PRACH config TLV. Used random values from the range of the document SCF-222 v4.0
  // section 3.3.2.4 in table Multi-PRACH configuration table.
  multi_prach_config config;
  config.prachs.emplace_back();
  prach_config& prach            = config.prachs.back();
  prach.prach_res_config_index   = 0;
  prach.prach_sequence_length    = 1;
  prach.prach_subcarrier_spacing = 2;
  prach.prach_ul_bwp_pusch_scs   = subcarrier_spacing::kHz15;
  prach.restricted_set           = restricted_set_config::UNRESTRICTED;
  prach.prach_config_index       = 1;
  prach.prach_format             = prach_format_type::zero;
  prach.num_prach_td_occasions   = 1;
  prach.num_preambles            = 1;
  prach.ssb_per_rach             = 1;
  prach.start_preamble_index     = 1;
  prach.fd_occasions.emplace_back();
  prach_fd_occasion_config& ocass = prach.fd_occasions.back();
  ocass.prach_root_sequence_index = 430;
  ocass.num_root_sequences        = 2;
  ocass.prach_freq_offset         = 9;
  ocass.prach_guardband_offset    = 21;
  ocass.prach_zero_corr_conf      = 3;
  ocass.unused_root_sequences.emplace_back(3);

  prach_detector::slot_configuration slot_config;
  convert_prach_fapi_to_phy(slot_config, fapi_pdu, config);

  ASSERT_EQ(static_cast<unsigned>(fapi_pdu.prach_format), static_cast<unsigned>(slot_config.format));
  const ul_prach_maintenance_v3& v3 = fapi_pdu.maintenance_v3;
  ASSERT_EQ(v3.start_preamble_index, slot_config.start_preamble_index);
  ASSERT_EQ(v3.num_preamble_indices, slot_config.nof_preamble_indices);
  ASSERT_EQ(static_cast<unsigned>(prach.restricted_set), static_cast<unsigned>(slot_config.restricted_set));
  ASSERT_EQ(static_cast<unsigned>(prach.prach_ul_bwp_pusch_scs), static_cast<unsigned>(slot_config.pusch_scs));
  ASSERT_EQ(ocass.prach_root_sequence_index, slot_config.root_sequence_index);
  ASSERT_EQ(ocass.prach_freq_offset, slot_config.frequency_offset);
  ASSERT_EQ(ocass.prach_zero_corr_conf, slot_config.zero_correlation_zone);
}
