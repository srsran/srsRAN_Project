/*
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/fapi/message_builders.h"
#include "srsgnb/fapi_adaptor/phy/messages/pdcch.h"
#include "srsgnb/srsvec/bit.h"
#include "srsgnb/srsvec/compare.h"
#include "srsgnb/support/test_utils.h"
#include <random>

using namespace srsgnb;
using namespace fapi;
using namespace fapi_adaptor;

static std::mt19937 gen(0);

static void pdcch_conversion_test()
{
  // Random generators.
  std::uniform_int_distribution<unsigned> sfn_dist(0, 1023);
  std::uniform_int_distribution<unsigned> slot_dist(0, 159);
  std::uniform_int_distribution<unsigned> bwp_size_dist(1, 275);
  std::uniform_int_distribution<unsigned> bwp_start_dist(0, 274);
  std::uniform_int_distribution<unsigned> start_symbol_index_dist(0, 13);
  std::uniform_int_distribution<unsigned> duration_symbol_dist(0, 3);
  std::uniform_int_distribution<unsigned> shift_index_dist(0, 275);
  std::uniform_int_distribution<unsigned> n_rnti_dist(0, 65535);
  std::uniform_int_distribution<unsigned> cce_dist(0, 135);
  std::uniform_int_distribution<unsigned> aggregation_dist(0, 4);
  std::uniform_int_distribution<unsigned> nid_dmrs_dist(0, 65535);
  std::uniform_int_distribution<unsigned> nid_data_dist(0, 65535);
  std::uniform_real_distribution<float>   power_dmrs_dist(-32.767, 32.767);

  for (auto cyclic_p : {cyclic_prefix_type::normal, cyclic_prefix_type::extended}) {
    for (auto cce_reg_mapping : {cce_to_reg_mapping_type::non_interleaved, cce_to_reg_mapping_type::interleaved}) {
      for (auto reg_bundle : {2U, 3U, 6U}) {
        for (auto interleaver_size : {2U, 3U, 6U}) {
          for (auto type : {pdcch_coreset_type::pbch_or_sib1, pdcch_coreset_type::other}) {
            for (auto precoder :
                 {precoder_granularity_type::same_as_reg_bundle, precoder_granularity_type::all_contiguous_rbs}) {
              for (int power_nr = -9; power_nr != -7; ++power_nr) {
                for (int power = -33; power != 3; power += 3) {
                  unsigned sfn                = sfn_dist(gen);
                  unsigned slot               = slot_dist(gen);
                  unsigned bwp_size           = bwp_size_dist(gen);
                  unsigned bwp_start          = bwp_start_dist(gen);
                  unsigned start_symbol_index = start_symbol_index_dist(gen);
                  unsigned duration_symbol    = duration_symbol_dist(gen);
                  unsigned shift_index        = shift_index_dist(gen);
                  unsigned n_rnti             = n_rnti_dist(gen);
                  unsigned cce                = cce_dist(gen);
                  unsigned aggregation        = static_cast<unsigned>(std::pow(2, aggregation_dist(gen)));
                  unsigned nid_dmrs           = nid_dmrs_dist(gen);
                  unsigned nid_data           = nid_data_dist(gen);
                  float    power_dmrs         = power_dmrs_dist(gen);

                  dl_pdcch_pdu         pdu;
                  dl_pdcch_pdu_builder builder(pdu);

                  static_vector<uint8_t, 6> freq_domain = {3, 2, 1, 4, 5, 1};

                  // Always work with the biggest numerology.
                  builder.set_bwp_parameters(bwp_size, bwp_start, subcarrier_spacing::kHz240, cyclic_p);
                  builder.set_coreset_parameters(start_symbol_index,
                                                 duration_symbol,
                                                 {freq_domain},
                                                 cce_reg_mapping,
                                                 reg_bundle,
                                                 interleaver_size,
                                                 type,
                                                 shift_index,
                                                 precoder);

                  // Add DCI.
                  auto builder_dci = builder.add_dl_dci();

                  builder_dci.set_basic_parameters(to_rnti(0), nid_data, n_rnti, cce, aggregation);
                  optional<float> profile_nr;
                  optional<float> profile_dmrs;
                  if (power_nr != -9) {
                    profile_nr.emplace(power_nr);
                  } else {
                    profile_dmrs.emplace(power_dmrs);
                  }

                  builder_dci.set_tx_power_info_parameter(profile_nr);

                  // Payload.
                  static_vector<uint8_t, 128> payload = {1, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1};
                  static_vector<uint8_t, 128> packed_payload;
                  packed_payload.resize(std::ceil(payload.size() / 8.F));
                  srsvec::bit_pack({packed_payload}, {payload});

                  builder_dci.set_payload({packed_payload});

                  optional<float> profile_data;
                  if (power != -33) {
                    profile_data.emplace(power);
                  }

                  builder_dci.set_maintenance_v3_dci_parameters(true, profile_dmrs, profile_data);
                  builder_dci.set_parameters_v4_dci(nid_dmrs);

                  pdcch_processor::pdu_t proc_pdu;
                  convert_pdcch_fapi_to_phy(proc_pdu, pdu, sfn, slot);

                  // Test basic parameters.
                  TESTASSERT_EQ(sfn, proc_pdu.slot.sfn());
                  TESTASSERT_EQ(slot, proc_pdu.slot.slot_index());
                  TESTASSERT_EQ(static_cast<cyclic_prefix::options>(cyclic_p), proc_pdu.cp.value);

                  // Test coreset parameters.
                  TESTASSERT_EQ(bwp_size, proc_pdu.coreset.bwp_size_rb);
                  TESTASSERT_EQ(bwp_start, proc_pdu.coreset.bwp_start_rb);
                  TESTASSERT_EQ(start_symbol_index, proc_pdu.coreset.start_symbol_index);
                  TESTASSERT_EQ(duration_symbol, proc_pdu.coreset.duration);
                  TESTASSERT_EQ(shift_index, proc_pdu.coreset.shift_index);
                  TESTASSERT_EQ(static_cast<unsigned>(cce_reg_mapping),
                                static_cast<unsigned>(proc_pdu.coreset.cce_to_reg_mapping_type));
                  TESTASSERT_EQ(reg_bundle, proc_pdu.coreset.reg_bundle_size);
                  TESTASSERT_EQ(interleaver_size, proc_pdu.coreset.interleaver_size);
                  TESTASSERT_EQ(static_cast<unsigned>(type), static_cast<unsigned>(proc_pdu.coreset.type));
                  TESTASSERT_EQ(static_cast<unsigned>(precoder),
                                static_cast<unsigned>(proc_pdu.coreset.precoder_granularity));

                  // Test DCIs.
                  TESTASSERT_EQ(1, proc_pdu.dci_list.size());
                  TESTASSERT_EQ(n_rnti, proc_pdu.dci_list[0].n_rnti);
                  TESTASSERT_EQ(cce, proc_pdu.dci_list[0].cce_index);
                  TESTASSERT_EQ(aggregation, proc_pdu.dci_list[0].aggregation_level);
                  TESTASSERT_EQ(nid_data, proc_pdu.dci_list[0].n_id_pdcch_data);
                  TESTASSERT_EQ(nid_dmrs, proc_pdu.dci_list[0].n_id_pdcch_dmrs);

                  // Test powers.
                  TESTASSERT(std::fabs((profile_nr ? profile_nr.value() : profile_dmrs.value()) -
                                       proc_pdu.dci_list[0].dmrs_power_offset_dB) < 0.001F);
                  TESTASSERT(std::fabs((profile_data ? profile_data.value()
                                        : profile_nr ? profile_nr.value()
                                                     : profile_dmrs.value()) -
                                       proc_pdu.dci_list[0].data_power_offset_dB) < 0.001F);

                  // Test vectors.
                  TESTASSERT(payload == proc_pdu.dci_list[0].payload);

                  // Test frequency domain resources.
                  for (unsigned i = 0, e = 45; i != e; ++i) {
                    unsigned byte = i / 8;
                    unsigned pos  = i % 8;
                    TESTASSERT_EQ((static_cast<unsigned>(freq_domain[byte] >> pos) & 1U) == 1U,
                                  proc_pdu.coreset.frequency_resources[i]);
                  }
                }
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
  pdcch_conversion_test();
  fmt::print("PDCCH FAPI to PHY message conversion OK\n");
}
