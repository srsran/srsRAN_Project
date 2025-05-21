/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "o_du_high_metrics_notifier_proxy.h"
#include "srsran/du/du_high/o_du_high_metrics.h"
#include "srsran/support/error_handling.h"

using namespace srsran;
using namespace srs_du;

namespace {

/// DU metrics notifier dummy implementation.
class du_metrics_notifier_dummy : public du_metrics_notifier
{
public:
  // See interface for documentation.
  void on_new_metric_report(const du_metrics_report& report) override {}
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
static du_metrics_notifier_dummy        dummy_du_notifier;

o_du_high_metrics_notifier_proxy::o_du_high_metrics_notifier_proxy(du_metrics_notifier* notifier_) :
  du_notifier(notifier_ ? notifier_ : &dummy_du_notifier), odu_hi_notifier(&dummy_odu_high_notifier)
{
}

void o_du_high_metrics_notifier_proxy::on_new_metric_report(const du_metrics_report& report)
{
  // Continue to report as expected.
  du_notifier->on_new_metric_report(report);

  // Report to E2.
  // :TODO:

  // Report new O-DU high metrics.
  odu_hi_notifier->on_new_metrics({});
}

void o_du_high_metrics_notifier_proxy::set_o_du_high_metrics_notifier(o_du_high_metrics_notifier& notifier)
{
  odu_hi_notifier = &notifier;
}
