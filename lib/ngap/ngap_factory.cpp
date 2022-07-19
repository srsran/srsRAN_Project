/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/ngap/ngap_factory.h"
#include "ngap_impl.h"

using namespace srsgnb;
using namespace srs_cu_cp;

std::unique_ptr<ngap> srsgnb::srs_cu_cp::create_ngap()
{
  return std::unique_ptr<ngap>(new ngap_impl);
}
