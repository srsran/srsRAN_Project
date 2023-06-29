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

#include "srsran/du/du.h"

namespace srsran {

class du_impl : public du
{
public:
  du_impl();

  void start() override;

  void stop() override;

private:
};

} // namespace srsran