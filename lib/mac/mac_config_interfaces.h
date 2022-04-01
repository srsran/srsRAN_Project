
#ifndef SRSGNB_MAC_CONFIG_INTERFACES_H
#define SRSGNB_MAC_CONFIG_INTERFACES_H

#include "srsgnb/mac/mac.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {

class mac_ul_configurer
{
public:
  virtual ~mac_ul_configurer()                                                               = default;
  virtual async_task<bool> add_ue(const mac_ue_create_request_message& msg)                  = 0;
  virtual async_task<bool> reconfigure_ue(const mac_ue_reconfiguration_request_message& msg) = 0;
  virtual async_task<void> remove_ue(const mac_ue_delete_request_message& msg)               = 0;
};

class mac_dl_configurer
{
public:
  virtual ~mac_dl_configurer()                                                               = default;
  virtual async_task<bool> add_ue(const mac_ue_create_request_message& msg)                  = 0;
  virtual async_task<bool> reconfigure_ue(const mac_ue_reconfiguration_request_message& msg) = 0;
  virtual async_task<void> remove_ue(const mac_ue_delete_request_message& msg)               = 0;
};

class mac_ctrl_configurer
{
public:
  virtual ~mac_ctrl_configurer()                                                        = default;
  virtual bool add_ue(rnti_t rnti, du_ue_index_t ue_index, du_cell_index_t pcell_index) = 0;
  virtual void remove_ue(rnti_t rnti)                                                   = 0;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_CONFIG_INTERFACES_H
