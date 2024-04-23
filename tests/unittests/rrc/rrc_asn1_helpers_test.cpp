/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
