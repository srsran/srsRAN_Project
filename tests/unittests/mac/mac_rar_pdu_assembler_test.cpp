/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "lib/mac/mac_dl/rar_pdu_assembler.h"
#include "mac_test_helpers.h"
#include "srsran/adt/circular_array.h"
#include "srsran/support/bit_encoding.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>
#include <random>

using namespace srsran;

std::random_device                      rd;
std::mt19937                            gen(rd());
std::uniform_int_distribution<unsigned> rnti_dist(MIN_CRNTI, MAX_CRNTI);
std::uniform_int_distribution<unsigned> rapid_dist(0, 63);
std::uniform_int_distribution<unsigned> mcs_dist(0, 15);
std::uniform_int_distribution<unsigned> time_res_dist(0, 15);
std::uniform_int_distribution<unsigned> freq_res_dist(0, 16383);
std::uniform_int_distribution<unsigned> ta_dist(0, 63);
std::uniform_int_distribution<unsigned> freq_hop_dist(0, 1);
std::uniform_int_distribution<uint8_t>  tpc_dist(0, 7);
std::uniform_int_distribution<unsigned> nof_ul_grants_per_rar(1, MAX_RAR_PDUS_PER_SLOT - 1);

// Check TS38.321 6.2.2 and 6.2.3.
static const unsigned RAR_PDU_SIZE = 8;

rar_ul_grant make_random_ul_grant()
{
  rar_ul_grant grant{};
  grant.freq_hop_flag            = freq_hop_dist(gen) > 0;
  grant.rapid                    = rapid_dist(gen);
  grant.temp_crnti               = to_rnti(rnti_dist(gen));
  grant.mcs                      = mcs_dist(gen);
  grant.freq_resource_assignment = freq_res_dist(gen);
  grant.time_resource_assignment = time_res_dist(gen);
  grant.tpc                      = tpc_dist(gen);
  grant.csi_req                  = freq_hop_dist(gen) > 0;
  grant.ta                       = ta_dist(gen);
  return grant;
}

rar_information make_random_rar_info(unsigned nof_ul_grants = 1, unsigned padding_bytes = 0)
{
  rar_information rar{};
  rar.pdsch_cfg.codewords.resize(1);
  rar.pdsch_cfg.codewords[0].tb_size_bytes = nof_ul_grants * RAR_PDU_SIZE + padding_bytes;
  rar.grants.resize(nof_ul_grants);
  for (unsigned i = 0; i < nof_ul_grants; ++i) {
    rar.grants[i] = make_random_ul_grant();
  }
  return rar;
}

/// \brief Checks whether it is the last subPDU of a MAC RAR PDU. The Extension field "E" should flag this information
/// according to TS38.321 6.2.2.
static bool is_last_subpdu(span<const uint8_t> rar_pdu)
{
  return (rar_pdu[0] & (1U << 7U)) == 0;
}

/// \brief Checks whether the MAC RAR PDU contains a RAPID field as per TS38.321 6.2.2.
static bool is_rapid_subpdu(span<const uint8_t> rar_pdu)
{
  return (rar_pdu[0] & (1U << 6U)) > 0;
}

/// Decode RAR UL PDU as per TS 38.321, Section 6.2.2 and 6.2.3.
rar_ul_grant decode_ul_grant(span<const uint8_t> rar_subpdu)
{
  byte_buffer  buf = rar_subpdu;
  bit_decoder  dec(buf);
  rar_ul_grant ret{};

  dec.advance_bits(2);
  dec.unpack(ret.rapid, 6);
  dec.advance_bits(1);
  dec.unpack(ret.ta, 7 + 5);
  dec.unpack(ret.freq_hop_flag, 1);
  dec.unpack(ret.freq_resource_assignment, 14);
  dec.unpack(ret.time_resource_assignment, 4);
  uint8_t mcs;
  dec.unpack(mcs, 4);
  ret.mcs = mcs;
  dec.unpack(ret.tpc, 3);
  dec.unpack(ret.csi_req, 1);
  uint16_t rnti;
  dec.unpack(rnti, 16);
  ret.temp_crnti = to_rnti(rnti);

  TESTASSERT_EQ(RAR_PDU_SIZE, dec.nof_bytes());

  return ret;
}

bool operator==(const rar_ul_grant& lhs, const rar_ul_grant& rhs)
{
  return lhs.ta == rhs.ta and lhs.freq_hop_flag == rhs.freq_hop_flag and
         lhs.freq_resource_assignment == rhs.freq_resource_assignment and
         lhs.time_resource_assignment == rhs.time_resource_assignment and lhs.mcs == rhs.mcs and lhs.tpc == rhs.tpc and
         lhs.csi_req == rhs.csi_req and lhs.temp_crnti == rhs.temp_crnti;
}

/// Tests if the encoded RAR PDU matches the content in the original RAR.
void test_encoded_rar(const rar_information& original_rar, span<const uint8_t> rar_pdu)
{
  TESTASSERT(not rar_pdu.empty());
  TESTASSERT_EQ(RAR_PDU_SIZE * original_rar.grants.size(), rar_pdu.size());

  for (unsigned i = 0; i < original_rar.grants.size(); ++i) {
    span<const uint8_t> subpdu = rar_pdu.subspan(i * RAR_PDU_SIZE, RAR_PDU_SIZE);
    TESTASSERT_EQ(is_last_subpdu(subpdu), (i == original_rar.grants.size() - 1), "for index={}", i);
    TESTASSERT(is_rapid_subpdu(subpdu));

    rar_ul_grant grant2 = decode_ul_grant(subpdu);
    TESTASSERT(original_rar.grants[i] == grant2);
  }
}

TEST(rar_assembler_test, multiple_random_ul_grants)
{
  constexpr static size_t MAX_RAR_GRANT_SIZE = 64;
  test_delimit_logger     test_delim{"MAC assembler for multiple UL grants"};

  rar_information          rar_info = make_random_rar_info(nof_ul_grants_per_rar(gen));
  ticking_ring_buffer_pool pdu_pool(MAX_DL_PDUS_PER_SLOT * MAX_GRANTS_PER_RAR * MAX_RAR_GRANT_SIZE, 1, 10240);

  rar_pdu_assembler   assembler(pdu_pool);
  span<const uint8_t> bytes = assembler.encode_rar_pdu(rar_info);

  test_encoded_rar(rar_info, bytes);
}

/// In this test, we verify that the RAR PDU assembler is able to store past RAR PDUs for a short period of time,
/// so that the output PDUs can be referenced by lower layers without risking dangling pointers.
TEST(rar_assembler_test, rar_assembler_maintains_old_results)
{
  constexpr static size_t MAX_RAR_GRANT_SIZE = 64;

  test_delimit_logger test_delim{"MAC assembler maintains previous results"};

  ticking_ring_buffer_pool pdu_pool(
      MAX_DL_PDUS_PER_SLOT * MAX_GRANTS_PER_RAR * MAX_RAR_GRANT_SIZE, NOF_SUBFRAMES_PER_FRAME, 10240);
  rar_pdu_assembler assembler(pdu_pool);

  // The RAR assembler has to internally store previous slot results. This variable defines a reasonable slot duration
  // that the RAR assembler has to keep these results stored.
  static constexpr unsigned MEMORY_RESULT_IN_SLOTS = MAX_RAR_PDUS_PER_SLOT * NOF_SUBFRAMES_PER_FRAME;

  circular_array<span<const uint8_t>, MEMORY_RESULT_IN_SLOTS> previous_pdus;
  circular_array<rar_information, MEMORY_RESULT_IN_SLOTS>     previous_rars;

  unsigned nof_slots_tests = MEMORY_RESULT_IN_SLOTS * 64;
  for (unsigned i = 0; i < nof_slots_tests; ++i) {
    pdu_pool.tick(i);
    if (i >= MEMORY_RESULT_IN_SLOTS) {
      // Test old results to check if they are still valid.
      test_encoded_rar(previous_rars[i], previous_pdus[i]);
    }
    previous_rars[i] = make_random_rar_info(nof_ul_grants_per_rar(gen));
    previous_pdus[i] = assembler.encode_rar_pdu(previous_rars[i]);
  }
}
