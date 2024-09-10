/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/du/du.h"

namespace srsran {
namespace srs_du {

class du_high_wrapper;
class du_low_wrapper;

/// Distributed Unit wrapper interface. Wraps the DU high wrapper and DU low wrapper in an object.
class du_wrapper : public du
{
public:
  virtual ~du_wrapper() = default;

  /// Returns the DU high wrapper of this DU wrapper.
  virtual du_high_wrapper& get_du_high_wrapper() = 0;

  /// Returns the DU low wrapper of this DU wrapper.
  virtual du_low_wrapper& get_du_low_wrapper() = 0;
};

} // namespace srs_du
} // namespace srsran
