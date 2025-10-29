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
#include <memory>

namespace srsran::rohc {

std::unique_ptr<rohc_engine> create_rohc_engine();

} // namespace srsran::rohc
