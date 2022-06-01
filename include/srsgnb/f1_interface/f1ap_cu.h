/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_F1AP_CU_H
#define SRSGNB_F1AP_CU_H

#include "f1c_common.h"
#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/expected.h"
#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {

class f1ap_cu_configurator
{
public:
  virtual ~f1ap_cu_configurator() = default;
};

/// Combined entry point for F1C/U handling.
class f1_cu_interface : public f1c_message_handler, public f1c_event_handler, public f1ap_cu_configurator
{
public:
  virtual ~f1_cu_interface() = default;
};

} // namespace srsgnb

#endif // SRSGNB_F1AP_CU_H
