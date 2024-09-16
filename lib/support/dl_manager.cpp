/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#include "srsran/support/dl_manager.h"
#include <dlfcn.h>
#include <string>

using namespace srsran;

dl_manager::dl_manager(srslog::basic_logger& logger_) : logger(logger_) {}

bool dl_manager::open(const std::string& dl_name)
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

expected<void*> dl_manager::load_symbol(const std::string& symbol_name)
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
      logger.error("Error loading symbol {}: {}\n", "start_ngap_preparation_procedure_func", err);
    } else {
      logger.error("Error loading symbol {}:\n", "start_ngap_preparation_procedure_func");
    }
    return make_unexpected(default_error_t{});
  }
  logger.debug("Loaded symbol: {}", symbol_name);
  return symb;
}

bool dl_manager::close()
{
  if (dl_handle == nullptr) {
    return false;
  }

  if (::dlclose(dl_handle) != 0) {
    char* err = ::dlerror();
    if (err != nullptr) {
      logger.error("Failed to close DL handle: {}", err);
    } else {
      logger.error("Failed to close DL handle");
    }
    return false;
  }
  logger.debug("Closed library");
  return true;
}
