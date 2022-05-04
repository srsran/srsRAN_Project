#include "helpers.h"
#include "srsgnb/fapi/message_validators.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;
using namespace fapi;
using namespace unittest;

/// Tests that a valid DL TTI request message validates correctly.
static void test_dl_tti_request_ok()
{
  dl_tti_request_message msg = build_valid_dl_tti_request();

  const auto& result = validate_dl_tti_request(msg);

  TESTASSERT(result);
}

/// Tests that a DL TTI request message which contains errors in some properties fails to validate.
static void test_dl_tti_request_error()
{
  dl_tti_request_message msg = build_valid_dl_tti_request();

  // Set some errors.
  msg.sfn                              = 2000;
  msg.slot                             = 1000;
  msg.pdus.back().ssb_pdu.phys_cell_id = 2000;

  const auto& result = validate_dl_tti_request(msg);

  TESTASSERT(!result);

  const auto& report = result.error();
  // Check that the 3 errors are reported.
  TESTASSERT_EQ(report.reports.size(), 3u);
}

int main()
{
  test_dl_tti_request_ok();
  test_dl_tti_request_error();
}
