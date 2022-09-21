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

#include "du_manager_config.h"
#include "du_ue/du_ue.h"

namespace srsgnb {
namespace srs_du {

class ue_manager_ctrl_configurator
{
public:
  virtual ~ue_manager_ctrl_configurator()          = default;
  virtual du_ue* add_ue(std::unique_ptr<du_ue> u)  = 0;
  virtual void   remove_ue(du_ue_index_t ue_index) = 0;
  virtual du_ue* find_ue(du_ue_index_t ue_index)   = 0;
  virtual du_ue* find_rnti(rnti_t rnti)            = 0;
};

} // namespace srs_du
} // namespace srsgnb
