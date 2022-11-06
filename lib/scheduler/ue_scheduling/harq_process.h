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
#include "srsgnb/scheduler/config/bwp_configuration.h"
#include "srsgnb/scheduler/harq_id.h"
#include "srsgnb/scheduler/sched_consts.h"
#include "srsgnb/scheduler/scheduler_dci.h"

namespace srsgnb {

/// PDSCH-specific parameters stored in DL HARQ process.
struct dl_harq_info_params {
  dci_dl_rnti_config_type dci_cfg_type;
  bwp_id_t                bwp_id;
  prb_grant               prbs;
  sch_mcs_index           mcs;
  unsigned                tbs_bytes;
};

/// PUSCH-specific parameters stored in UL HARQ process.
struct ul_harq_info_params {
  dci_ul_rnti_config_type dci_cfg_type;
  bwp_id_t                bwp_id;
  prb_grant               prbs;
  sch_mcs_index           mcs;
  unsigned                tbs_bytes;
};

namespace detail {

/// Basic class for HARQ processes: will be extended by DL and UL HARQ process classes
template <bool IsDownlink>
class harq_process
{
  /// Maximum number of Transport Blocks as per TS38.321, 5.3.2.1 and 5.4.2.1.
  constexpr static size_t MAX_NOF_TBS = IsDownlink ? 2 : 1;

  /// Parameters that can be stored in the HARQ process and used across retxs.
  using grant_params = std::conditional_t<IsDownlink, dl_harq_info_params, ul_harq_info_params>;

public:
  struct base_transport_block {
    /// State of the Transport Block.
    enum class state_t { empty, pending_retx, waiting_ack } state = state_t::empty;

    /// Whether the Transport Block was ACKed.
    bool ack_state = false;

    /// New Data Indicator. Its value should flip for every new Tx.
    bool ndi = false;

    /// For DL, slot_tx corresponds to the slot when the TB in the HARQ process is going to be transmitted by the gNB.
    /// For UL, slot_tx corresponds to the slot when the TB in the HARQ process is going to be transmitted by the UE.
    slot_point slot_tx;

    /// For DL, slot_ack is the slot at which gNB is expected to receive the ACK via UCI.
    /// For UL, slot_ack is the slot when the PUSCH CRC will be processed. It coincides with slot_tx.
    slot_point slot_ack;

    /// Number of retransmissions that took place for the current Transport Block.
    uint32_t nof_retxs = 0;

    bool empty() const { return state == state_t::empty; }
  };

  struct transport_block : public base_transport_block {
    using state_t = typename base_transport_block::state_t;

    /// Grant parameters used for the last transport block transmission.
    grant_params grant = {};
  };

  /// HARQ process id.
  const harq_id_t id;

  /// \brief HARQ process constructor.
  /// \param h_id HARQ process ID.
  /// \param max_ack_wait_in_slots_ number of slots above which the scheduler considers that the ACK/CRC went missing.
  explicit harq_process(harq_id_t h_id, unsigned max_ack_wait_in_slots_ = 8) :
    id(h_id), max_ack_wait_in_slots(max_ack_wait_in_slots_)
  {
  }

  /// \brief Indicate the beginning of a new slot.
  void slot_indication(slot_point slot_tx);

  /// \brief Checks whether a specified TB is empty.
  bool empty(unsigned tb_idx) const
  {
    srsgnb_assert(tb_idx < tb_array.size(), "Invalid TB index={}", tb_idx);
    return tb_array[tb_idx].empty();
  }

  /// \brief Checks whether specified TB has pending retx.
  bool has_pending_retx(unsigned tb_idx) const
  {
    srsgnb_assert(tb_idx < tb_array.size(), "Invalid TB index ({} >= {})", tb_idx, tb_array.size());
    return tb_array[tb_idx].state == transport_block::state_t::pending_retx;
  }

  /// \brief Current maximum value of retransmissions of a single TB.
  uint32_t max_nof_retxs() const { return max_retx; }

  /// \brief Access to the parameters used for the last TB transmission.
  const grant_params& last_tx_params(unsigned tb_idx) const
  {
    srsgnb_assert(not empty(tb_idx), "TB {} is not active", tb_idx);
    return tb_array[tb_idx].grant;
  }

  /// \brief New Data Indication for a given TB.
  bool ndi(unsigned tb_idx) const
  {
    srsgnb_assert(not empty(tb_idx), "TB {} is not active", tb_idx);
    return tb_array[tb_idx].ndi;
  }

  /// \brief Last slot when SCH grant is scheduled for a given TB.
  slot_point slot_tx(unsigned tb_idx) const
  {
    srsgnb_assert(not empty(tb_idx), "TB {} is not active", tb_idx);
    return tb_array[tb_idx].slot_tx;
  }

  /// \brief Last slot when SCH grant is scheduled for a given TB.
  slot_point slot_ack(unsigned tb_idx) const
  {
    srsgnb_assert(not empty(tb_idx), "TB {} is not active", tb_idx);
    return tb_array[tb_idx].slot_ack;
  }

  /// \brief Number of ReTxs performed for a given TB.
  unsigned nof_retxs(unsigned tb_idx) const
  {
    srsgnb_assert(not empty(tb_idx), "TB {} is not active", tb_idx);
    return tb_array[tb_idx].nof_retxs;
  }

  /// \brief Clear the contents of this HARQ process and reset it back to empty state.
  void reset(unsigned tb_idx);

  /// \brief Updates the ACK state of the HARQ process.
  /// \return The number of bytes of the TB in case of ack==true, zero in case ack==false, and -1 if HARQ is inactive.
  int ack_info(uint32_t tb_idx, bool ack);

protected:
  grant_params* new_tx_common(unsigned tb_idx, slot_point slot_tx, slot_point slot_ack, unsigned max_harq_retxs);
  grant_params* new_retx_common(unsigned tb_idx, slot_point slot_tx, slot_point slot_ack);

  const unsigned max_ack_wait_in_slots;

  /// Maximum number of retxs for the current TB.
  unsigned max_retx = 1;

  std::array<transport_block, MAX_NOF_TBS> tb_array;
};

} // namespace detail

class dl_harq_process : public detail::harq_process<true>
{
  using base_type = detail::harq_process<true>;

public:
  using base_type::transport_block;

  using base_type::empty;
  using base_type::harq_process;
  using base_type::has_pending_retx;

  /// \brief Checks whether the HARQ process has no TBs currently active.
  bool empty() const { return base_type::empty(0) and base_type::empty(1); }

  /// \brief Checks whether there is at least one TB with pending retx.
  bool has_pending_retx() const { return base_type::has_pending_retx(0) or base_type::has_pending_retx(1); }

  /// \brief Clear the contents of all TBs of this HARQ process and reset it back to empty state.
  void reset()
  {
    base_type::reset(0);
    base_type::reset(1);
  }

  /// \brief Called on every new transmission. It marks this HARQ process as busy and stores respective TB
  /// information.
  dl_harq_info_params* new_tx(unsigned tb_idx, slot_point pdsch_slot, unsigned k1, unsigned max_harq_retxs);

  /// \brief Called on every HARQ retransmission. It updates the HARQ internal state accordingly.
  dl_harq_info_params* new_retx(unsigned tb_idx, slot_point pdsch_slot, unsigned k1);
};

class ul_harq_process : private detail::harq_process<false>
{
  using base_type = detail::harq_process<false>;

public:
  using base_type::transport_block;

  using base_type::harq_process;
  using base_type::id;
  using base_type::max_nof_retxs;
  using base_type::slot_indication;

  /// \brief Checks whether the HARQ process is inactive.
  bool empty() const { return base_type::empty(0); }

  /// \brief Checks whether the UL HARQ process has a pending retx.
  bool has_pending_retx() const { return base_type::has_pending_retx(0); }

  /// \brief Number of ReTxs performed for the UL HARQ process.
  unsigned nof_retxs() const { return base_type::nof_retxs(0); }

  /// \brief Access to the parameters used for the last TB transmission.
  const ul_harq_info_params& last_tx_params() const { return base_type::last_tx_params(0); }

  /// \brief New Data Indication for the UL HARQ.
  bool ndi() const { return base_type::ndi(0); }

  /// \brief Last slot when PUSCH grant was scheduled.
  slot_point slot_tx() const { return base_type::slot_tx(0); }

  /// \brief Last slot when PUSCH grant was scheduled.
  slot_point slot_ack() const { return base_type::slot_ack(0); }

  /// \brief Processes CRC info for the UL HARQ process.
  /// \return The number of bytes of the TB in case of ack==true, zero in case ack==false, and -1 if HARQ is inactive.
  int crc_info(bool ack) { return base_type::ack_info(0, ack); }

  /// \brief Processes CRC info for the UL HARQ process.
  /// \return The number of bytes of the TB in case of ack==true, zero in case ack==false, and -1 if HARQ is inactive.
  void reset() { base_type::reset(0); }

  /// \brief Called on every new transmission. It marks this HARQ process as busy and stores respective TB
  /// information.
  ul_harq_info_params* new_tx(slot_point pusch_slot, unsigned max_harq_retxs);

  /// \brief Called on every HARQ retransmission. It updates the HARQ internal state accordingly.
  ul_harq_info_params* new_retx(slot_point pusch_slot);
};

/// This object handles the DL and UL HARQ processes for each UE's Component Carrier; therefore, for each UE, we
/// instantiate as many harq_entity object as CCs
class harq_entity
{
public:
  explicit harq_entity(rnti_t rnti, uint32_t nof_harq_procs, unsigned max_ack_wait_in_slots = 8);

  /// Update slot, and checks if there are HARQ processes that have reached maxReTx with no ACK
  void slot_indication(slot_point slot_tx_);

  int dl_ack_info(uint32_t pid, uint32_t tb_idx, bool ack) { return dl_harqs[pid].ack_info(tb_idx, ack); }
  int ul_crc_info(uint32_t pid, bool ack) { return ul_harqs[pid].crc_info(ack); }

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
