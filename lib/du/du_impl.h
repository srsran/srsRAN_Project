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
#include "srsran/du/du_config.h"
#include "srsran/du_high/du_high.h"

namespace srsran {

class du_impl final : public du
{
public:
  du_impl(const du_config& du_cfg);

  void start() override;

  void stop() override;

  f1ap_message_handler& get_f1ap_message_handler() override { return du_hi->get_f1ap_message_handler(); }

private:
  srslog::basic_logger& logger;

  std::unique_ptr<du_high> du_hi;
};

} // namespace srsran