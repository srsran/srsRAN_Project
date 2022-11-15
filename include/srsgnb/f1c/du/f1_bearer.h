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

#include "srsgnb/adt/byte_buffer_slice_chain.h"

namespace srsgnb {
namespace srs_du {

/// Interface used by lower layers to push Rx PDUs to F1.
class f1_pdu_handler
{
public:
  virtual ~f1_pdu_handler() = default;

  /// Handle PDUs that are pushed to the F1 from lower layers.
  virtual void handle_pdu(byte_buffer_slice_chain pdu) = 0;
};

/// Interface used by upper layers to push Tx SDUs to F1.
class f1_sdu_handler
{
public:
  virtual ~f1_sdu_handler() = default;

  /// Handle SDUs that are pushed to the F1 from upper layers.
  virtual void handle_sdu(byte_buffer sdu) = 0;
};

class f1_bearer : public f1_pdu_handler, public f1_sdu_handler
{};

/// Interface used by F1 to push PDUs to lower layers.
class f1_tx_pdu_notifier
{
public:
  virtual ~f1_tx_pdu_notifier() = default;

  virtual void on_tx_pdu(byte_buffer pdu) = 0;
};

} // namespace srs_du
} // namespace srsgnb