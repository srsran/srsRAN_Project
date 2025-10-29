/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/rohc/rohc_engine.h"
#include "srsran/srslog/logger.h"

namespace srsran::rohc {

class rohc_lib_engine : public rohc_engine
{
public:
  rohc_lib_engine();
  ~rohc_lib_engine() = default;

private:
  srslog::basic_logger& logger;
};

} // namespace srsran::rohc
