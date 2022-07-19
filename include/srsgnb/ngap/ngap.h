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

namespace srsgnb {

namespace srs_cu_cp {

/// Interface class for the NGAP object.
class ngap
{
public:
  ngap()          = default;
  virtual ~ngap() = default;
};

} // namespace srs_cu_cp

} // namespace srsgnb