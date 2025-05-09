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

#include "split6_flexible_o_du_low_impl.h"
#include "srsran/fapi/cell_operation_request_notifier.h"
#include <memory>

namespace srsran {

/// \brief Cell operation request handler implementation.
///
/// This class listens to on start/stop requests from FAPI and creates the objects needed to run the slot messages of
/// FAPI.
class cell_operation_request_handler_impl : public fapi::cell_operation_request_notifier
{
public:
  // See interface for documentation.
  bool on_start_request(const fapi::fapi_cell_config& config) override;

  // See interface for documentation.
  void on_stop_request() override { flexible_odu_low.reset(); }

private:
  std::unique_ptr<split6_flexible_o_du_low_impl> flexible_odu_low;
};

} // namespace srsran
