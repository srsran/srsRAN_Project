/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../../lib/fapi/validators/dl_pdsch_pdu.h"
#include "helpers.h"
#include "srsgnb/fapi/message_validators.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;
using namespace fapi;
using namespace unittest;

/// Tests that a valid PDSCH PDU validates correctly.
static void test_dl_pdsch_pdu_ok()
{
  dl_pdsch_pdu pdu = build_valid_dl_pdsch_pdu();

  validator_report report(0, 0);
  TESTASSERT(validate_dl_pdsch_pdu(pdu, report));
  // Assert no reports were generated.
  TESTASSERT(report.reports.empty());
}

/// Tests that a PDSCH PDU that contains errors fails to validate.
static void test_dl_pdsch_pdu_error()
{
  dl_pdsch_pdu pdu = build_valid_dl_pdsch_pdu();

  // Force 3 errors.
  pdu.bwp_size                                                 = 2690;
  pdu.power_control_offset_profile_nr                          = 255;
  pdu.pdsch_maintenance_v3.pdsch_dmrs_power_offset_profile_sss = -32768;
  pdu.cws[0].qam_mod_order                                     = 1;

  validator_report report(0, 0);
  TESTASSERT(!validate_dl_pdsch_pdu(pdu, report));
  // Assert 3 reports were generated.
  TESTASSERT_EQ(report.reports.size(), 3u);
}

int main()
{
  test_dl_pdsch_pdu_ok();
  test_dl_pdsch_pdu_error();

  return 0;
}
