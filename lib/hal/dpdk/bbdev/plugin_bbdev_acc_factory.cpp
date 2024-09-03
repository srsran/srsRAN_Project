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

#include "plugin_bbdev_acc_factory.h"
#include "srsran/hal/dpdk/bbdev/bbdev_acc.h"
#include "srsran/hal/dpdk/bbdev/bbdev_acc_factory.h"
#include <dlfcn.h>

using namespace srsran;
using namespace dpdk;

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

/// Wraps a bbdev accelerator interface with the dynamic library handle.
class bbdev_acc_dynamic_wrapper : public bbdev_acc
{
public:
  /// Constructs a wrapper from the dynamic library handler and an open bbdev accelerator interface.
  explicit bbdev_acc_dynamic_wrapper(handler_ptr handler_, std::shared_ptr<bbdev_acc> acc_) noexcept :
    handler(std::move(handler_)), acc(std::move(acc_))
  {
  }

  /// Default destructor - deletes the instance first then the dynamic library handler.
  ~bbdev_acc_dynamic_wrapper()
  {
    acc.reset();
    handler.reset();
  }

  unsigned              get_device_id() const override { return acc->get_device_id(); }
  int                   get_socket_id() const override { return acc->get_socket_id(); }
  unsigned              get_nof_ldpc_enc_cores() const override { return acc->get_nof_ldpc_enc_cores(); }
  unsigned              get_nof_ldpc_dec_cores() const override { return acc->get_nof_ldpc_dec_cores(); }
  unsigned              get_nof_fft_cores() const override { return acc->get_nof_fft_cores(); }
  uint64_t              get_harq_buff_size_bytes() const override { return acc->get_harq_buff_size_bytes(); }
  units::bytes          get_msg_mbuf_size() const override { return acc->get_msg_mbuf_size(); }
  units::bytes          get_rm_mbuf_size() const override { return acc->get_rm_mbuf_size(); }
  unsigned              get_nof_mbuf() const override { return acc->get_nof_mbuf(); }
  srslog::basic_logger& get_logger() override { return acc->get_logger(); }
  int                   reserve_queue(::rte_bbdev_op_type op_type) override { return acc->reserve_queue(op_type); }
  void                  free_queue(::rte_bbdev_op_type op_type, unsigned queue_id) override
  {
    return acc->free_queue(op_type, queue_id);
  }
  unsigned reserve_encoder() override { return acc->reserve_encoder(); }
  unsigned reserve_decoder() override { return acc->reserve_decoder(); }

private:
  handler_ptr                handler;
  std::shared_ptr<bbdev_acc> acc;
};

/// Wraps a bbdev accelerator factory and a dynamic library handler.
class bbdev_acc_factory_dynamic_wrapper : public bbdev_acc_factory
{
public:
  /// Constructs from handler and factory.
  bbdev_acc_factory_dynamic_wrapper(handler_ptr handle_, std::unique_ptr<bbdev_acc_factory> factory_) :
    handle(std::move(handle_)), factory(std::move(factory_))
  {
  }

  /// Default destructor - deletes the factory first then the dynamic library handler.
  ~bbdev_acc_factory_dynamic_wrapper()
  {
    factory.reset();
    handle.reset();
  }

  // See interface for documentation.
  std::shared_ptr<bbdev_acc> create(const bbdev_acc_configuration& cfg, srslog::basic_logger& logger) override
  {
    // Create bbdev accelerator using the loaded factory.
    std::shared_ptr<bbdev_acc> bbdev_if = factory->create(cfg, logger);
    if (bbdev_if == nullptr) {
      report_error("Failed to create plugin bbdev_acc");
    }

    // Wrap the dynamic library handler and the bbdev accelerator instance.
    return std::make_unique<bbdev_acc_dynamic_wrapper>(handle, std::move(bbdev_if));
  }

private:
  handler_ptr                        handle;
  std::unique_ptr<bbdev_acc_factory> factory;
};

} // namespace

std::unique_ptr<bbdev_acc_factory> srsran::dpdk::create_plugin_bbdev_acc_factory(std::string impl_name)
{
  char*       err         = nullptr;
  std::string plugin_name = "libsrsran_bbdev_acc_" + impl_name + ".so";

  void* dl_handle = ::dlopen(plugin_name.c_str(), RTLD_NOW + RTLD_DEEPBIND + RTLD_GLOBAL);
  if (dl_handle == nullptr) {
    err = ::dlerror();
    if (err != nullptr) {
      fmt::print("Failed to load bbdev_acc plugin {}: {}\n", plugin_name, err);
    }
    return nullptr;
  }

  // Create factory function prototype.
  using create_factory_func = std::unique_ptr<bbdev_acc_factory>(void);

  // Load symbol.
  auto* create_factory = reinterpret_cast<create_factory_func*>(::dlsym(dl_handle, "create_dynamic_bbdev_acc_factory"));

  // Handle an error loading the symbol.
  if (create_factory == nullptr) {
    err = ::dlerror();
    if (err != nullptr) {
      fmt::print("Error loading symbol {}: {}\n", "create_dynamic_bbdev_acc_factory", err);
    }
    return nullptr;
  }

  // Create factory.
  std::unique_ptr<bbdev_acc_factory> factory = (*create_factory)();

  if (factory == nullptr) {
    return nullptr;
  }

  // Create shared pointer with deleter.
  dynamic_library_deleter deleter;
  handler_ptr             handle(dl_handle, deleter);

  return std::make_unique<bbdev_acc_factory_dynamic_wrapper>(std::move(handle), std::move(factory));
}
