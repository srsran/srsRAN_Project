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

#include "plugin_radio_factory.h"
#include "srsran/radio/radio_factory.h"
#include "srsran/radio/radio_session.h"
#include <dlfcn.h>
#include <memory>

using namespace srsran;

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

/// Wraps a radio session with the dynamic library handle.
class radio_dynamic_wrapper : public radio_session
{
public:
  /// Constructs a wrapper from the dynamic library handler and an open radio session.
  explicit radio_dynamic_wrapper(handler_ptr handler_, std::unique_ptr<radio_session> session_) noexcept :
    handler(std::move(handler_)), session(std::move(session_))
  {
  }

  /// Default destructor - deletes the session first then the dynamic library handler.
  ~radio_dynamic_wrapper()
  {
    session.reset();
    handler.reset();
  }

  radio_management_plane& get_management_plane() override { return session->get_management_plane(); }
  baseband_gateway&       get_baseband_gateway(unsigned stream_id) override
  {
    return session->get_baseband_gateway(stream_id);
  }
  baseband_gateway_timestamp read_current_time() override { return session->read_current_time(); }
  void                       start(baseband_gateway_timestamp init_time) override { session->start(init_time); }
  void                       stop() override { session->stop(); }

private:
  handler_ptr                    handler;
  std::unique_ptr<radio_session> session;
};

/// Wraps a radio factory and a dynamic library handler.
class radio_factory_dynamic_wrapper : public radio_factory
{
public:
  /// Constructs from handler and factory.
  radio_factory_dynamic_wrapper(handler_ptr handle_, std::unique_ptr<radio_factory> factory_) :
    handle(std::move(handle_)), factory(std::move(factory_))
  {
  }

  /// Default destructor - deletes the factory first then the dynamic library handler.
  ~radio_factory_dynamic_wrapper()
  {
    factory.reset();
    handle.reset();
  }

  // See interface for documentation.
  const radio_configuration::validator& get_configuration_validator() override
  {
    return factory->get_configuration_validator();
  }

  // See interface for documentation.
  std::unique_ptr<radio_session> create(const radio_configuration::radio& config,
                                        task_executor&                    async_task_executor,
                                        radio_notification_handler&       notifier) override
  {
    return std::make_unique<radio_dynamic_wrapper>(handle, factory->create(config, async_task_executor, notifier));
  }

private:
  handler_ptr                    handle;
  std::unique_ptr<radio_factory> factory;
};

} // namespace

std::unique_ptr<radio_factory> srsran::create_plugin_radio_factory(std::string driver_name)
{
  char*       err         = nullptr;
  std::string plugin_name = "libsrsran_radio_" + driver_name + ".so";

  void* dl_handle = ::dlopen(plugin_name.c_str(), RTLD_NOW + RTLD_DEEPBIND + RTLD_GLOBAL);
  if (dl_handle == nullptr) {
    err = ::dlerror();
    if (err != nullptr) {
      fmt::print("Failed to load RF plugin {}: {}\n", plugin_name, err);
    }
    return nullptr;
  }

  // Create factory function prototype.
  using create_factory_func = std::unique_ptr<radio_factory>(void);

  // Load symbol.
  auto* create_factory =
      reinterpret_cast<create_factory_func*>(::dlsym(dl_handle, "_ZN6srsran28create_dynamic_radio_factoryEv"));

  // Handle an error loading the symbol.
  if (create_factory == nullptr) {
    err = ::dlerror();
    if (err != nullptr) {
      fmt::print("Error loading symbol {}: {}\n", "create_dynamic_radio_factory", err);
    }
    return nullptr;
  }

  // Create factory.
  std::unique_ptr<radio_factory> factory = (*create_factory)();

  if (factory == nullptr) {
    return nullptr;
  }

  // Create shared pointer with deleter.
  dynamic_library_deleter deleter;
  handler_ptr             handle(dl_handle, deleter);

  return std::make_unique<radio_factory_dynamic_wrapper>(std::move(handle), std::move(factory));
}
