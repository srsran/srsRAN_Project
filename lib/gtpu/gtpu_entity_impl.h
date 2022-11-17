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
#include "srsgnb/gtpu/gtpu_config.h"
#include "srsgnb/gtpu/gtpu_entity.h"
#include "srsgnb/srslog/logger.h"

namespace srsgnb {

class gtpu_entity_impl : public gtpu_entity
{
public:
  gtpu_entity_impl(uint32_t                      ue_index,
                   gtpu_config                   cfg,
                   gtpu_dl_lower_layer_notifier& dl_lower,
                   gtpu_ul_upper_layer_notifier& ul_upper) :
    logger(srslog::fetch_basic_logger("GTPU"))
  {
    dl = std::make_unique<gtpu_entity_dl>(ue_index, cfg.dl, dl_lower);
    logger.info("GTP-U DL configured: {}", cfg.dl);
    ul = std::make_unique<gtpu_entity_ul>(ue_index, cfg.ul, ul_upper);
    logger.info("GTP-U UL configured: {}", cfg.ul);
  }
  ~gtpu_entity_impl() override = default;

  gtpu_dl_upper_layer_interface* get_dl_upper_layer_interface() final { return dl.get(); }
  gtpu_ul_lower_layer_interface* get_ul_lower_layer_interface() final { return ul.get(); };

private:
  srslog::basic_logger& logger;

  std::unique_ptr<gtpu_entity_dl> dl = {};
  std::unique_ptr<gtpu_entity_ul> ul = {};
};
} // namespace srsgnb
