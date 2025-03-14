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

#include "o_du_impl.h"
#include "srsran/du/du_high/o_du_high.h"
#include "srsran/du/du_low/o_du_low_metrics_collector.h"
#include "srsran/du/o_du_metrics.h"
#include "srsran/du/o_du_metrics_notifier.h"

using namespace srsran;
using namespace srs_du;

namespace {

/// O-DU metrics notifier dummy implementation.
class o_du_metrics_notifier_dummy : public o_du_metrics_notifier
{
public:
  // See interface for documentation.
  void on_new_metrics(const o_du_metrics& metrics) override {}
};

} // namespace

/// Dummy O-DU metrics notifier.
static o_du_metrics_notifier_dummy dummy_notifier;

o_du_impl::o_du_impl(o_du_dependencies&& dependencies) :
  metrics_notifier(dependencies.metrics_notifier ? *dependencies.metrics_notifier : dummy_notifier),
  odu_hi(std::move(dependencies.odu_hi)),
  odu_lo(std::move(dependencies.odu_lo))
{
  srsran_assert(odu_lo, "Invalid DU low");
  srsran_assert(odu_hi, "Invalid DU high");

  // Register the O-DU in the O-DU high to listen to O-DU high metrics.
  odu_hi->set_o_du_high_metrics_notifier(*this);
}

void o_du_impl::on_new_metrics(const o_du_high_metrics& metrics)
{
  o_du_metrics du_metrics;

  // Get O-DU low metrics.
  if (auto* odu_low_collector = odu_lo->get_metrics_collector()) {
    odu_low_collector->collect_metrics(du_metrics.low);
  }

  // Notify the metrics.
  metrics_notifier.on_new_metrics(du_metrics);
}

void o_du_impl::start()
{
  odu_hi->get_operation_controller().start();
  odu_lo->get_operation_controller().start();
}

void o_du_impl::stop()
{
  odu_lo->get_operation_controller().stop();
  odu_hi->get_operation_controller().stop();
}

o_du_high& o_du_impl::get_o_du_high()
{
  return *odu_hi;
}

o_du_low& o_du_impl::get_o_du_low()
{
  return *odu_lo;
}
