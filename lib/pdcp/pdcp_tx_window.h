/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "pdcp_bearer_logger.h"
#include "pdcp_tx_xdu_info.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/pdcp/pdcp_sn_size.h"
#include "srsran/security/security.h"
#include "srsran/support/sdu_window.h"

namespace srsran {

struct pdcp_tx_window_element {
  /// The actual SDU with additional meta info
  pdcp_tx_xdu_info sdu_info;
  /// Holds the SDU length even in case no SDU is stored (e.g. UM bearer or discard timer off). Required for tracking
  /// the total number of bytes in TX window to consider the desired buffer size from lower layers.
  uint32_t sdu_length;
};

/// Wrapper to PDCP tx window.
/// It provides interfaces for the PDCP bearers, for the higher and lower layers
class pdcp_tx_window
{
public:
  pdcp_tx_window(pdcp_rb_type rb_type_, pdcp_rlc_mode rlc_mode_, pdcp_sn_size sn_size_, pdcp_bearer_logger logger_);

  bool has_sn(uint32_t count) const;

  /// \brief Add SDU to TX window
  /// This method adds an SDU from the TX window. It will keep track of the PDU bytes in the window.
  void add_sdu(pdcp_tx_xdu_info&& sdu_info);

  /// \brief Remove SDU from TX window
  /// This method removes an SDU from the TX window. It will keep track of the PDU bytes in the window.
  ///
  /// \return true if the SDU exists in the window and was removed, false otherwise.
  void remove_sdu(uint32_t count);

  pdcp_tx_xdu_info& operator[](uint32_t count);

  void clear();

  uint32_t get_sdu_bytes() const;

  uint32_t get_pdu_bytes(security::integrity_enabled integrity) const;

private:
  pdcp_bearer_logger logger;

  /// \brief Tx window.
  /// This container is used to store discard timers of transmitted SDUs and, only for AM, a copy of the SDU for data
  /// recovery procedure. Upon expiry of a discard timer, the PDCP Tx entity instructs the lower layers to discard the
  /// associated PDCP PDU. See section 5.2.1 and 7.3 of TS 38.323.
  sdu_window<pdcp_tx_window_element, pdcp_bearer_logger> tx_window;

  /// Creates the tx_window according to sn_size
  /// \param sn_size Size of the sequence number (SN)
  /// \return unique pointer to tx_window instance
  void create_tx_window();

  pdcp_rb_type  rb_type;
  pdcp_rlc_mode rlc_mode;
  pdcp_sn_size  sn_size;

  uint32_t sdu_bytes = 0;
  uint32_t nof_sdus  = 0;
};

} // namespace srsran
