/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#pragma once

#include "srsran/du_low/du_low.h"
#include "srsran/du_low/du_low_wrapper.h"
#include "srsran/fapi_adaptor/phy/phy_fapi_adaptor.h"
#include <memory>
#include <vector>

namespace srsran {

class du_low_wrapper_impl final : public du_low_wrapper
{
public:
  du_low_wrapper_impl(std::unique_ptr<du_low>                                      du_lo_,
                      std::vector<std::unique_ptr<fapi_adaptor::phy_fapi_adaptor>> fapi_adaptors_);

  // See interface for documentation.
  du_low& get_du_low() override;

  // See interface for documentation.
  fapi::slot_message_gateway& get_slot_message_gateway(unsigned cell_id) override;

  // See interface for documentation.
  fapi::slot_last_message_notifier& get_slot_last_message_notifier(unsigned cell_id) override;

  // See interface for documentation.
  void set_slot_time_message_notifier(unsigned cell_id, fapi::slot_time_message_notifier& fapi_time_notifier) override;

  // See interface for documentation.
  void set_slot_error_message_notifier(unsigned                           cell_id,
                                       fapi::slot_error_message_notifier& fapi_error_notifier) override;

  // See interface for documentation.
  void set_slot_data_message_notifier(unsigned cell_id, fapi::slot_data_message_notifier& fapi_data_notifier) override;

private:
  std::unique_ptr<du_low>                                      du_lo;
  std::vector<std::unique_ptr<fapi_adaptor::phy_fapi_adaptor>> fapi_adaptors;
};

} // namespace srsran
