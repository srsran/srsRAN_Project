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

#include "split6_o_du_low_metrics_collector_impl.h"
#include "metrics/split6_flexible_o_du_low_metrics.h"
#include "srsran/du/du_low/o_du_low_metrics_collector.h"
#include "srsran/ru/ru_metrics_collector.h"

using namespace srsran;

namespace {

/// Radio Unit metrics collector dummy implementation.
class ru_metrics_collector_dummy : public ru_metrics_collector
{
public:
  void collect_metrics(ru_metrics& metrics) override {}
};

/// O-DU low metrics collector dummy implementation.
class o_du_low_metrics_collector_dummy : public srs_du::o_du_low_metrics_collector
{
public:
  void collect_metrics(srs_du::o_du_low_metrics& metrics) override {}
};

} // namespace

static o_du_low_metrics_collector_dummy dummy_odu_low;
static ru_metrics_collector_dummy       dummy_ru;

split6_o_du_low_metrics_collector_impl::split6_o_du_low_metrics_collector_impl(
    srs_du::o_du_low_metrics_collector*        odu_low_,
    ru_metrics_collector*                      ru_,
    split6_flexible_o_du_low_metrics_notifier* notifier_,
    unique_timer                               timer_,
    std::chrono::milliseconds                  report_period_) :
  timer(std::move(timer_)),
  report_period(report_period_),
  notifier(notifier_),
  odu_low(odu_low_ ? odu_low_ : &dummy_odu_low),
  ru(ru_ ? ru_ : &dummy_ru)
{
  // When no notifier is configured, do not start timer to collect metrics.
  if (!notifier || report_period.count() == 0) {
    return;
  }

  srsran_assert(timer.is_valid(), "Invalid timer passed to metrics controller");
  timer.set(report_period, [this](timer_id_t tid) { collect_metrics(); });

  stopped.store(false, std::memory_order_relaxed);
  timer.run();
}

split6_o_du_low_metrics_collector_impl&
split6_o_du_low_metrics_collector_impl::operator=(split6_o_du_low_metrics_collector_impl&& other) noexcept
{
  // Stop others timer to put it into known state.
  other.stopped.store(true, std::memory_order_relaxed);
  other.timer.stop();

  // Copy/move members.
  timer         = std::move(other.timer);
  report_period = other.report_period;
  notifier      = other.notifier;
  odu_low       = other.odu_low;
  ru            = other.ru;

  // Set to nullptr other.
  other.notifier = nullptr;
  other.odu_low  = nullptr;
  other.ru       = nullptr;

  if (!notifier || report_period.count() == 0) {
    return *this;
  }

  // Configure and start timer again.
  timer.set(report_period, [this](timer_id_t tid) { collect_metrics(); });

  stopped.store(false, std::memory_order_relaxed);
  timer.run();

  return *this;
}

split6_o_du_low_metrics_collector_impl::~split6_o_du_low_metrics_collector_impl()
{
  stopped.store(true, std::memory_order_relaxed);
  timer.stop();
}

void split6_o_du_low_metrics_collector_impl::collect_metrics()
{
  if (stopped.load(std::memory_order_relaxed)) {
    return;
  }

  split6_flexible_o_du_low_metrics metrics;
  odu_low->collect_metrics(metrics.du_low);
  ru->collect_metrics(metrics.ru);

  notifier->on_new_metrics(metrics);

  timer.run();
}
