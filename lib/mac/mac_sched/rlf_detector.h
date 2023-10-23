/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/mac/mac_ue_configurator.h"
#include "srsran/ran/du_types.h"
#include "srsran/support/srsran_assert.h"
#include <array>
#include <atomic>

namespace srsran {

/// \brief Detector of RLFs in the MAC, based on HARQ-ACK and CRC indications.
class rlf_detector
{
public:
  rlf_detector(unsigned max_consecutive_dl_kos, unsigned max_consecutive_ul_kos) :
    max_consecutive_kos({max_consecutive_dl_kos, max_consecutive_ul_kos}), logger(srslog::fetch_basic_logger("MAC"))
  {
  }

  void add_ue(du_ue_index_t ue_index, mac_ue_radio_link_notifier& notifier)
  {
    srsran_assert(ue_index < MAX_NOF_DU_UES, "Invalid ue_index={}", ue_index);

    ues[ue_index].ko_counters[0] = 0;
    ues[ue_index].ko_counters[1] = 0;
    std::lock_guard<std::mutex> lock(ues[ue_index].notifier_mutex);
    ues[ue_index].notifier = &notifier;
  }

  void rem_ue(du_ue_index_t ue_index)
  {
    srsran_assert(ue_index < MAX_NOF_DU_UES, "Invalid ue_index={}", ue_index);

    ues[ue_index].ko_counters[0] = max_consecutive_kos[0] + 1;
    ues[ue_index].ko_counters[1] = max_consecutive_kos[1] + 1;
    std::lock_guard<std::mutex> lock(ues[ue_index].notifier_mutex);
    ues[ue_index].notifier = nullptr;
  }

  void handle_ack(du_ue_index_t ue_index, bool ack) { handle_ack_common(ue_index, ack, 0); }

  void handle_crc(du_ue_index_t ue_index, bool crc) { handle_ack_common(ue_index, crc, 1); }

  void handle_crnti_ce(du_ue_index_t ue_index)
  {
    ues[ue_index].ko_counters[0] = 0;
    ues[ue_index].ko_counters[1] = 0;
    std::lock_guard<std::mutex> lock(ues[ue_index].notifier_mutex);
    if (ues[ue_index].notifier != nullptr) {
      ues[ue_index].notifier->on_crnti_ce_received();
    }
  }

private:
  void handle_ack_common(du_ue_index_t ue_index, bool ack, unsigned count_index)
  {
    srsran_assert(ue_index < MAX_NOF_DU_UES, "Invalid ue_index={}", ue_index);
    auto& u = ues[ue_index];

    if (ack) {
      u.ko_counters[count_index].store(0, std::memory_order::memory_order_relaxed);
    } else {
      unsigned current_count = u.ko_counters[count_index].fetch_add(1, std::memory_order::memory_order_relaxed) + 1;
      // Note: We use == instead of <= to ensure only one notification is sent.
      if (current_count == max_consecutive_kos[count_index]) {
        std::lock_guard<std::mutex> lock(u.notifier_mutex);
        if (u.notifier != nullptr) {
          logger.info("ue={}: RLF detected. Cause: {} consecutive {} KOs.",
                      ue_index,
                      current_count,
                      count_index == 0 ? "HARQ-ACK" : "CRC");

          // Notify upper layers.
          u.notifier->on_rlf_detected();
        }
      }
    }
  }

  // DL for index 0 and UL for index 1.
  std::array<const unsigned, 2> max_consecutive_kos;
  srslog::basic_logger&         logger;

  struct ue_context {
    std::array<std::atomic<unsigned>, 2> ko_counters{};
    mac_ue_radio_link_notifier*          notifier = nullptr;
    // Access to the notifier is protected, as many threads may access it.
    std::mutex notifier_mutex;
  };

  std::array<ue_context, MAX_NOF_DU_UES> ues;
};

} // namespace srsran