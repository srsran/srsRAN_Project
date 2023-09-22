/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/variant.h"
#include <vector>

namespace srsran {

enum class sib_type { sib1 = 1, sib2 = 2, sib19 = 19 };

struct sib2 {
  optional<uint8_t> nof_ssbs_to_average;
  // TODO
};

struct sib19 {
  optional<uint16_t> distance_thres;
  // TODO
};

/// \brief Variant type that can hold different types of SIBs that go in a SI message.
using sib_info = variant<sib2, sib19>;

inline sib_type get_sib_info_type(const sib_info& sib)
{
  return static_cast<sib_type>(sib.index() + 2);
}

/// \brief This struct contains the information required for the generation of the SI messages sent by the network and
/// the generation of the SIB1 "SI-SchedulingInfo" field of the SIB1. See TS 38.331, "SystemInformation" and
/// "SI-SchedulingInfo".
struct si_scheduling_info_config {
  struct sched_info {
    /// List of SIBs (sib2, sib3, ...) included in this SI message. The list has at most 32 elements.
    std::vector<sib_type> sib_mapping_info;
    /// Periodicity of the SI-message in radio frames. Values: {8, 16, 32, 64, 128, 256, 512}.
    unsigned si_period_radio_frames;
  };

  /// \brief The length of the SI scheduling window, in slots. It is always shorter or equal to the period of the SI
  /// message. Values: {5, 10, 20, 40, 80, 160, 320, 640, 1280}.
  unsigned si_window_len_slots;
  /// List of SI-messages and associated scheduling information.
  std::vector<sched_info> sched_info_list;
  /// Information included in each SIB that is scheduled as part of one of the SI-messages.
  std::vector<sib_info> sibs;
};

} // namespace srsran