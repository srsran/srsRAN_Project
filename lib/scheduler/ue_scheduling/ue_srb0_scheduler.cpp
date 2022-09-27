/*
*
* Copyright 2013-2022 Software Radio Systems Limited
*
* By using this file, you agree to the terms and conditions set
* forth in the LICENSE file which can be found at the top level of
* the distribution.
*
*/

#include "ue_srb0_scheduler.h"

using namespace srsgnb;

ue_srb0_scheduler::ue_srb0_scheduler(srslog::basic_logger& logger) : logger(logger) {}

void ue_srb0_scheduler::schedule_srb0(ue_pdsch_allocator& pdsch_alloc, const ue_list& ues)
{

}

void ue_srb0_scheduler::handle_dl_buffer_state_indication(const dl_buffer_state_indication_message& bs)
{
  std::lock_guard<std::mutex> lock(dl_bs_mutex);
  dl_bs.emplace_back(bs);
}
