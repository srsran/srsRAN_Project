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
#include "srsgnb/f1_interface/common/f1c_common.h"
#include "srsgnb/ran/nr_cgi.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

/// Test PLMN decoding
void test_ngi_converter()
{
  test_delimit_logger delimiter{"Test NGI converter"};

  // use known a PLMN
  asn1::f1ap::nrcgi_s asn1_cgi;
  asn1_cgi.plmn_id.from_string("02f899"); // 208.99

  // convert to internal NGI representation
  nr_cell_global_identity ngi = cgi_from_asn1(asn1_cgi);

  TESTASSERT_EQ(0xf208, ngi.mcc);   // BCD-encoded MCC
  TESTASSERT_EQ(0xff99, ngi.mnc);   // BCD-encoded MNC
  TESTASSERT_EQ("20899", ngi.plmn); // human-readable PLMN
}

int main()
{
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);

  srslog::init();

  test_ngi_converter();
}