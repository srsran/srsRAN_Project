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

#include "srsran/adt/span.h"

namespace srsran {

class du_operation_controller;
class upper_phy;

namespace srs_du {

/// DU low interface.
class du_low
{
public:
  /// Default destructor.
  virtual ~du_low() = default;

  /// Returns the operation controller of this DU low.
  virtual du_operation_controller& get_operation_controller() = 0;

  /// Returns the upper PHY for the given cell of this DU low.
  virtual upper_phy& get_upper_phy(unsigned cell_id) = 0;
};

} // namespace srs_du
} // namespace srsran
