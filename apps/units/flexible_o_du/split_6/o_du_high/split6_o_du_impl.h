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

#include "srsran/du/du.h"
#include "srsran/du/du_high/o_du_high.h"
#include "srsran/du/du_operation_controller.h"
#include "srsran/fapi_adaptor/phy/phy_fapi_adaptor.h"
#include <memory>
#include <vector>

namespace srsran {

class radio_unit;

/// Split 6 O-RAN DU implementation.
class split6_o_du_impl : public srs_du::du, public du_operation_controller
{
public:
  explicit split6_o_du_impl(unsigned                                        nof_cells_,
                            std::unique_ptr<fapi_adaptor::phy_fapi_adaptor> adaptor_,
                            std::unique_ptr<srs_du::o_du_high>              odu_hi_);

  // See interface for documentation.
  du_operation_controller& get_operation_controller() override { return *this; }

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

private:
  const unsigned                                  nof_cells;
  std::unique_ptr<srs_du::o_du_high>              odu_hi;
  std::unique_ptr<fapi_adaptor::phy_fapi_adaptor> adaptor;
};

} // namespace srsran
