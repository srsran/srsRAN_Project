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

#include "lib/rrc/ue/rrc_asn1_converters.h"
#include "srsran/asn1/asn1_utils.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include <cstdint>
#include <gtest/gtest.h>

using namespace srsran;
using namespace srsran::srs_cu_cp;

/// Test five-g-s-tmsi conversion
TEST(rrc_asn1_helpers_test, test_five_g_s_tmsi_converter_for_valid_five_g_s_tmsi)
{
  // use known a Five-G-S-TMSI
  asn1::fixed_bitstring<48> asn1_five_g_s_tmsi;
  asn1_five_g_s_tmsi.from_number(278099133963U);

  srs_cu_cp::cu_cp_five_g_s_tmsi five_g_s_tmsi = asn1_to_five_g_s_tmsi(asn1_five_g_s_tmsi);

  ASSERT_EQ(1U, five_g_s_tmsi.get_amf_set_id());
  ASSERT_EQ(0U, five_g_s_tmsi.get_amf_pointer());
  ASSERT_EQ(3221227019U, five_g_s_tmsi.get_five_g_tmsi());
}

/// Test five-g-s-tmsi conversion with concatenation
TEST(rrc_asn1_helpers_test, test_five_g_s_tmsi_concatenation_for_valid_five_g_s_tmsi)
{
  // use known Five-G-S-TMSI-Par1 and Five-G-S-TMSI-Part2
  asn1::fixed_bitstring<39> asn1_five_g_s_tmsi_part1;
  asn1_five_g_s_tmsi_part1.from_number(278099133963);

  asn1::fixed_bitstring<9> asn1_five_g_s_tmsi_part_2;
  asn1_five_g_s_tmsi_part_2.from_number(0);

  srs_cu_cp::cu_cp_five_g_s_tmsi five_g_s_tmsi =
      asn1_to_five_g_s_tmsi(asn1_five_g_s_tmsi_part1, asn1_five_g_s_tmsi_part_2);

  ASSERT_EQ(1U, five_g_s_tmsi.get_amf_set_id());
  ASSERT_EQ(0U, five_g_s_tmsi.get_amf_pointer());
  ASSERT_EQ(3221227019U, five_g_s_tmsi.get_five_g_tmsi());
}

/// Test amf-identifier decoding
TEST(rrc_asn1_helpers_test, test_amf_identifier_converter_for_valid_amf_id)
{
  // use known a amf-identifier
  asn1::rrc_nr::registered_amf_s registered_amf;
  registered_amf.amf_id.from_number(0xf511b2);

  srs_cu_cp::cu_cp_amf_identifier_t amf_id = asn1_to_amf_identifier(registered_amf.amf_id);

  ASSERT_EQ(245U, amf_id.amf_region_id);
  ASSERT_EQ(70U, amf_id.amf_set_id);
  ASSERT_EQ(50U, amf_id.amf_pointer);
}
