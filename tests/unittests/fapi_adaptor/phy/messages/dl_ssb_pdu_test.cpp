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

#include "srsran/fapi/message_builders.h"
#include "srsran/fapi_adaptor/phy/messages/ssb.h"
#include "srsran/ran/ssb/pbch_mib_pack.h"
#include "srsran/srsvec/bit.h"
#include "srsran/srsvec/compare.h"
#include <chrono>
#include <gtest/gtest.h>
#include <random>

using namespace srsran;
using namespace fapi_adaptor;

static std::mt19937 gen(0);

static uint32_t generate_bch_payload(unsigned            ssb_subcarrier_offset,
                                     dmrs_typeA_position dmrs_type_a_position,
                                     uint8_t             pdcch_config_sib1,
                                     bool                cell_barred,
                                     bool                intra_freq_reselection,
                                     uint32_t            sfn,
                                     bool                hrf,
                                     subcarrier_spacing  scs_common)
{
  // Prepare message parameters.
  pbch_mib_message msg = {.sfn                   = sfn,
                          .hrf                   = hrf,
                          .scs_common            = scs_common,
                          .subcarrier_offset     = ssb_subcarrier_offset,
                          .dmrs_typeA_pos        = dmrs_type_a_position,
                          .pdcch_config_sib1     = pdcch_config_sib1,
                          .cell_barred           = cell_barred,
                          .intrafreq_reselection = intra_freq_reselection,
                          .ssb_block_index       = 0};

  // Generate payload.
  return pbch_mib_pack(msg);
}

TEST(fapi_to_phy_ssb_conversion_test, valid_pdu_conversion_success)
{
  // Random generators.
  std::uniform_int_distribution<unsigned> sfn_dist(0, 1023);
  std::uniform_int_distribution<unsigned> slot_dist(0, 160);
  std::uniform_int_distribution<unsigned> pci_dist(0, 3000);
  std::uniform_int_distribution<uint8_t>  eight_bit_dist(0, UINT8_MAX);
  std::uniform_int_distribution<unsigned> binary_dist(0, 1);
  std::uniform_int_distribution<unsigned> dmrs_type_A_dist(2, 3);
  std::uniform_int_distribution<unsigned> subcarrier_offset_dist(0, 23);
  std::uniform_int_distribution<unsigned> offset_pointA_dist(0, 2199);
  std::uniform_int_distribution<unsigned> sib1_dist(0, 255);
  std::uniform_real_distribution<float>   power_scaling_ss_pbch_dist(-110, 120);
  std::uniform_real_distribution<float>   pss_to_sss_ratio_dist(-32.767, 32.767);

  for (unsigned lmax : {4u, 8u, 64u}) {
    for (unsigned ssb_idx = 0; ssb_idx != lmax; ++ssb_idx) {
      for (subcarrier_spacing scs : {subcarrier_spacing::kHz15,
                                     subcarrier_spacing::kHz30,
                                     subcarrier_spacing::kHz120,
                                     subcarrier_spacing::kHz240}) {
        for (subcarrier_spacing common_scs : {subcarrier_spacing::kHz15,
                                              subcarrier_spacing::kHz30,
                                              subcarrier_spacing::kHz60,
                                              subcarrier_spacing::kHz120}) {
          for (fapi::beta_pss_profile_type beta_pss :
               {fapi::beta_pss_profile_type::dB_0, fapi::beta_pss_profile_type::dB_3}) {
            for (ssb_pattern_case pattern_case : {ssb_pattern_case::A,
                                                  ssb_pattern_case::B,
                                                  ssb_pattern_case::C,
                                                  ssb_pattern_case::D,
                                                  ssb_pattern_case::E}) {
              slot_point slot(
                  static_cast<uint32_t>(scs), sfn_dist(gen), slot_dist(gen) % (10 << static_cast<uint32_t>(scs)));

              unsigned            sfn                    = slot.sfn();
              unsigned            pci                    = pci_dist(gen);
              unsigned            ssb_subcarrier_offset  = subcarrier_offset_dist(gen);
              unsigned            offset_pointA          = offset_pointA_dist(gen);
              dmrs_typeA_position dmrs_type_a_position   = static_cast<dmrs_typeA_position>(dmrs_type_A_dist(gen));
              uint8_t             pdcch_config_sib1      = eight_bit_dist(gen);
              bool                cell_barred            = binary_dist(gen);
              bool                intra_freq_reselection = binary_dist(gen);

              fapi::dl_tti_request_message         msg;
              fapi::dl_tti_request_message_builder builder(msg);
              builder.set_basic_parameters(slot.sfn(), slot.slot_index(), 0);
              auto     ssb_builder = builder.add_ssb_pdu(pci, beta_pss, ssb_idx, ssb_subcarrier_offset, offset_pointA);
              uint32_t mib_payload = generate_bch_payload(ssb_subcarrier_offset,
                                                          dmrs_type_a_position,
                                                          pdcch_config_sib1,
                                                          cell_barred,
                                                          intra_freq_reselection,
                                                          sfn,
                                                          slot.is_odd_hrf(),
                                                          slot.scs()) >>
                                     8;
              ssb_builder.set_bch_payload_phy_timing_info(mib_payload);
              ssb_builder.set_maintenance_v3_basic_parameters(pattern_case, scs, lmax);

              // PHY processor PDU.
              ssb_processor::pdu_t pdu;

              // Conversion block.
              convert_ssb_fapi_to_phy(pdu, msg.pdus[0].ssb_pdu, msg.sfn, msg.slot, common_scs);

              // Assert contents.
              ASSERT_EQ(pdu.slot.sfn(), sfn);
              ASSERT_EQ(pdu.slot.slot_index(), slot.slot_index());
              ASSERT_EQ(pdu.phys_cell_id, pci);
              switch (beta_pss) {
                case fapi::beta_pss_profile_type::dB_0:
                  ASSERT_EQ(pdu.beta_pss, 0.0);
                  break;
                case fapi::beta_pss_profile_type::dB_3:
                  ASSERT_EQ(pdu.beta_pss, 3.0);
                  break;
                case fapi::beta_pss_profile_type::beta_pss_profile_sss:
                  break;
              }
              ASSERT_EQ(pdu.ssb_idx, ssb_idx);
              ASSERT_EQ(pdu.L_max, lmax);
              ASSERT_EQ(pdu.subcarrier_offset.to_uint(), ssb_subcarrier_offset);
              ASSERT_EQ(pdu.offset_to_pointA.to_uint(), offset_pointA);
              ASSERT_EQ(pdu.pattern_case, pattern_case);
              ASSERT_TRUE(srsvec::equal(pdu.ports, std::vector<uint8_t>{0}));

              std::array<uint8_t, ssb_processor::MIB_PAYLOAD_SIZE> dest;
              srsvec::bit_unpack(dest, mib_payload, dest.size());
              ASSERT_EQ(pdu.mib_payload, dest);
            }
          }
        }
      }
    }
  }
}
