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

/// Parameters relative to a Transport Block transmission.
struct harq_tb_tx_params {
  bool          is_newtx;
  sch_mcs_index mcs;
  unsigned      tbs_bytes;
  unsigned      max_harq_retxs;
};

namespace detail {

/// Basic class for HARQ processes: will be extended by DL and UL HARQ process classes
template <bool IsDownlink>
class harq_process
{
  /// Maximum number of Transport Blocks as per TS38.321, 5.3.2.1 and 5.4.2.1.
  constexpr static size_t MAX_NOF_TBS = IsDownlink ? 2 : 1;

  /// Type used to store the DCI format and RNTI type of the TB stored in the HARQ.
  using dci_rnti_config_type = std::conditional_t<IsDownlink, dci_dl_rnti_config_type, dci_ul_rnti_config_type>;

public:
  struct transport_block {
    /// State of the Transport Block.
    enum class state_t { empty, pending_retx, waiting_ack } state = state_t::empty;

    /// Whether the Transport Block was ACKed.
    bool ack_state = false;

    /// New Data Indicator. Its value should flip for every new Tx.
    bool ndi = false;

    /// Number of retransmissions that took place for the current Transport Block.
    unsigned nof_retxs = 0;

    /// Maximum number of retransmission before Transport Block is reset.
    unsigned max_nof_harq_retxs = 0;

    /// Last MCS used for the TB transmission.
    sch_mcs_index mcs;

    /// TB size in bytes.
    unsigned tbs_bytes;

    bool empty() const { return state == state_t::empty; }
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

  /// \brief Last slot when SCH grant is scheduled for a given TB.
  slot_point slot_tx() const { return last_slot_tx; }

  /// \brief Last slot when SCH grant is scheduled for a given TB.
  slot_point slot_ack() const { return last_slot_ack; }

  /// \brief Last SCH grant frequency resource allocation.
  prb_grant freq_ra() const { return last_prbs; }

  /// \brief DCI format and RNTI type of the TB(s) currently stored in the HARQ.
  dci_rnti_config_type dci_config_type() const { return dci_type; }

  /// \brief Checks whether a specified TB is empty.
  bool empty(unsigned tb_idx) const
  {
    srsgnb_assert(tb_idx < tb_array.size(), "Invalid TB index={}", tb_idx);
    return tb_array[tb_idx].empty();
  }

  /// \brief Checks whether specified TB has pending new_retx.
  bool has_pending_retx(unsigned tb_idx) const
  {
    srsgnb_assert(tb_idx < tb_array.size(), "Invalid TB index ({} >= {})", tb_idx, tb_array.size());
    return tb_array[tb_idx].state == transport_block::state_t::pending_retx;
  }

  unsigned max_nof_harq_retxs(unsigned tb_idx) const
  {
    srsgnb_assert(tb_idx < tb_array.size(), "Invalid TB index ({} >= {})", tb_idx, tb_array.size());
    return tb_array[tb_idx].max_nof_harq_retxs;
  }

  /// \brief Getter for TB parameters.
  const transport_block& tb(unsigned tb_idx) const
  {
    srsgnb_assert(not empty(tb_idx), "TB {} is not active", tb_idx);
    return tb_array[tb_idx];
  }

  /// \brief Updates the ACK state of the HARQ process.
  /// \return The number of bytes of the TB in case of ack==true, zero in case ack==false, and -1 if HARQ is inactive.
  int ack_info(uint32_t tb_idx, bool ack);

  /// \brief Resets HARQ process state.
  void reset();

protected:
  void tx_common(dci_rnti_config_type               dci_type_,
                 bwp_id_t                           bwp_id_,
                 slot_point                         slot_tx,
                 slot_point                         slot_ack,
                 const prb_grant&                   prbs,
                 const optional<harq_tb_tx_params>& tb1_params,
                 const optional<harq_tb_tx_params>& tb2_params = {});
  void tx_tb_common(unsigned tb_idx, const harq_tb_tx_params& tb_params);

  /// \brief Clear the contents of this HARQ process and reset it back to empty state.
  void reset_tb(unsigned tb_idx);

  /// Time interval, in slots, before the HARQ process assumes that the ACK/CRC went missing.
  const unsigned max_ack_wait_in_slots;

  /// Encodes the DCI format and RNTI type used for the HARQ tx.
  dci_rnti_config_type dci_type;

  /// Last BWP used for HARQ TB transmission.
  bwp_id_t bwp_id;

  /// For DL, slot_tx corresponds to the slot when the TB in the HARQ process is going to be transmitted by the gNB.
  /// For UL, slot_tx corresponds to the slot when the TB in the HARQ process is going to be transmitted by the UE.
  slot_point last_slot_tx;

  /// For DL, slot_ack is the slot at which gNB is expected to receive the ACK via UCI.
  /// For UL, slot_ack is the slot when the PUSCH CRC will be processed. It coincides with slot_tx.
  slot_point last_slot_ack;

  /// PRBs used in last HARQ TB transmission.
  prb_grant last_prbs;

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

  /// \brief Checks whether there is at least one TB with pending new_retx.
  bool has_pending_retx() const { return base_type::has_pending_retx(0) or base_type::has_pending_retx(1); }

  /// \brief Called on every new TB transmission, when only one TB is active. It marks this HARQ process as busy and
  /// stores respective TB information.
  void new_tx(dci_dl_rnti_config_type dci_cfg_type,
              bwp_id_t                bwp_id,
              slot_point              pdsch_slot,
              unsigned                k1,
              const prb_grant&        prbs,
              sch_mcs_index           mcs,
              unsigned                tbs_bytes,
              unsigned                max_harq_nof_retxs);

  /// \brief Called on every TB retransmission, when only one TB is active. This function assumes that the HARQ TB is
  /// in pending new_retx state.
  void new_retx(bwp_id_t bwp_id, slot_point pdsch_slot, unsigned k1, const prb_grant& prbs, sch_mcs_index mcs);

  /// \brief Called on every new TB transmission/retransmission, when 2 TBs are enabled.
  void tx_2_tb(dci_dl_rnti_config_type            dci_cfg_type,
               bwp_id_t                           bwp_id,
               slot_point                         pdsch_slot,
               unsigned                           k1,
               const prb_grant&                   prbs,
               const optional<harq_tb_tx_params>& tb1,
               const optional<harq_tb_tx_params>& tb2 = {});
};

class ul_harq_process : private detail::harq_process<false>
{
  using base_type = detail::harq_process<false>;

public:
  using base_type::transport_block;

  using base_type::dci_config_type;
  using base_type::freq_ra;
  using base_type::harq_process;
  using base_type::id;
  using base_type::reset;
  using base_type::slot_ack;
  using base_type::slot_indication;
  using base_type::slot_tx;

  /// \brief Checks whether the HARQ process is inactive.
  bool empty() const { return base_type::empty(0); }

  /// \brief Checks whether the UL HARQ process has a pending new_retx.
  bool has_pending_retx() const { return base_type::has_pending_retx(0); }

  /// \brief Gets the maximum number of HARQ retxs.
  unsigned max_nof_harq_retxs() const { return base_type::max_nof_harq_retxs(0); }

  /// \brief Getter of TB params.
  const transport_block& tb() const { return base_type::tb(0); }

  /// \brief Processes CRC info for the UL HARQ process.
  /// \return The number of bytes of the TB in case of ack==true, zero in case ack==false, and -1 if HARQ is inactive.
  int crc_info(bool ack) { return base_type::ack_info(0, ack); }

  /// \brief Called on every new transmission. It marks this HARQ process as busy and stores respective TB information.
  void new_tx(dci_ul_rnti_config_type dci_cfg_type,
              bwp_id_t                bwp_id,
              slot_point              pusch_slot,
              const prb_grant&        prbs,
              sch_mcs_index           mcs_idx,
              unsigned                tbs_bytes,
              unsigned                max_harq_retxs);

  /// \brief Called on every TB retransmission. This function assumes that the HARQ TB is in pending new_retx state.
  void new_retx(bwp_id_t bwp_id, slot_point pusch_slot, const prb_grant& prbs, sch_mcs_index mcs_idx);
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
