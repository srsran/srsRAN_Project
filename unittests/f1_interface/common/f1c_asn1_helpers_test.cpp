/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/f1_interface/common/asn1_helpers.h"
#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/ran/nr_cgi.h"
#include <gtest/gtest.h>

using namespace srsgnb;

/// Test PLMN decoding
TEST(f1c_asn1_helpers_test, test_ngi_converter)
{
  // use known a PLMN
  asn1::f1ap::nrcgi_s asn1_cgi;
  asn1_cgi.plmn_id.from_string("02f899"); // 208.99

  // convert to internal NGI representation
  nr_cell_global_identity ngi = cgi_from_asn1(asn1_cgi);

  ASSERT_EQ(0xf208, ngi.mcc);        // BCD-encoded MCC
  ASSERT_EQ(0xff99, ngi.mnc);        // BCD-encoded MNC
  ASSERT_EQ("20899", ngi.plmn);      // human-readable PLMN
  ASSERT_EQ("02f899", ngi.plmn_hex); // hex-encoded PLMN (like above)
}