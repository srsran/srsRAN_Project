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
#include "srsran/cu_cp/cu_cp_types.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srsran::srs_cu_cp;

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
