/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/hal/phy/upper/channel_processors/pusch/hw_accelerator_factories.h"
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
  /// Indicates if the accelerated function uses a dedicated hardware queue or needs to reserve one for each operation.
  bool dedicated_queue;

public:
  // Default constructor.
  explicit hw_accelerator_pusch_dec_factory_spec(const hw_accelerator_pusch_dec_configuration& accelerator_config) :
    acc_type(accelerator_config.acc_type),
    bbdev_accelerator(std::move(accelerator_config.bbdev_accelerator)),
    ext_softbuffer(accelerator_config.ext_softbuffer),
    harq_buffer_context(accelerator_config.harq_buffer_context),
    dedicated_queue(accelerator_config.dedicated_queue)
  {
  }

  std::unique_ptr<hw_accelerator_pusch_dec> create() override
  {
    if (acc_type == "acc100") {
      return std::make_unique<hw_accelerator_pusch_dec_acc100_impl>(
          bbdev_accelerator, ext_softbuffer, harq_buffer_context, dedicated_queue);
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
