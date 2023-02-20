/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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