/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_DATA_SCHED_OUTPUT_H
#define SRSGNB_DATA_SCHED_OUTPUT_H

#include "data_sched_input.h"
#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

/// Object where data scheduler strategy stores DL grants
class dl_data_sched_output
{
public:
  dl_data_sched_output(slot_point sl_tx_, du_cell_index_t cell_index_) : pdcch_slot(sl_tx_), cell_index(cell_index_) {}

  bool alloc_pdsch(ue_candidate ue)
  {
    // TODO
    return true;
  }

  const slot_point      pdcch_slot;
  const du_cell_index_t cell_index;
};

/// Object where data scheduler strategy stores UL grants
class ul_data_sched_output
{
public:
  ul_data_sched_output(slot_point sl_tx_, du_cell_index_t cell_index_) : pdcch_slot(sl_tx_), cell_index(cell_index_) {}

  bool alloc_pusch(ue_candidate ue)
  {
    // TODO
    return true;
  }

  const slot_point      pdcch_slot;
  const du_cell_index_t cell_index;
};

} // namespace srsgnb

#endif // SRSGNB_DATA_SCHED_OUTPUT_H