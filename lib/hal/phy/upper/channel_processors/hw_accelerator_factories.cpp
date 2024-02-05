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

#include "srsran/hal/phy/upper/channel_processors/hw_accelerator_factories.h"
#include "../../../dpdk/bbdev/bbdev.h"
#include "../../../dpdk/dpdk.h"
#include "hw_accelerator_pdsch_enc_acc100_impl.h"
#include "hw_accelerator_pdsch_enc_impl.h"

using namespace srsran;
using namespace hal;

#ifdef ENABLE_PDSCH_HWACC

namespace {

class hw_accelerator_pdsch_enc_factory_spec : public hw_accelerator_pdsch_enc_factory
{
private:
  /// Accelerator type.
  std::string acc_type;
  /// Interfacing to a bbdev-based hardware-accelerator.
  std::shared_ptr<srsran::dpdk::bbdev_acc> bbdev_accelerator;

public:
  // Default constructor.
  explicit hw_accelerator_pdsch_enc_factory_spec(const hw_accelerator_pdsch_enc_configuration& accelerator_config) :
    acc_type(accelerator_config.acc_type), bbdev_accelerator(std::move(accelerator_config.bbdev_accelerator))
  {
  }

  std::unique_ptr<hw_accelerator_pdsch_enc> create() override
  {
    if (acc_type == "acc100") {
      return std::make_unique<hw_accelerator_pdsch_enc_acc100_impl>(bbdev_accelerator);
    }
    // Handle other accelerator types here.
    return {};
  }
};

} // namespace

#endif // ENABLE_PDSCH_HWACC

std::shared_ptr<hw_accelerator_pdsch_enc_factory>
srsran::hal::create_hw_accelerator_pdsch_enc_factory(const hw_accelerator_pdsch_enc_configuration& accelerator_config)
{
#ifdef ENABLE_PDSCH_HWACC
  return std::make_shared<hw_accelerator_pdsch_enc_factory_spec>(accelerator_config);
#else  // ENABLE_PDSCH_HWACC
  return nullptr;
#endif // ENABLE_PDSCH_HWACC
}
