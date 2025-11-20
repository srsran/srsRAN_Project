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

#ifdef SRSRAN_TRACY_ENABLE
#include "tracy/Tracy.hpp"

#define SRSRAN_ZONE_SCOPED_NC ZoneScopedNC
#else
#define SRSRAN_ZONE_SCOPED_NC(name, color)                                                                             \
  do {                                                                                                                 \
  } while (0)
#endif
