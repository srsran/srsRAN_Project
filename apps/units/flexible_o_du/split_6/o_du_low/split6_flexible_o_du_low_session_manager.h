/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "split6_flexible_o_du_low_session.h"
#include "split6_flexible_o_du_low_session_factory.h"
#include "srsran/fapi/cell_operation_request_notifier.h"
#include <memory>

namespace srsran {
class split6_flexible_o_du_low_metrics_notifier;

/// \brief Split 6 flexible O-DU low session manager.
///
/// This class listens to on start/stop requests from FAPI and creates the flexible O-DU low session.
class split6_flexible_o_du_low_session_manager : public fapi::cell_operation_request_notifier
{
public:
  explicit split6_flexible_o_du_low_session_manager(
      std::unique_ptr<split6_flexible_o_du_low_session_factory> odu_low_session_factory_) :
    odu_low_session_factory(std::move(odu_low_session_factory_))
  {
    srsran_assert(odu_low_session_factory, "Invalid O-DU low session factory");
  }

  // See interface for documentation.
  bool on_start_request(const fapi::fapi_cell_config& config) override;

  // See interface for documentation.
  void on_stop_request() override { flexible_odu_low.reset(); }

private:
  std::unique_ptr<split6_flexible_o_du_low_session_factory> odu_low_session_factory;
  std::unique_ptr<split6_flexible_o_du_low_session>         flexible_odu_low;
};

} // namespace srsran
