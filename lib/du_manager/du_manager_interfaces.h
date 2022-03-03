
#ifndef SRSGNB_DU_MANAGER_INTERFACES_H
#define SRSGNB_DU_MANAGER_INTERFACES_H

#include "du_manager_config.h"
#include "du_ue_context.h"

namespace srsgnb {

class ue_manager_ctrl_configurer
{
public:
  virtual ~ue_manager_ctrl_configurer()                    = default;
  virtual du_ue_context* add_ue(du_ue_context u)           = 0;
  virtual void           remove_ue(du_ue_index_t ue_index) = 0;
  virtual du_ue_context* find_ue(du_ue_index_t ue_index)   = 0;
  virtual du_ue_context* find_rnti(rnti_t rnti)            = 0;
};
} // namespace srsgnb

#endif // SRSGNB_DU_MANAGER_INTERFACES_H
