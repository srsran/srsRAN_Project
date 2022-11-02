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

#include "srsgnb/adt/circular_array.h"
#include "srsgnb/mac/mac_cell_result.h"
#include <mutex>

namespace srsgnb {

namespace fapi {
class slot_message_gateway;
} // namespace fapi

namespace fapi_adaptor {

/// \brief PDSCH PDU registry.
///
/// This class stores the index positions of the PDSCH PDUs carried in the FAPI DL_TTI.request.
class pdsch_pdu_registry
{
  /// Maximum number of PDUs that the registry will contain. It is multiplied by 2 because it also stores the CW index
  /// and a PUSCH PDU can contain up to 2 CW.
  static constexpr size_t MAX_PDUS =
      std::max({MAX_SIB1_PDUS_PER_SLOT, MAX_RAR_PDUS_PER_SLOT, MAX_UE_PDUS_PER_SLOT}) * 2U;

public:
  /// Labels for the different types of PDSCH PDUs.
  enum pdsch_pdu_type { sib = 0, rar, ue, last };

  struct pdu_struct {
    unsigned fapi_index;
    unsigned cw_index;
  };

  /// \brief Records a PDU in the registry.
  ///
  /// \param[in] fapi_pdu_index Index of the FAPI PDU.
  /// \param[in] cw_index Codeword index.
  /// \param[in] type PDSCH PDU type.
  void register_pdu(unsigned fapi_pdu_index, unsigned cw_index, pdsch_pdu_type type)
  {
    srsgnb_assert(type < last, "Invalid PDSCH PDU data type");
    pdus[type].push_back({fapi_pdu_index, cw_index});
  }

  /// \brief Returns the number of recorded PDUs of the given type.
  ///
  /// \param[in] type PDSCH PDU type.
  /// \return Number of PDUs of the given type present in the registry.
  unsigned get_nof_pdus(pdsch_pdu_type type) const
  {
    srsgnb_assert(type < last, "Invalid PDSCH PDU data type");
    return pdus[type].size();
  }

  /// \brief Returns the FAPI PDU index for the given PDU type and MAC PDU index.
  ///
  /// \param[in] mac_pdu_index Index of the MAC PDU.
  /// \param[in] type          PDSCH PDU type.
  /// \return A PDU struct that contains the index of the FAPI PDU and the codeword index.
  const pdu_struct& get_fapi_pdu_index(unsigned mac_pdu_index, pdsch_pdu_type type) const
  {
    srsgnb_assert(type < last, "Invalid PDSCH PDU data type");
    srsgnb_assert(mac_pdu_index < pdus[type].size(), "MAC PDU index is not registered");

    return pdus[type][mac_pdu_index];
  }

  /// \brief Resets the registry.
  void reset() { pdus = {}; }

private:
  using pdu_vector = static_vector<pdu_struct, MAX_PDUS>;
  std::array<pdu_vector, last> pdus;
};

/// \brief MAC-to-FAPI translator.
///
/// This class listens to cell-specific MAC events carrying scheduling results and translates them into FAPI messages
/// that are sent through the FAPI message gateway.
class mac_to_fapi_translator : public mac_cell_result_notifier
{
public:
  explicit mac_to_fapi_translator(fapi::slot_message_gateway& msg_gw) : msg_gw(msg_gw) {}

  // See interface for documentation.
  void on_new_downlink_scheduler_results(const mac_dl_sched_result& dl_res) override;

  // See interface for documentation.
  void on_new_downlink_data(const mac_dl_data_result& dl_data) override;

  // See interface for documentation.
  void on_new_uplink_scheduler_results(const mac_ul_sched_result& ul_res) override;

  /// \brief Handles a new slot.
  ///
  /// When a new slot is notified, the PDSCH PDUs from the previous slot should be discarded and, in turn, the PDSCH PDU
  /// registry has to be reset.
  ///
  /// \param[in] slot Identifies the new slot.
  /// \note This method is thread safe and may be called from different threads.
  void handle_new_slot()
  {
    std::lock_guard<std::mutex> lock(mutex);
    pdsch_registry.reset();
  }

private:
  /// Adds the given PDCCH PDUS to the uplink DCI request.
  void add_pdcch_pdus_to_ul_dci_request(span<const pdcch_ul_information> ul_pdcch,
                                        span<const dci_payload>          payload,
                                        slot_point                       slot);

private:
  /// FAPI message gateway to the outside world.
  fapi::slot_message_gateway& msg_gw;
  /// PDSCH PDU registry helper object.
  pdsch_pdu_registry pdsch_registry;
  // Protects pdsch_registry.
  //: TODO: make this lock free.
  std::mutex mutex;
};

} // namespace fapi_adaptor
} // namespace srsgnb
