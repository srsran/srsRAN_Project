
#ifndef SRSGNB_SCHED_INTERFACE_H
#define SRSGNB_SCHED_INTERFACE_H

#include "sched_configurer.h"
#include "sched_result.h"

namespace srsgnb {

class sched_interface : public sched_configurer
{
public:
  virtual ~sched_interface()                                                                = default;
  virtual const dl_sched_result* get_dl_sched(slot_point sl_tx, du_cell_index_t cell_index) = 0;
  virtual const ul_sched_result* get_ul_sched(slot_point sl_tx, du_cell_index_t cell_index) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_SCHED_INTERFACE_H
