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

#include "srsgnb/ngap/ngap.h"
#include <cstdio>

namespace srsgnb {

namespace srs_cu_cp {

class ngap_impl : public ngap
{
public:
  ngap_impl()  = default;
  ~ngap_impl() = default;
};

} // namespace srs_cu_cp

} // namespace srsgnb