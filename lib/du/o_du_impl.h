/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/du/du_high/o_du_high.h"
#include "srsran/du/du_low/o_du_low.h"
#include "srsran/du/du_operation_controller.h"
#include "srsran/du/o_du.h"
#include <memory>

namespace srsran {
namespace srs_du {

/// O-RAN DU implementation dependencies.
struct o_du_impl_dependencies {
  std::unique_ptr<o_du_low>  du_lo;
  std::unique_ptr<o_du_high> du_hi;
};

/// O-RAN DU implementation.
class o_du_impl final : public o_du, public du_operation_controller
{
public:
  explicit o_du_impl(o_du_impl_dependencies&& du_cfg);

  // See interface for documentation.
  du_operation_controller& get_operation_controller() override { return *this; }

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

  // See interface for documentation.
  o_du_high& get_o_du_high() override;

  // See interface for documentation.
  o_du_low& get_o_du_low() override;

private:
  std::unique_ptr<o_du_high> du_hi;
  std::unique_ptr<o_du_low>  du_lo;
};

} // namespace srs_du
} // namespace srsran
