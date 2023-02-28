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

#include "srsran/asn1/rrc_nr/rrc_nr.h"
#include "srsran/mac/cell_configuration.h"
#include "srsran/scheduler/scheduler_slot_handler.h"
#include "srsran/support/memory_pool/ring_buffer_pool.h"

namespace srsran {

/// Encodes Paging PDUs based on Paging information provided by the scheduler.
class paging_pdu_assembler
{
public:
  paging_pdu_assembler() = default;

  /// \brief Encode Paging MAC PDU from Paging grant information.
  /// \param[in] pg Paging grant information.
  /// \return Encoded Paging MAC PDU.
  static span<const uint8_t> encode_paging_pdu(const dl_paging_allocation& pg);

private:
  /// \brief Build RRC-NR Paging message from the Paging grant.
  /// \param[in] pg Paging grant information.
  /// \return RRC-NR Paging message.
  static asn1::rrc_nr::paging_s make_asn1_rrc_cell_paging_msg(const dl_paging_allocation& pg);

  /// \brief Derive packed cell PCCH-PCH Paging message.
  /// \param[in] pg Paging grant information.
  /// \return byte buffer with packed cell PCCH-PCH message.
  static byte_buffer make_asn1_rrc_cell_pcch_pch_msg(const dl_paging_allocation& pg);
};

} // namespace srsran
