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

#include "plugin_bbdev_pusch_dec_acc_factory.h"
#include "srsran/hal/phy/upper/channel_processors/pusch/hw_accelerator_pusch_dec.h"
#include "srsran/hal/phy/upper/channel_processors/pusch/hw_accelerator_pusch_dec_factory.h"
#include <dlfcn.h>

using namespace srsran;
using namespace hal;

namespace {

/// Dynamic library handler deleter - closes the dynamic library upon destruction.
struct dynamic_library_deleter {
  void operator()(void* handler) const
  {
    if (handler != nullptr) {
      ::dlclose(handler);
    }
  }
};

using handler_ptr = std::shared_ptr<void>;

/// Wraps a PUSCH decoder hardware-accelerator interface with the dynamic library handle.
class hw_accelerator_pusch_dec_dynamic_wrapper : public hw_accelerator_pusch_dec
{
public:
  /// Constructs a wrapper from the dynamic library handler and an open PUSCH decoder hardware-accelerator interface.
  explicit hw_accelerator_pusch_dec_dynamic_wrapper(handler_ptr                               handler_,
                                                    std::shared_ptr<hw_accelerator_pusch_dec> pusch_acc_) noexcept :
    handler(std::move(handler_)), pusch_acc(std::move(pusch_acc_))
  {
  }

  /// Default destructor - deletes the instance first then the dynamic library handler.
  ~hw_accelerator_pusch_dec_dynamic_wrapper()
  {
    pusch_acc.reset();
    handler.reset();
  }

  bool enqueue_operation(span<const int8_t> data, span<const int8_t> aux_data = {}, unsigned cb_index = 0) override
  {
    return pusch_acc->enqueue_operation(data, aux_data, cb_index);
  }
  bool dequeue_operation(span<uint8_t> data, span<int8_t> aux_data = {}, unsigned segment_index = 0) override
  {
    return pusch_acc->dequeue_operation(data, aux_data, segment_index);
  }
  void reserve_queue() override { return pusch_acc->reserve_queue(); }
  void free_queue() override { return pusch_acc->free_queue(); }
  void configure_operation(const hw_pusch_decoder_configuration& config, unsigned cb_index = 0) override
  {
    return pusch_acc->configure_operation(config, cb_index);
  }
  void
  read_operation_outputs(hw_pusch_decoder_outputs& out, unsigned cb_index = 0, unsigned absolute_cb_id = 0) override
  {
    return pusch_acc->read_operation_outputs(out, cb_index, absolute_cb_id);
  }
  void free_harq_context_entry(unsigned absolute_cb_id) override
  {
    return pusch_acc->free_harq_context_entry(absolute_cb_id);
  }
  bool is_external_harq_supported() const override { return pusch_acc->is_external_harq_supported(); }

private:
  handler_ptr                               handler;
  std::shared_ptr<hw_accelerator_pusch_dec> pusch_acc;
};

/// Wraps a bbdev-based PUSCH decoder hardware-accelerator factory and a dynamic library handler.
class bbdev_pusch_dec_acc_factory_dynamic_wrapper : public hw_accelerator_pusch_dec_factory
{
public:
  /// Constructs from handler and factory.
  bbdev_pusch_dec_acc_factory_dynamic_wrapper(handler_ptr                                       handle_,
                                              std::unique_ptr<hw_accelerator_pusch_dec_factory> factory_) :
    handle(std::move(handle_)), factory(std::move(factory_))
  {
  }

  /// Default destructor - deletes the factory first then the dynamic library handler.
  ~bbdev_pusch_dec_acc_factory_dynamic_wrapper()
  {
    factory.reset();
    handle.reset();
  }

  // See interface for documentation.
  std::unique_ptr<hw_accelerator_pusch_dec> create() override
  {
    // Create bbdev-based PUSCH decoder hardware-accelerator using the loaded factory.
    std::unique_ptr<hw_accelerator_pusch_dec> pusch_dec_if = factory->create();
    if (pusch_dec_if == nullptr) {
      report_error("Failed to create plugin bbdev_pusch_dec_acc_factory");
    }

    // Wrap the dynamic library handler and the bbdev-based PUSCH decoder hardware-accelerator instance.
    return std::make_unique<hw_accelerator_pusch_dec_dynamic_wrapper>(handle, std::move(pusch_dec_if));
  }

private:
  handler_ptr                                       handle;
  std::unique_ptr<hw_accelerator_pusch_dec_factory> factory;
};

} // namespace

std::unique_ptr<hw_accelerator_pusch_dec_factory> srsran::hal::create_plugin_bbdev_pusch_dec_acc_factory(
    const bbdev_hwacc_pusch_dec_factory_configuration& accelerator_config,
    std::string                                        impl_name)
{
  char*       err         = nullptr;
  std::string plugin_name = "libsrsran_bbdev_pusch_dec_acc_" + impl_name + ".so";

  void* dl_handle = ::dlopen(plugin_name.c_str(), RTLD_NOW + RTLD_DEEPBIND + RTLD_GLOBAL);
  if (dl_handle == nullptr) {
    err = ::dlerror();
    if (err != nullptr) {
      fmt::print("Failed to load bbdev_pusch_dec_acc plugin {}: {}\n", plugin_name, err);
    }
    return nullptr;
  }

  // Create factory function prototype.
  using create_factory_func =
      std::unique_ptr<hw_accelerator_pusch_dec_factory>(const bbdev_hwacc_pusch_dec_factory_configuration&);

  // Load symbol.
  auto* create_factory =
      reinterpret_cast<create_factory_func*>(::dlsym(dl_handle, "create_dynamic_bbdev_pusch_dec_acc_factory"));

  // Handle an error loading the symbol.
  if (create_factory == nullptr) {
    err = ::dlerror();
    if (err != nullptr) {
      fmt::print("Error loading symbol {}: {}\n", "create_dynamic_bbdev_pusch_dec_acc_factory", err);
    }
    return nullptr;
  }

  // Create factory.
  std::unique_ptr<hw_accelerator_pusch_dec_factory> factory = (*create_factory)(accelerator_config);

  if (factory == nullptr) {
    return nullptr;
  }

  // Create shared pointer with deleter.
  dynamic_library_deleter deleter;
  handler_ptr             handle(dl_handle, deleter);

  return std::make_unique<bbdev_pusch_dec_acc_factory_dynamic_wrapper>(std::move(handle), std::move(factory));
}
