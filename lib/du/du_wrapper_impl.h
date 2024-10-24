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

#include "srsran/du/du_high/du_high_wrapper.h"
#include "srsran/du/du_low/du_low_wrapper.h"
#include "srsran/du/du_power_controller.h"
#include "srsran/du/du_wrapper.h"
#include <memory>

namespace srsran {
namespace srs_du {

/// DU wrapper implementation dependencies.
struct du_wrapper_impl_dependencies {
  std::unique_ptr<du_low_wrapper>  du_lo;
  std::unique_ptr<du_high_wrapper> du_hi;
};

/// DU wrapper implementation.
class du_wrapper_impl final : public du_wrapper, public du_power_controller
{
public:
  explicit du_wrapper_impl(du_wrapper_impl_dependencies&& du_cfg);

  // See interface for documentation.
  du_power_controller& get_power_controller() override { return *this; }

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

  // See interface for documentation.
  du_high_wrapper& get_du_high_wrapper() override;

  // See interface for documentation.
  du_low_wrapper& get_du_low_wrapper() override;

private:
  std::unique_ptr<du_high_wrapper> du_hi;
  std::unique_ptr<du_low_wrapper>  du_lo;
};

} // namespace srs_du
} // namespace srsran
