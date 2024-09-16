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

#include "srsran/adt/expected.h"
#include "srsran/srslog/logger.h"

namespace srsran {
class dl_manager
{
public:
  explicit dl_manager(srslog::basic_logger& logger_);

  [[nodiscard]] bool open(const std::string& dl_name);

  expected<void*> load_symbol(const std::string& symbol_name);

  [[nodiscard]] bool close();

private:
  void*                 dl_handle = nullptr;
  srslog::basic_logger& logger;
};

} // namespace srsran
