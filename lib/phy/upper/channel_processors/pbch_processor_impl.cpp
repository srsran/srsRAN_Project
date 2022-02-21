/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pbch_processor_impl.h"

using namespace srsgnb;

void pbch_processor_impl::proccess(const srsgnb::pbch_processor::pdu_t& pdu, srsgnb::resource_grid_writer& grid)
{
  // Derive number of slots in 5ms
  unsigned nslots_burst = 1 << pdu.slot_context.numerology;

  // Generate PBCH message
  pbch_encoder::pbch_msg_t pbch_msg = {};
  pbch_msg.N_id                     = pdu.phys_cell_id;
  pbch_msg.ssb_idx                  = pdu.ssb_block_index;
}
