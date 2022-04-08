#include "../../../lib/fapi/validators/dl_ssb_pdu.h"
#include "helpers.h"
#include "srsgnb/fapi/validators.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;
using namespace fapi;
using namespace unittest;

/// Tests that a valid SSB pdu validates correctly.
static void test_dl_ssb_pdu_ok()
{
  dl_ssb_pdu pdu = build_valid_dl_ssb_pdu();

  validator_report report;
  TESTASSERT(validate_dl_ssb_pdu(pdu, report));
  // Assert no reports were generated.
  TESTASSERT(report.reports.empty());
}

/// Tests that a SSB pdu that contains errors fails to validate.
static void test_dl_ssb_pdu_error()
{
  dl_ssb_pdu pdu = build_valid_dl_ssb_pdu();

  // Force 3 errors.
  pdu.phys_cell_id                                   = 2000;
  pdu.ssb_block_index                                = 100;
  pdu.ssb_maintenance_v3.ss_pbch_block_power_scaling = -12000;

  validator_report report;
  TESTASSERT(!validate_dl_ssb_pdu(pdu, report));
  // Assert 3 reports were generated.
  TESTASSERT_EQ(report.reports.size(), 3u);
}

int main()
{
  test_dl_ssb_pdu_ok();
  test_dl_ssb_pdu_error();
}
