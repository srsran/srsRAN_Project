/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_F1_PROCEDURE_HELPERS_H
#define SRSGNB_F1_PROCEDURE_HELPERS_H

#include "srsgnb/f1_interface/f1ap_du.h"

namespace srsgnb {
namespace f1ap {

/// \brief Fills f1_setup_request_message.
/// \param[out] request_msg The f1_setup_request_message struct to fill.
/// \param[in] du_params DU setup parameters to fill the f1 setup request message with.
void fill_f1_setup_request_message(f1_setup_request_message& request_msg, const du_setup_params& du_params)
{
  // TODO: Add other inputs and set values accordingly
  request_msg.setup_params = du_params;
}

} // namespace f1ap
} // namespace srsgnb

#endif // SRSGNB_F1_PROCEDURE_HELPERS_H