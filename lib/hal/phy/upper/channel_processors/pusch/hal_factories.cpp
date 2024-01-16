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

#include "srsran/hal/phy/upper/channel_processors/pusch/hal_factories.h"
#include "../../../../dpdk/bbdev/bbdev.h"
#include "../../../../dpdk/dpdk.h"
#include "hw_accelerator_pusch_dec_acc100_impl.h"
#include "hw_accelerator_pusch_dec_impl.h"

using namespace srsran;
using namespace hal;

#ifdef ENABLE_PUSCH_HWACC

namespace {

class hw_accelerator_pusch_dec_factory_spec : public hw_accelerator_pusch_dec_factory
{
private:
  /// Accelerator type.
  std::string acc_type;
  /// Interfacing to a bbdev-based hardware-accelerator.
  std::shared_ptr<srsran::dpdk::bbdev_acc> bbdev_accelerator;
  /// Defines if the soft-buffer is implemented in the accelerator or not.
  bool ext_softbuffer;
  /// Interfacing to an external HARQ buffer context repository.
  std::shared_ptr<ext_harq_buffer_context_repository> harq_buffer_context;

public:
  // Default constructor.
  explicit hw_accelerator_pusch_dec_factory_spec(const hw_accelerator_pusch_dec_configuration& accelerator_config) :
    acc_type(accelerator_config.acc_type),
    bbdev_accelerator(std::move(accelerator_config.bbdev_accelerator)),
    ext_softbuffer(accelerator_config.ext_softbuffer),
    harq_buffer_context(accelerator_config.harq_buffer_context)
  {
  }

  std::unique_ptr<hw_accelerator_pusch_dec> create() override
  {
    if (acc_type == "acc100") {
      return std::make_unique<hw_accelerator_pusch_dec_acc100_impl>(
          bbdev_accelerator, ext_softbuffer, harq_buffer_context);
    }
    // Handle other accelerator types here.
    return {};
  }
};

} // namespace

#endif // ENABLE_PUSCH_HWACC

std::shared_ptr<hw_accelerator_pusch_dec_factory>
srsran::hal::create_hw_accelerator_pusch_dec_factory(const hw_accelerator_pusch_dec_configuration& accelerator_config)
{
#ifdef ENABLE_PUSCH_HWACC
  return std::make_shared<hw_accelerator_pusch_dec_factory_spec>(accelerator_config);
#else  // ENABLE_PUSCH_HWACC
  return nullptr;
#endif // ENABLE_PUSCH_HWACC
}
