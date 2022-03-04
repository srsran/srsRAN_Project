
#ifndef SRSGNB_SCHED_INTERFACE_H
#define SRSGNB_SCHED_INTERFACE_H

#include "srsgnb/adt/static_vector.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

const size_t MAX_GRANTS = 16;

struct dl_sched_info {};

using dl_sched_result = static_vector<dl_sched_info, MAX_GRANTS>;

struct ul_sched_info {};

using ul_sched_result = static_vector<ul_sched_info, MAX_GRANTS>;

/// Interface to Add/Remove UEs and Cells
class sched_configurer
{
public:
  virtual ~sched_configurer()                 = default;
  virtual void config_ue(rnti_t rnti)         = 0;
  virtual void delete_ue_request(rnti_t rnti) = 0;
};

class sched_interface : public sched_configurer
{
public:
  virtual const dl_sched_result* get_dl_sched(slot_point sl_tx, du_cell_index_t cc) = 0;
  virtual const ul_sched_result* get_ul_sched(slot_point sl_tx, du_cell_index_t cc) = 0;
};

class sched_cfg_notifier
{
public:
  virtual ~sched_cfg_notifier()                   = default;
  virtual void on_ue_config_complete(rnti_t)      = 0;
  virtual void on_ue_delete_response(rnti_t rnti) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_SCHED_INTERFACE_H
