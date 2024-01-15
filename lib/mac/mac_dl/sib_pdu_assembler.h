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

#include "srsran/mac/mac_cell_manager.h"
#include "srsran/scheduler/scheduler_slot_handler.h"

namespace srsran {

/// Class that manages the encoding of BCCH-DL-SCH messages to be fit in a Transport Block.
class sib_pdu_assembler
{
public:
  explicit sib_pdu_assembler(const std::vector<byte_buffer>& bcch_dl_sch_payloads)
  {
    // Number of padding bytes to pre-reserve. This value is implementation-defined.
    static constexpr unsigned MAX_PADDING_BYTES_LEN = 64;

    bcch_min_payload_sizes.resize(bcch_dl_sch_payloads.size());
    bcch_payloads.resize(bcch_dl_sch_payloads.size());
    for (unsigned i = 0; i != bcch_payloads.size(); ++i) {
      bcch_min_payload_sizes[i] = units::bytes(bcch_dl_sch_payloads[i].length());

      // Note: Resizing the bcch_payload after the ctor is forbidden, to avoid vector memory relocations and
      // invalidation of pointers passed to the lower layers. For this reason, we pre-reserve any potential padding
      // bytes.
      bcch_payloads[i].resize(bcch_dl_sch_payloads[i].length() + MAX_PADDING_BYTES_LEN, 0);
      std::copy(bcch_dl_sch_payloads[i].begin(), bcch_dl_sch_payloads[i].end(), bcch_payloads[i].begin());
    }
  }

  span<const uint8_t> encode_sib1_pdu(units::bytes tbs_bytes) const { return encode_si_pdu(0, tbs_bytes); }

  span<const uint8_t> encode_si_message_pdu(unsigned si_msg_idx, units::bytes tbs_bytes) const
  {
    return encode_si_pdu(si_msg_idx + 1, tbs_bytes);
  }

private:
  span<const uint8_t> encode_si_pdu(unsigned idx, units::bytes tbs_bytes) const
  {
    srsran_assert(tbs_bytes >= bcch_min_payload_sizes[idx],
                  "The allocated PDSCH TBS cannot be smaller than the respective SI{} payload",
                  idx == 0 ? fmt::format("B1") : fmt::format("-message {}", idx + 1));
    srsran_assert(tbs_bytes <= units::bytes(bcch_payloads[idx].size()),
                  "Memory rellocations of the SIB1 payload not allowed. Consider reserving more bytes for PADDING");
    return span<const uint8_t>(bcch_payloads[idx].data(), tbs_bytes.value());
  }

  /// Holds the original BCCH-DL-SCH messages, defined in the MAC cell configuration, plus extra padding bytes.
  std::vector<std::vector<uint8_t>> bcch_payloads;
  /// Length of the original BCCH-DL-SCH message, without padding, defined in the MAC cell configuration.
  std::vector<units::bytes> bcch_min_payload_sizes;
};

} // namespace srsran
