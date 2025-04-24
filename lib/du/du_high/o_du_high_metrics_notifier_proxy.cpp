/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "o_du_high_metrics_notifier_proxy.h"
#include "srsran/du/du_high/o_du_high_metrics.h"
#include "srsran/support/error_handling.h"

using namespace srsran;
using namespace srs_du;

namespace {

/// Scheduler metrics notifier dummy implementation.
class scheduler_metrics_notifier_dummy : public mac_metrics_notifier
{
public:
  // See interface for documentation.
  void on_new_metrics_report(const mac_metric_report& report) override {}
};

/// O-RAN DU high metrics notifier dummy implementation.
class o_du_high_metrics_notifier_dummy : public o_du_high_metrics_notifier
{
public:
  // See interface for documentation.
  void on_new_metrics(const o_du_high_metrics& metrics) override {}
};

} // namespace

static o_du_high_metrics_notifier_dummy dummy_odu_high_notifier;
static scheduler_metrics_notifier_dummy dummy_scheduler_notifier;

o_du_high_metrics_notifier_proxy::o_du_high_metrics_notifier_proxy(mac_metrics_notifier* notifier_) :
  mac_notifier(notifier_ ? notifier_ : &dummy_scheduler_notifier), odu_hi_notifier(&dummy_odu_high_notifier)
{
}

void o_du_high_metrics_notifier_proxy::on_new_metrics_report(const mac_metric_report& report)
{
  // Continue to report as expected.
  mac_notifier->on_new_metrics_report(report);

  // Report to E2.
  // :TODO:

  // Report new O-DU high metrics.
  odu_hi_notifier->on_new_metrics({});
}

void o_du_high_metrics_notifier_proxy::set_o_du_high_metrics_notifier(o_du_high_metrics_notifier& notifier)
{
  odu_hi_notifier = &notifier;
}
