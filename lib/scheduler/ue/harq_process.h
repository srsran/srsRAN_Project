/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SCHED_HARQ_H
#define SCHED_HARQ_H

#include "../scheduler_prb.h"
#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/ran/slot_point.h"
#include "srsgnb/scheduler/sched_consts.h"
#include "srsgnb/scheduler/scheduler_dci.h"
#include <array>

namespace srsgnb {

/// Basic class for HARQ processes: will be extended by DL and UL HARQ process classes
class harq_process
{
public:
  explicit harq_process(uint32_t id_) : pid(id_) {}

  /// \brief Indicate the beginning of a new slot.
  void slot_indication(slot_point slot_rx);

  /// \brief Clear the contents of this HARQ process and reset it back to empty state.
  void reset();

  bool set_tbs(uint32_t tbs);

  // NOTE: Has to be used before first tx is dispatched
  bool set_mcs(uint32_t mcs);

  /// Checks whether the HARQ process has no Transport Block currently stored.
  bool empty() const
  {
    return std::all_of(tb.begin(), tb.end(), [](const tb_t& t) { return t.state == tb_t::state_t::empty; });
  }
  bool empty(unsigned tb_idx) const { return tb[tb_idx].state == tb_t::state_t::empty; }

  /// \brief Checks whether the TB has not been positively ACKED yet.
  bool has_pending_retx() const
  {
    return std::any_of(tb.begin(), tb.end(), [](const tb_t& t) { return t.state == tb_t::state_t::pending_retx; });
  }

  bool has_pending_retx(unsigned tb_idx) const { return tb[tb_idx].state == tb_t::state_t::pending_retx; }

  /// \brief Number of times the currently stored TB has been retransmitted.
  uint32_t nof_retx() const { return tb[0].n_rtx; }

  /// \brief Current maximum value of retransmissions of a single TB.
  uint32_t         max_nof_retx() const { return max_retx; }
  uint32_t         tbs(unsigned tb_index) const { return tb[tb_index].tbs; }
  bool             ndi(unsigned tb_index) const { return tb[tb_index].ndi; }
  uint32_t         mcs(unsigned tb_index) const { return tb[tb_index].mcs; }
  const prb_grant& prbs() const { return prbs_; }
  slot_point       harq_slot_tx() const { return slot_tx; }
  slot_point       harq_slot_ack() const { return slot_ack; }

  int ack_info(uint32_t tb_idx, bool ack);

  const uint32_t pid;

protected:
  bool new_tx_common(slot_point slot_tx, slot_point slot_ack, const prb_grant& grant, uint32_t mcs, uint32_t max_retx);
  bool new_retx_common(slot_point slot_tx, slot_point slot_ack, const prb_grant& grant);
  bool new_retx_common(slot_point slot_tx, slot_point slot_ack);

  struct tb_t {
    enum class state_t { empty, pending_retx, waiting_ack } state = state_t::empty;
    bool     ack_state                                            = false;
    bool     ndi                                                  = false;
    uint32_t n_rtx                                                = 0;
    uint32_t mcs                                                  = 0;
    uint32_t tbs                                                  = 0;
  };

  uint32_t max_retx = 1;
  /// For DL, slot_tx corresponds to the slot when the TB in the HARQ process is going to be transmitted by the gNB.
  /// For UL, slot_tx corresponds to the slot when the TB in the HARQ process is going to be transmitted by the UE.
  slot_point slot_tx;
  /// For DL, slot_ack is the slot at which gNB is expected to receive the ACK via UCI.
  /// For UL, slot_ack is the slot when the PUSCH CRC will be processed. It coincides with slot_tx.
  slot_point                          slot_ack;
  prb_grant                           prbs_;
  static_vector<tb_t, MAX_NOF_LAYERS> tb{1};
};

class dl_harq_process : public harq_process
{
public:
  explicit dl_harq_process(uint32_t id_);

  /// \brief Called on every new transmission. It marks this HARQ process as busy and stores respective TB
  /// information.
  bool new_tx(slot_point slot_tx, slot_point slot_ack, const prb_grant& grant, uint32_t mcs, uint32_t max_retx);

  /// \brief Called on every HARQ retransmission. It updates the HARQ internal state accordingly.
  bool new_retx(slot_point slot_tx, slot_point slot_ack, const prb_grant& grant);
};

class ul_harq_process : public harq_process
{
public:
  explicit ul_harq_process(uint32_t id_) : harq_process(id_) {}

  /// \brief Called on every new transmission. It marks this HARQ process as busy and stores respective TB
  /// information.
  bool new_tx(slot_point slot_tx, const prb_grant& grant, uint32_t mcs, uint32_t max_retx);

  /// \brief Called on every HARQ retransmission. It updates the HARQ internal state accordingly.
  bool new_retx(slot_point slot_tx, const prb_grant& grant);
};

/// This object handles the DL and UL HARQ processes for each UE's Component Carrier; therefore, for each UE, we
/// instantiate as many harq_entity object as CCs
class harq_entity
{
public:
  explicit harq_entity(rnti_t rnti, uint32_t nprb, uint32_t nof_harq_procs, srslog::basic_logger& logger);

  /// Update slot, and checks if there are HARQ processes that have reached maxReTx with no ACK
  void slot_indication(slot_point slot_rx_);

  int dl_ack_info(uint32_t pid, uint32_t tb_idx, bool ack) { return dl_harqs[pid].ack_info(tb_idx, ack); }
  int ul_crc_info(uint32_t pid, bool ack) { return ul_harqs[pid].ack_info(0, ack); }

  uint32_t               nof_dl_harqs() const { return dl_harqs.size(); }
  uint32_t               nof_ul_harqs() const { return ul_harqs.size(); }
  const dl_harq_process& dl_harq(uint32_t pid) const { return dl_harqs[pid]; }
  const ul_harq_process& ul_harq(uint32_t pid) const { return ul_harqs[pid]; }

  dl_harq_process* find_pending_dl_retx()
  {
    return find_dl([](const dl_harq_process& h) { return h.has_pending_retx(); });
  }
  ul_harq_process* find_pending_ul_retx()
  {
    return find_ul([](const ul_harq_process& h) { return h.has_pending_retx(); });
  }
  dl_harq_process* find_empty_dl_harq()
  {
    return find_dl([](const dl_harq_process& h) { return h.empty(); });
  }
  ul_harq_process* find_empty_ul_harq()
  {
    return find_ul([](const ul_harq_process& h) { return h.empty(); });
  }

private:
  template <typename Predicate>
  dl_harq_process* find_dl(Predicate p)
  {
    auto it = std::find_if(dl_harqs.begin(), dl_harqs.end(), p);
    return (it == dl_harqs.end()) ? nullptr : &(*it);
  }
  template <typename Predicate>
  ul_harq_process* find_ul(Predicate p)
  {
    auto it = std::find_if(ul_harqs.begin(), ul_harqs.end(), p);
    return (it == ul_harqs.end()) ? nullptr : &(*it);
  }

  rnti_t                rnti;
  srslog::basic_logger& logger;

  // slot_rx is the slot index at which the scheduler is currently working
  slot_point                   slot_rx;
  std::vector<dl_harq_process> dl_harqs;
  std::vector<ul_harq_process> ul_harqs;
};

} // namespace srsgnb

#endif // SCHED_HARQ_H
