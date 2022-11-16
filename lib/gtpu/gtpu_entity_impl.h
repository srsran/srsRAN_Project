/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "gtpu_entity_dl.h"
#include "gtpu_entity_ul.h"
#include "srsgnb/srslog/logger.h"

namespace srsgnb {

class gtpu_entity_impl : public gtpu_entity
{
public:
  gtpu_entity_impl(uint32_t ue_index) : logger("GTPU")
  {
    dl = std::make_unique<gtpu_entity_dl>(ue_index);
    logger.log_info("GTP-U DL configured"); // TODO print config
    ul = std::make_unique<gtpu_entity_ul>(ue_index);
    logger.log_info("GTP-U UL configured"); // TODO print config
  }
  ~gtpu_entity_impl() override = default;

private:
  srslog::basic_logger& logger;

  std::unique_ptr<gtpu_entity_dl> dl = {};
  std::unique_ptr<gtpu_entity_ul> ul = {};
};
} // namespace srsgnb
