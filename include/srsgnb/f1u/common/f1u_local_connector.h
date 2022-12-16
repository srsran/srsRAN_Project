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

#include "srsgnb/f1u/cu_up/f1u_connector.h"
#include "srsgnb/f1u/du/f1u_connector.h"

/*
 * This file will hold the interfaces for the connector between
 * the DU and CU-UP. At the moment, only co-located configurations
 * are supported.
 */

namespace srsgnb {

/// TODO write docs.
class f1u_local_connector
{
public:
  f1u_local_connector()                                       = default;
  virtual ~f1u_local_connector()                              = default;
  f1u_local_connector(const f1u_local_connector&)             = delete;
  f1u_local_connector& operator=(const f1u_local_connector&)  = delete;
  f1u_local_connector(const f1u_local_connector&&)            = delete;
  f1u_local_connector& operator=(const f1u_local_connector&&) = delete;

  virtual f1u_du_connector*    get_f1u_du_connector()    = 0;
  virtual f1u_cu_up_connector* get_f1u_cu_up_connector() = 0;
};

} // namespace srsgnb
