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

#include "srsran/du/du.h"
#include "srsran/du/du_high_wrapper.h"
#include "srsran/du/du_power_controller.h"
#include "srsran/fapi_adaptor/fapi_adaptor.h"
#include <memory>
#include <vector>

namespace srsran {

class radio_unit;

class split6_du_impl : public srs_du::du, public du_power_controller
{
public:
  explicit split6_du_impl(std::vector<std::unique_ptr<fapi::fapi_adaptor>> adaptors_,
                          std::vector<std::unique_ptr<du_high_wrapper>>    du_list_);

  // See interface for documentation.
  du_power_controller& get_power_controller() override { return *this; }

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

private:
  std::vector<std::unique_ptr<du_high_wrapper>>    du_list;
  std::vector<std::unique_ptr<fapi::fapi_adaptor>> adaptors;
};

} // namespace srsran
