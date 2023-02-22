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

#include "lib/f1ap/common/asn1_helpers.h"
#include "srsran/asn1/f1ap/f1ap.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/nr_cgi_helpers.h"
#include <gtest/gtest.h>

using namespace srsran;

/// Test PLMN decoding
TEST(f1ap_asn1_helpers_test, test_ngi_converter)
{
  // use known a PLMN
  asn1::f1ap::nrcgi_s asn1_cgi;
  asn1_cgi.plmn_id.from_string("02f899"); // 208.99
  asn1_cgi.nrcell_id.from_number(0x12345678);

  // convert to internal NGI representation
  nr_cell_global_identity ngi = cgi_from_asn1(asn1_cgi);

  ASSERT_TRUE(srsran::config_helpers::is_valid(ngi));
  ASSERT_EQ(0xf208, ngi.mcc);        // BCD-encoded MCC
  ASSERT_EQ(0xff99, ngi.mnc);        // BCD-encoded MNC
  ASSERT_EQ("20899", ngi.plmn);      // human-readable PLMN
  ASSERT_EQ("02f899", ngi.plmn_hex); // hex-encoded PLMN (like above)
}