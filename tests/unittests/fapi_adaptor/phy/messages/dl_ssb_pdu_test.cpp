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

#include "srsran/fapi/message_builders.h"
#include "srsran/fapi_adaptor/phy/messages/ssb.h"
#include "srsran/srsvec/compare.h"
#include "srsran/support/test_utils.h"
#include <chrono>
#include <random>

using namespace srsran;
using namespace fapi_adaptor;

static std::mt19937 gen(0);

/// Benchmark that measures the performance converting SSB data structures from MAC -> FAPI -> PHY.
static void ssb_conversion_test()
{
  // Random generators.
  std::uniform_int_distribution<unsigned> sfn_dist(0, 1023);
  std::uniform_int_distribution<unsigned> slot_dist(0, 160);
  std::uniform_int_distribution<unsigned> pci_dist(0, 3000);
  std::uniform_int_distribution<uint8_t>  binary_1bit_dist(0, 1);
  std::uniform_int_distribution<uint8_t>  binary_8bit_dist(0, UINT8_MAX);
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
          for (fapi::beta_pss_profile_type beta_pss : {fapi::beta_pss_profile_type::dB_0,
                                                       fapi::beta_pss_profile_type::dB_3,
                                                       fapi::beta_pss_profile_type::beta_pss_profile_sss}) {
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
              uint8_t             pdcch_config_sib1      = binary_8bit_dist(gen);
              bool                cell_barred            = binary_dist(gen);
              bool                intra_freq_reselection = binary_dist(gen);

              // :TODO: Begin with the MAC structure when it is defined.
              fapi::dl_tti_request_message         msg = {};
              fapi::dl_tti_request_message_builder builder(msg);
              // :TODO: when the groups are available, add them.
              builder.set_basic_parameters(slot.sfn(), slot.slot_index(), 0);
              auto ssb_builder = builder.add_ssb_pdu(pci, beta_pss, ssb_idx, ssb_subcarrier_offset, offset_pointA);

              ssb_builder.set_bch_payload_phy_full(
                  dmrs_type_a_position, pdcch_config_sib1, cell_barred, intra_freq_reselection);
              ssb_builder.set_maintenance_v3_basic_parameters(pattern_case, scs, lmax);

              float power_scaling_ss_pbch_dB = power_scaling_ss_pbch_dist(gen);
              float pss_to_sss_ratio_dB      = pss_to_sss_ratio_dist(gen);
              ssb_builder.set_maintenance_v3_tx_power_info(power_scaling_ss_pbch_dB, pss_to_sss_ratio_dB);

              // PHY processor PDU.
              ssb_processor::pdu_t pdu;

              // Conversion block.
              convert_ssb_fapi_to_phy(pdu, msg.pdus[0].ssb_pdu, msg.sfn, msg.slot, common_scs);

              // Assert contents.
              TESTASSERT_EQ(pdu.slot.sfn(), sfn);
              TESTASSERT_EQ(pdu.slot.slot_index(), slot.slot_index());
              TESTASSERT_EQ(pdu.phys_cell_id, pci);
              switch (beta_pss) {
                case fapi::beta_pss_profile_type::dB_0:
                  TESTASSERT_EQ(pdu.beta_pss, 0.0);
                  break;
                case fapi::beta_pss_profile_type::dB_3:
                  TESTASSERT_EQ(pdu.beta_pss, 3.0);
                  break;
                case fapi::beta_pss_profile_type::beta_pss_profile_sss:
                  TESTASSERT(std::abs(pdu.beta_pss - pss_to_sss_ratio_dB) < 0.01,
                             "Beta PSS is {} but expected {}.",
                             pdu.beta_pss,
                             pss_to_sss_ratio_dB);
                  break;
              }
              TESTASSERT_EQ(pdu.ssb_idx, ssb_idx);
              TESTASSERT_EQ(pdu.L_max, lmax);
              TESTASSERT_EQ(pdu.subcarrier_offset.to_uint(), ssb_subcarrier_offset);
              TESTASSERT_EQ(pdu.offset_to_pointA.to_uint(), offset_pointA);
              TESTASSERT_EQ(pdu.pattern_case, pattern_case);
              TESTASSERT(srsvec::equal(pdu.ports, std::vector<uint8_t>{0}));

              // MIB - 1 bit
              TESTASSERT_EQ(pdu.bch_payload[0], 0);
              // systemFrameNumber - 6 bits MSB
              TESTASSERT_EQ(pdu.bch_payload[1], (sfn >> 9U) & 1U);
              TESTASSERT_EQ(pdu.bch_payload[2], (sfn >> 8U) & 1U);
              TESTASSERT_EQ(pdu.bch_payload[3], (sfn >> 7U) & 1U);
              TESTASSERT_EQ(pdu.bch_payload[4], (sfn >> 6U) & 1U);
              TESTASSERT_EQ(pdu.bch_payload[5], (sfn >> 5U) & 1U);
              TESTASSERT_EQ(pdu.bch_payload[6], (sfn >> 4U) & 1U);
              // subCarrierSpacingCommon - 1 bit
              TESTASSERT_EQ(pdu.bch_payload[7],
                            (common_scs == subcarrier_spacing::kHz15 || common_scs == subcarrier_spacing::kHz60) ? 0
                                                                                                                 : 1);
              // ssb-SubcarrierOffset - 4 bits
              TESTASSERT_EQ(pdu.bch_payload[8], (ssb_subcarrier_offset >> 3U) & 1U);
              TESTASSERT_EQ(pdu.bch_payload[9], (ssb_subcarrier_offset >> 2U) & 1U);
              TESTASSERT_EQ(pdu.bch_payload[10], (ssb_subcarrier_offset >> 1U) & 1U);
              TESTASSERT_EQ(pdu.bch_payload[11], (ssb_subcarrier_offset >> 0U) & 1U);

              // dmrs-TypeA-Position - 1 bit
              TESTASSERT_EQ(pdu.bch_payload[12],
                            static_cast<unsigned>(dmrs_type_a_position == dmrs_typeA_position::pos2) ? 0 : 1);

              // pdcch-ConfigSIB1
              // controlResourceSetZero - 4 bits
              TESTASSERT_EQ(pdu.bch_payload[13], (pdcch_config_sib1 >> 7U) & 1U);
              TESTASSERT_EQ(pdu.bch_payload[14], (pdcch_config_sib1 >> 6U) & 1U);
              TESTASSERT_EQ(pdu.bch_payload[15], (pdcch_config_sib1 >> 5U) & 1U);
              TESTASSERT_EQ(pdu.bch_payload[16], (pdcch_config_sib1 >> 4U) & 1U);

              // searchSpaceZero - 4 bits
              TESTASSERT_EQ(pdu.bch_payload[17], (pdcch_config_sib1 >> 3U) & 1U);
              TESTASSERT_EQ(pdu.bch_payload[18], (pdcch_config_sib1 >> 2U) & 1U);
              TESTASSERT_EQ(pdu.bch_payload[19], (pdcch_config_sib1 >> 1U) & 1U);
              TESTASSERT_EQ(pdu.bch_payload[20], (pdcch_config_sib1 >> 0U) & 1U);

              // Barred - 1 bit
              TESTASSERT_EQ(pdu.bch_payload[21], cell_barred ? 0U : 1U);

              // intraFreqReselection - 1 bit
              TESTASSERT_EQ(pdu.bch_payload[22], intra_freq_reselection ? 0U : 1U);

              // Spare - 1 bit
              TESTASSERT_EQ(pdu.bch_payload[23], 0);

              // systemFrameNumber - 4 bits LSB
              TESTASSERT_EQ(pdu.bch_payload[24], (sfn >> 3U) & 1U);
              TESTASSERT_EQ(pdu.bch_payload[25], (sfn >> 2U) & 1U);
              TESTASSERT_EQ(pdu.bch_payload[26], (sfn >> 1U) & 1U);
              TESTASSERT_EQ(pdu.bch_payload[27], (sfn >> 0U) & 1U);

              // Half radio frame - 1 bit
              TESTASSERT_EQ(pdu.bch_payload[28], slot.is_odd_hrf());

              if (lmax == 64) {
                // SS/PBCH block index - 3 MSB
                TESTASSERT_EQ(pdu.bch_payload[29], (ssb_idx >> 5U) & 1U);
                TESTASSERT_EQ(pdu.bch_payload[30], (ssb_idx >> 4U) & 1U);
                TESTASSERT_EQ(pdu.bch_payload[31], (ssb_idx >> 3U) & 1U);
              } else {
                // 3rd LSB set to MSB of SSB subcarrier offset. 2nd and 1st bits reserved.
                TESTASSERT_EQ(pdu.bch_payload[29], (ssb_subcarrier_offset >> 5U) & 1U);
                TESTASSERT_EQ(pdu.bch_payload[30], 0);
                TESTASSERT_EQ(pdu.bch_payload[31], 0);
              }
            }
          }
        }
      }
    }
  }
}

int main()
{
  ssb_conversion_test();

  fmt::print("Success\n");
  return 0;
}
