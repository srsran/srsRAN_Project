
#ifndef SRSGNB_SCHED_INTERFACE_H
#define SRSGNB_SCHED_INTERFACE_H

#include "srsgnb/ran/rnti.h"

namespace srsgnb {

class sched_cfg_notifier
{
public:
  virtual ~sched_cfg_notifier()                   = default;
  virtual void on_ue_config_complete(rnti_t)      = 0;
  virtual void on_ue_delete_response(rnti_t rnti) = 0;
};

class sched_interface
{
public:
  virtual ~sched_interface()                  = default;
  virtual void config_ue(rnti_t rnti)         = 0;
  virtual void delete_ue_request(rnti_t rnti) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_SCHED_INTERFACE_H
