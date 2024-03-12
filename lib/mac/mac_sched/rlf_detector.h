/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/mac/mac_config.h"
#include "srsran/mac/mac_ue_configurator.h"
#include "srsran/ran/du_types.h"
#include "srsran/support/srsran_assert.h"
#include <array>
#include <atomic>
#include <mutex>

namespace srsran {

/// \brief Metrics used as reference to detect/trigger RLF.
struct rlf_metrics {
  unsigned max_consecutive_dl_kos;
  unsigned max_consecutive_ul_kos;
  unsigned max_consecutive_csi_dtx;
};

/// \brief Detector of RLFs in the MAC, based on HARQ-ACK and CRC indications.
class rlf_detector
{
public:
  rlf_detector(const mac_expert_config& expert_cfg) : logger(srslog::fetch_basic_logger("MAC"))
  {
    max_consecutive_kos.resize(expert_cfg.configs.size());
    for (auto mac_cfg_it = expert_cfg.configs.begin(); mac_cfg_it != expert_cfg.configs.end(); ++mac_cfg_it) {
      max_consecutive_kos[std::distance(expert_cfg.configs.begin(), mac_cfg_it)] =
          rlf_metrics{.max_consecutive_dl_kos  = mac_cfg_it->max_consecutive_dl_kos,
                      .max_consecutive_ul_kos  = mac_cfg_it->max_consecutive_ul_kos,
                      .max_consecutive_csi_dtx = mac_cfg_it->max_consecutive_csi_dtx};
    }
  }

  void add_ue(du_ue_index_t ue_index, mac_ue_radio_link_notifier& notifier)
  {
    srsran_assert(ue_index < MAX_NOF_DU_UES, "Invalid ue_index={}", ue_index);

    ues[ue_index].ko_counters[0] = 0;
    ues[ue_index].ko_counters[1] = 0;
    ues[ue_index].ko_counters[2] = 0;
    std::lock_guard<std::mutex> lock(ues[ue_index].notifier_mutex);
    ues[ue_index].notifier = &notifier;
  }

  void rem_ue(du_ue_index_t ue_index, du_cell_index_t cell_index)
  {
    srsran_assert(ue_index < MAX_NOF_DU_UES, "Invalid ue_index={}", ue_index);

    ues[ue_index].ko_counters[0] = max_consecutive_kos[cell_index].max_consecutive_dl_kos + 1;
    ues[ue_index].ko_counters[1] = max_consecutive_kos[cell_index].max_consecutive_ul_kos + 1;
    ues[ue_index].ko_counters[2] = max_consecutive_kos[cell_index].max_consecutive_csi_dtx + 1;
    std::lock_guard<std::mutex> lock(ues[ue_index].notifier_mutex);
    ues[ue_index].notifier = nullptr;
  }

  void handle_ack(du_ue_index_t ue_index, du_cell_index_t cell_index, bool ack)
  {
    handle_ack_common(ue_index, cell_index, ack, /* is_dl=*/true);
  }

  void handle_crc(du_ue_index_t ue_index, du_cell_index_t cell_index, bool crc)
  {
    handle_ack_common(ue_index, cell_index, crc, /* is_dl=*/false);
  }

  void handle_csi(du_ue_index_t ue_index, du_cell_index_t cell_index, bool csi_decoded)
  {
    srsran_assert(ue_index < MAX_NOF_DU_UES, "Invalid ue_index={}", ue_index);
    auto& u = ues[ue_index];

    const unsigned csi_index = 2U;

    if (csi_decoded) {
      u.ko_counters[csi_index].store(0, std::memory_order::memory_order_relaxed);
    } else {
      unsigned current_count = u.ko_counters[csi_index].fetch_add(1, std::memory_order::memory_order_relaxed) + 1;
      // Note: We use == instead of <= to ensure only one notification is sent.
      if (current_count == max_consecutive_kos[cell_index].max_consecutive_csi_dtx) {
        std::lock_guard<std::mutex> lock(u.notifier_mutex);
        if (u.notifier != nullptr) {
          logger.warning("ue={}: RLF detected. Cause: {} consecutive undecoded CSIs", ue_index, current_count);

          // Notify upper layers.
          u.notifier->on_rlf_detected();
        }
      }
    }
  }

  void handle_crnti_ce(du_ue_index_t ue_index)
  {
    ues[ue_index].ko_counters[0] = 0;
    ues[ue_index].ko_counters[1] = 0;
    ues[ue_index].ko_counters[2] = 0;
    std::lock_guard<std::mutex> lock(ues[ue_index].notifier_mutex);
    if (ues[ue_index].notifier != nullptr) {
      ues[ue_index].notifier->on_crnti_ce_received();
    }
  }

private:
  void handle_ack_common(du_ue_index_t ue_index, du_cell_index_t cell_index, bool ack, bool is_dl)
  {
    srsran_assert(ue_index < MAX_NOF_DU_UES, "Invalid ue_index={}", ue_index);
    auto&          u           = ues[ue_index];
    const unsigned count_index = is_dl ? 0 : 1;

    if (ack) {
      u.ko_counters[count_index].store(0, std::memory_order::memory_order_relaxed);
    } else {
      unsigned current_count = u.ko_counters[count_index].fetch_add(1, std::memory_order::memory_order_relaxed) + 1;
      // Note: We use == instead of <= to ensure only one notification is sent.
      const unsigned max_counter = is_dl ? max_consecutive_kos[cell_index].max_consecutive_dl_kos
                                         : max_consecutive_kos[cell_index].max_consecutive_ul_kos;
      if (current_count == max_counter) {
        std::lock_guard<std::mutex> lock(u.notifier_mutex);
        if (u.notifier != nullptr) {
          logger.warning("ue={}: RLF detected. Cause: {} consecutive {} KOs.",
                         ue_index,
                         current_count,
                         is_dl ? "HARQ-ACK" : "CRC");

          // Notify upper layers.
          u.notifier->on_rlf_detected();
        }
      }
    }
  }

  // DL for index 0, UL for index 1, CSI for index 2 in each cell.
  static_vector<rlf_metrics, MAX_NOF_DU_CELLS> max_consecutive_kos;
  srslog::basic_logger&                        logger;

  struct ue_context {
    // DL for index 0, UL for index 1, CSI for index 2.
    std::array<std::atomic<unsigned>, 3> ko_counters{};
    mac_ue_radio_link_notifier*          notifier = nullptr;
    // Access to the notifier is protected, as many threads may access it.
    std::mutex notifier_mutex;
  };

  std::array<ue_context, MAX_NOF_DU_UES> ues;
};

} // namespace srsran