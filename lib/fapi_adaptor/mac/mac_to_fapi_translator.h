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

#include "srsgnb/mac/mac_cell_result.h"
#include <mutex>

namespace srsgnb {

namespace fapi {
class slot_message_gateway;
} // namespace fapi

namespace fapi_adaptor {

/// \brief Registers the PDSCH PDUs that are added in a DL_TTI.request message.
class pdsch_pdu_registy
{
  using pdu_vector = static_vector<unsigned, MAX_DL_PDUS_PER_SLOT>;

public:
  /// Defines the different types of PDSCH PDUs.
  enum pdsch_pdu_type { sib = 0, rar, ue, last };

  /// \brief Registers a PDU in the register.
  ///
  /// \param fapi_pdu_index Index of the FAPI PDU.
  /// \param type PDSCH PDU type.
  void register_pdu(unsigned fapi_pdu_index, pdsch_pdu_type type)
  {
    srsgnb_assert(type < last, "Invalid PDSCH PDU data type");
    pdus[type].push_back(fapi_pdu_index);
  }

  /// \brief Returns the number of PDUs registered for the given PDU type.
  ///
  /// \param type PDSCH PDU type.
  /// \return Number of PDUs of the given PDU type present in the registry.
  unsigned get_nof_pdus(pdsch_pdu_type type) const
  {
    srsgnb_assert(type < last, "Invalid PDSCH PDU data type");
    return pdus[type].size();
  }

  /// \brief Returns the FAPI PDU index for the given PDU type and MAC PDU index.
  ///
  /// \param mac_pdu_index Index of the MAC PDU.
  /// \param type PDSCH PDU type.
  /// \return Index of the FAPI PDU associated to the MAC PDU.
  unsigned get_fapi_pdu_index(unsigned mac_pdu_index, pdsch_pdu_type type) const
  {
    srsgnb_assert(type < last, "Invalid PDSCH PDU data type");
    srsgnb_assert(mac_pdu_index < pdus[type].size(), "MAC PDU index is not registered");

    return pdus[type][mac_pdu_index];
  }

  /// \brief Resets the registry.
  void reset()
  {
    for (auto& pdu : pdus) {
      pdu.clear();
    }
  }

private:
  std::array<pdu_vector, last> pdus;
};

/// \brief This class listens to MAC cell result events and translates them to FAPI messages that are sent through the
/// \c slot_message_gateway interface.
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
  /// Handling a new slot consists of the following steps:
  /// - Resetting the PDSCH PDU registry.
  ///
  /// \param slot Identifies the new slot.
  /// \note This method is thread safe and may be called from different threads.
  void handle_new_slot()
  {
    std::lock_guard<std::mutex> lock(mutex);
    pdsch_registry.reset();
  }

private:
  fapi::slot_message_gateway& msg_gw;
  pdsch_pdu_registy           pdsch_registry;
  // Protects pdsch_registry.
  //: TODO: make this lock free.
  std::mutex mutex;
};

} // namespace fapi_adaptor
} // namespace srsgnb
