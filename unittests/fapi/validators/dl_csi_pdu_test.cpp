/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../../lib/fapi/validators/dl_csi_pdu.h"
#include "helpers.h"
#include "srsgnb/fapi/message_validators.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;
using namespace fapi;
using namespace unittest;

/// Tests that a valid CSI-RS PDU validates correctly.
static void test_dl_csi_rs_pdu_ok()
{
  dl_csi_rs_pdu pdu = build_valid_dl_csi_pdu();

  validator_report report(0, 0);
  TESTASSERT(validate_dl_csi_pdu(pdu, report));
  // Assert no reports were generated.
  TESTASSERT(report.reports.empty());
}

/// Tests that a CSI-RS PDU that contains errors fails to validate.
static void test_dl_csi_rs_pdu_error()
{
  dl_csi_rs_pdu pdu = build_valid_dl_csi_pdu();

  // Force 3 errors.
  pdu.symb_L1                                               = 20;
  pdu.symb_L0                                               = 100;
  pdu.csi_rs_maintenance_v3.csi_rs_power_offset_profile_sss = 32123;

  validator_report report(0, 0);
  TESTASSERT(!validate_dl_csi_pdu(pdu, report));
  // Assert 3 reports were generated.
  TESTASSERT_EQ(report.reports.size(), 3u);
}

int main()
{
  test_dl_csi_rs_pdu_ok();
  test_dl_csi_rs_pdu_error();
}
