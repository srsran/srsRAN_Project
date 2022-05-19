/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../../lib/fapi/validators/dl_pdcch_pdu.h"
#include "helpers.h"
#include "srsgnb/fapi/message_validators.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;
using namespace fapi;
using namespace unittest;

/// Tests that a valid PDCCH PDU validates correctly.
static void test_dl_pdcch_pdu_ok()
{
  dl_pdcch_pdu pdu = build_valid_dl_pdcch_pdu();

  validator_report report(0, 0);
  TESTASSERT(validate_dl_pdcch_pdu(message_type_id::ul_dci_request, pdu, report));
  // Assert no reports were generated.
  TESTASSERT(report.reports.empty());
}

/// Tests that a PDCCH PDU that contains errors fails to validate.
static void test_dl_pdcch_pdu_error()
{
  dl_pdcch_pdu pdu = build_valid_dl_pdcch_pdu();

  // Force 3 errors.
  pdu.coreset_bwp_size = 2690;
  pdu.reg_bundle_size  = 8;
  pdu.dl_dci[0].rnti   = to_rnti(0);

  validator_report report(0, 0);
  TESTASSERT(!validate_dl_pdcch_pdu(message_type_id::ul_dci_request, pdu, report));
  // Assert 3 reports were generated.
  TESTASSERT_EQ(report.reports.size(), 3u);
}

int main()
{
  test_dl_pdcch_pdu_ok();
  test_dl_pdcch_pdu_error();

  return 0;
}
