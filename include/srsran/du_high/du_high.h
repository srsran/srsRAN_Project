/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/f1ap/common/f1ap_common.h"

namespace srsran {

/// \brief Interface to DU-high class, which owns and manages the interaction between MAC, RLC and F1 layers.
class du_high
{
public:
  virtual ~du_high() = default;

  virtual void start() = 0;

  virtual void stop() = 0;

  virtual f1ap_message_handler& get_f1ap_message_handler() = 0;
};

} // namespace srsran