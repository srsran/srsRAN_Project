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

#include "srsgnb/adt/static_vector.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/ran/sch_mcs.h"
#include "srsgnb/ran/slot_point.h"
#include "srsgnb/scheduler/harq_id.h"
#include "srsgnb/scheduler/sched_consts.h"
#include "srsgnb/scheduler/scheduler_dci.h"
#include <array>

namespace srsgnb {

/// Basic class for HARQ processes: will be extended by DL and UL HARQ process classes
class harq_process
{
public:
  /// \brief HARQ process constructor.
  /// \param h_id HARQ process ID.
  /// \param max_ack_wait_in_slots_ number of slots above which the scheduler considers that the ACK/CRC went missing.
  explicit harq_process(harq_id_t h_id, unsigned max_ack_wait_in_slots_ = 4) :
    id(h_id), max_ack_wait_in_slots(max_ack_wait_in_slots_)
  {
  }

  /// \brief Indicate the beginning of a new slot.
  void slot_indication(slot_point slot_tx);

  /// \brief Clear the contents of this HARQ process and reset it back to empty state.
  void reset();

  bool set_tbs(uint32_t tbs);

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
  uint32_t max_nof_retx() const { return max_retx; }
  uint32_t tbs(unsigned tb_index) const { return tb[tb_index].tbs; }
  uint32_t tbs() const
  {
    unsigned sum_bytes = 0;
    for (const auto& t : tb) {
      sum_bytes += t.tbs;
    }
    return sum_bytes;
  }
  bool             ndi(unsigned tb_index) const { return tb[tb_index].ndi; }
  sch_mcs_index    mcs(unsigned tb_index) const { return tb[tb_index].mcs; }
  const prb_grant& prbs() const { return prbs_; }
  slot_point       harq_slot_tx() const { return slot_tx; }
  slot_point       harq_slot_ack() const { return slot_ack; }

  /// \brief Updates the ACK state of the HARQ process.
  /// \return The number of bytes of the TB in case of ack==true, zero in case ack==false, and -1 if HARQ is inactive.
  int ack_info(uint32_t tb_idx, bool ack);

  /// HARQ process id.
  const harq_id_t id;

protected:
  bool
  new_tx_common(slot_point slot_tx, slot_point slot_ack, const prb_grant& grant, sch_mcs_index mcs, uint32_t max_retx);
  bool new_retx_common(slot_point slot_tx, slot_point slot_ack, const prb_grant& grant);
  bool new_retx_common(slot_point slot_tx, slot_point slot_ack);

  struct tb_t {
    enum class state_t { empty, pending_retx, waiting_ack } state = state_t::empty;
    bool          ack_state                                       = false;
    bool          ndi                                             = false;
    uint32_t      n_rtx                                           = 0;
    sch_mcs_index mcs                                             = 0;
    uint32_t      tbs                                             = 0;
  };

  const unsigned max_ack_wait_in_slots;

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
  using harq_process::harq_process;

  /// \brief Called on every new transmission. It marks this HARQ process as busy and stores respective TB
  /// information.
  bool new_tx(slot_point slot_tx, slot_point slot_ack, const prb_grant& grant, sch_mcs_index mcs, uint32_t max_retx);

  /// \brief Called on every HARQ retransmission. It updates the HARQ internal state accordingly.
  bool new_retx(slot_point slot_tx, slot_point slot_ack, const prb_grant& grant);
};

class ul_harq_process : public harq_process
{
public:
  using harq_process::harq_process;

  /// \brief Called on every new transmission. It marks this HARQ process as busy and stores respective TB
  /// information.
  bool new_tx(slot_point slot_tx, const prb_grant& grant, sch_mcs_index mcs, uint32_t max_retx);

  /// \brief Called on every HARQ retransmission. It updates the HARQ internal state accordingly.
  bool new_retx(slot_point slot_tx, const prb_grant& grant);
};

/// This object handles the DL and UL HARQ processes for each UE's Component Carrier; therefore, for each UE, we
/// instantiate as many harq_entity object as CCs
class harq_entity
{
public:
  explicit harq_entity(rnti_t rnti, uint32_t nprb, uint32_t nof_harq_procs, unsigned max_ack_wait_in_slots = 4);

  /// Update slot, and checks if there are HARQ processes that have reached maxReTx with no ACK
  void slot_indication(slot_point slot_tx_);

  int dl_ack_info(uint32_t pid, uint32_t tb_idx, bool ack) { return dl_harqs[pid].ack_info(tb_idx, ack); }
  int ul_crc_info(uint32_t pid, bool ack) { return ul_harqs[pid].ack_info(0, ack); }

  uint32_t               nof_dl_harqs() const { return dl_harqs.size(); }
  uint32_t               nof_ul_harqs() const { return ul_harqs.size(); }
  const dl_harq_process& dl_harq(uint32_t pid) const { return dl_harqs[pid]; }
  dl_harq_process&       dl_harq(uint32_t pid) { return dl_harqs[pid]; }
  const ul_harq_process& ul_harq(uint32_t pid) const { return ul_harqs[pid]; }
  ul_harq_process&       ul_harq(uint32_t pid) { return ul_harqs[pid]; }

  dl_harq_process* find_pending_dl_retx()
  {
    return find_dl([](const dl_harq_process& h) { return h.has_pending_retx(); });
  }
  const dl_harq_process* find_pending_dl_retx() const
  {
    return find_dl([](const dl_harq_process& h) { return h.has_pending_retx(); });
  }
  ul_harq_process* find_pending_ul_retx()
  {
    return find_ul([](const ul_harq_process& h) { return h.has_pending_retx(); });
  }
  const ul_harq_process* find_pending_ul_retx() const
  {
    return find_ul([](const ul_harq_process& h) { return h.has_pending_retx(); });
  }
  dl_harq_process* find_empty_dl_harq()
  {
    return find_dl([](const dl_harq_process& h) { return h.empty(); });
  }
  const dl_harq_process* find_empty_dl_harq() const
  {
    return find_dl([](const dl_harq_process& h) { return h.empty(); });
  }
  ul_harq_process* find_empty_ul_harq()
  {
    return find_ul([](const ul_harq_process& h) { return h.empty(); });
  }
  const ul_harq_process* find_empty_ul_harq() const
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
  const dl_harq_process* find_dl(Predicate p) const
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
  template <typename Predicate>
  const ul_harq_process* find_ul(Predicate p) const
  {
    auto it = std::find_if(ul_harqs.begin(), ul_harqs.end(), p);
    return (it == ul_harqs.end()) ? nullptr : &(*it);
  }

  rnti_t                rnti;
  srslog::basic_logger& logger;

  // slot_rx is the slot index at which the scheduler is currently working
  slot_point                   slot_tx;
  std::vector<dl_harq_process> dl_harqs;
  std::vector<ul_harq_process> ul_harqs;
};

} // namespace srsgnb
