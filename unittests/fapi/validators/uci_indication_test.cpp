/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../../lib/fapi/validators/uci_pdus.h"
#include "srsgnb/fapi/message_validators.h"
#include "srsgnb/support/test_utils.h"
#include "uci_helper.h"

using namespace srsgnb;
using namespace fapi;
using namespace unittest;

static void validate_pusch_pdu_ok()
{
  auto pdu = build_valid_uci_push_pdu();

  validator_report report(0, 0);
  TESTASSERT(validate_uci_pusch_pdu(pdu, report));
  // Assert no reports were generated.
  TESTASSERT(report.reports.empty());
}

static void validate_pusch_pdu_error()
{
  auto pdu = build_valid_uci_push_pdu();

  // Add 4 errors.
  pdu.timing_advance_offset_ns = std::numeric_limits<int16_t>::max();
  pdu.rssi                     = -16801;
  pdu.harq.detection_status    = static_cast<uci_detection_status>(6);
  pdu.csi_part2.bit_length     = 1707;

  validator_report report(0, 0);
  TESTASSERT(!validate_uci_pusch_pdu(pdu, report));
  // Assert no reports were generated.
  TESTASSERT_EQ(report.reports.size(), 4u);
}

static void validate_pucch_format01_pdu_ok()
{
  auto pdu = build_valid_uci_pucch_format01_pdu();

  validator_report report(0, 0);
  TESTASSERT(validate_uci_pucch_format01_pdu(pdu, report));
  // Assert no reports were generated.
  TESTASSERT(report.reports.empty());
}

static void validate_pucch_format01_pdu_error()
{
  auto pdu = build_valid_uci_pucch_format01_pdu();

  // Add 4 errors.
  pdu.harq.harq_confidence_level = 2;
  pdu.harq.harq_values.back()    = 3;
  pdu.sr.sr_indication           = 2;
  pdu.sr.sr_confidence_level     = 2;

  validator_report report(0, 0);
  TESTASSERT(!validate_uci_pucch_format01_pdu(pdu, report));
  // Assert no reports were generated.
  TESTASSERT_EQ(report.reports.size(), 4u);
}

static void validate_pucch_format234_pdu_ok()
{
  auto pdu = build_valid_uci_pucch_format234_pdu();

  validator_report report(0, 0);
  TESTASSERT(validate_uci_pucch_format234_pdu(pdu, report));
  // Assert no reports were generated.
  TESTASSERT(report.reports.empty());
}

static void validate_pucch_format234_pdu_error()
{
  auto pdu = build_valid_uci_pucch_format234_pdu();

  // Add 4 errors.
  pdu.timing_advance_offset      = 64;
  pdu.rsrp                       = 1281;
  pdu.csi_part1.detection_status = static_cast<uci_detection_status>(6);
  pdu.sr.sr_bitlen               = 1707;

  validator_report report(0, 0);
  TESTASSERT(!validate_uci_pucch_format234_pdu(pdu, report));
  // Assert no reports were generated.
  TESTASSERT_EQ(report.reports.size(), 4u);
}

static void validate_uci_indication_ok()
{
  auto msg = build_valid_uci_indication();

  const auto& result = validate_uci_indication(msg);
  TESTASSERT(result);
}

static void validate_uci_indication_error()
{
  auto msg = build_valid_uci_indication();

  // Add 4 errors.
  msg.sfn                                               = 1024;
  msg.pdus[0].pusch_pdu.rssi                            = 1281;
  msg.pdus[1].pucch_pdu_f01.sr.sr_indication            = 3;
  msg.pdus[2].pucch_pdu_f234.csi_part1.detection_status = static_cast<uci_detection_status>(0);

  const auto& result = validate_uci_indication(msg);
  TESTASSERT(!result);
  // Assert no reports were generated.
  const auto& report = result.error();
  TESTASSERT_EQ(report.reports.size(), 4u);
}

int main()
{
  // Validate PUSCH PDU.
  validate_pusch_pdu_ok();
  validate_pusch_pdu_error();

  // Validate PUCCH format 0/1 PDU.
  validate_pucch_format01_pdu_ok();
  validate_pucch_format01_pdu_error();

  // Validate PUCCH format 2/3/4 PDU.
  validate_pucch_format234_pdu_ok();
  validate_pucch_format234_pdu_error();

  // Validate UCI.indication.
  validate_uci_indication_ok();
  validate_uci_indication_error();

  fmt::print("UCI.indication validator -> OK\n");
}
