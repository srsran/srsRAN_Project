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

#include "srsran/ngap/ngap.h"
#include "srsran/ran/plmn_identity.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Handler of the NG interface of the CU-CP.
///
/// This interface is used to forward NGAP messages or TNL connection updates to the CU-CP.
class cu_cp_ng_handler
{
public:
  virtual ~cu_cp_ng_handler() = default;

  /// \brief Get the NG message handler interface.
  /// \param[in] plmn The PLMN of the NGAP.
  /// \return A pointer to the NG message handler interface if it was found, nullptr otherwise.
  virtual ngap_message_handler* get_ngap_message_handler(const plmn_identity& plmn) = 0;

  /// \brief Get the state of the AMF connections.
  /// \return True if all AMFs are connected, false otherwise.
  virtual bool amfs_are_connected() = 0;
};

} // namespace srs_cu_cp
} // namespace srsran