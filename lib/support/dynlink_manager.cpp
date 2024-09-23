/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#include "srsran/support/dynlink_manager.h"
#include <dlfcn.h>
#include <string>

using namespace srsran;

std::optional<dynlink_manager> dynlink_manager::create(const std::string& dl_name, srslog::basic_logger& logger)
{
  dynlink_manager mng(dl_name, logger);
  if (not mng.open()) {
    return {};
  }
  return mng;
}

dynlink_manager::dynlink_manager(const std::string& dl_name_, srslog::basic_logger& logger_) :
  dl_name(dl_name_), logger(logger_)
{
}

dynlink_manager::~dynlink_manager()
{
  close();
}

bool dynlink_manager::open()
{
  char* err = nullptr;

  dl_handle = ::dlopen(dl_name.c_str(), RTLD_NOW + RTLD_DEEPBIND + RTLD_GLOBAL);
  if (dl_handle == nullptr) {
    err = ::dlerror();
    if (err != nullptr) {
      logger.error("Failed to load library {}: {}", dl_name, err);
    } else {
      logger.error("Failed to load library {}", dl_name);
    }
    return false;
  }
  logger.debug("Opened library: {}", dl_name);
  return true;
}

expected<void*> dynlink_manager::load_symbol(const std::string& symbol_name)
{
  if (dl_handle == nullptr) {
    return make_unexpected(default_error_t{});
  }

  void* symb = ::dlsym(dl_handle, symbol_name.c_str());

  // Handle an error loading the symbol.
  char* err = nullptr;
  if (symb == nullptr) {
    err = ::dlerror();
    if (err != nullptr) {
      logger.error("Error loading symbol {} in {}: {}\n", symbol_name, dl_name, err);
    } else {
      logger.error("Error loading symbol {} in {}:\n", symbol_name, dl_name);
    }
    return make_unexpected(default_error_t{});
  }
  logger.debug("Loaded symbol: {}", symbol_name);
  return symb;
}

bool dynlink_manager::close()
{
  if (dl_handle == nullptr) {
    return false;
  }

  if (::dlclose(dl_handle) != 0) {
    char* err = ::dlerror();
    if (err != nullptr) {
      logger.error("Failed to close dynamic link handle: {}", err);
    } else {
      logger.error("Failed to close dynamic link handle");
    }
    return false;
  }
  logger.debug("Closed library");
  return true;
}