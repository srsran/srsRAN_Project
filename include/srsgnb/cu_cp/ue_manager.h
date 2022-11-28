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

#include "ue_context.h"

namespace srsgnb {
namespace srs_cu_cp {

class du_processor_ue_manager
{
public:
  virtual ~du_processor_ue_manager() = default;

  /// Allocate new UE context for the given RNTI. A UE index is allocated internally.
  /// If a new UE can't be allocated or if a UE with the same RNTI already exists, nulltpr is returned.
  /// \param rnti RNTI of the UE to be added.
  virtual ue_context* add_ue(rnti_t rnti)            = 0;
  virtual void        remove_ue(ue_index_t ue_index) = 0;
  virtual ue_context* find_ue(ue_index_t ue_index)   = 0;
  virtual ue_index_t  find_ue_index(rnti_t rnti)     = 0;
  virtual size_t      get_nof_ues()                  = 0;
};

} // namespace srs_cu_cp

} // namespace srsgnb
