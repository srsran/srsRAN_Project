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

#include "srsgnb/cu_up/cu_up_types.h"
#include <map>

namespace srsgnb {
namespace srs_cu_up {

/// \brief Context for a UE within the CU-UP with storage for all active PDU sessions.
class ue_context
{
public:
  ue_context(ue_index_t index_) : index(index_){};
  ~ue_context() = default;

  ue_index_t get_index() const { return index; };

private:
  ue_index_t index;
};

} // namespace srs_cu_up
} // namespace srsgnb
