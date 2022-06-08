/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RLC_COMMON_BEARER_H
#define SRSGNB_RLC_COMMON_BEARER_H

#include "rlc_logger.h"
#include "srsgnb/rlc/rlc.h"
#include "srsgnb/srslog/srslog.h"

namespace srsgnb {

/// Class used to store common parameters for all RLC bearer types.
/// It will contain the base class for TX and RX entities and getters
/// for them.
///
/// \param du_index UE identifier within the DU
/// \param lcid LCID for the bearer
class rlc_common_bearer
{
public:
  rlc_common_bearer(du_ue_index_t du_index, lcid_t lcid) : du_index(du_index), lcid(lcid), logger(du_index, lcid) {}

protected:
  const du_ue_index_t du_index;
  const lcid_t        lcid;
  rlc_logger          logger;
};

} // namespace srsgnb
#endif

