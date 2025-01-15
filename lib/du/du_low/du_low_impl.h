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

#include "srsran/du/du_low/du_low.h"
#include "srsran/du/du_operation_controller.h"
#include "srsran/phy/upper/upper_phy.h"

namespace srsran {
namespace srs_du {

/// DU low implementation.
class du_low_impl final : public du_low, public du_operation_controller
{
public:
  explicit du_low_impl(std::vector<std::unique_ptr<upper_phy>> upper_);

  // See interface for documentation.
  upper_phy& get_upper_phy(unsigned cell_id) override;

  // See interface for documentation.
  du_operation_controller& get_operation_controller() override { return *this; }

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

private:
  std::vector<std::unique_ptr<upper_phy>> upper;
  std::vector<upper_phy*>                 upper_ptrs;
};

} // namespace srs_du
} // namespace srsran
