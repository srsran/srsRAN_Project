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

#include "srsgnb/pdcp/pdcp_entity.h"
#include <cstdio>

namespace srsgnb {

class pdcp_entity_impl : public pdcp_entity
{
public:
  bool decapsulate(byte_buffer& data) override
  {
    std::printf("[PDCP-ENTITY] Removing PDCP header from packet of size = %u\n", (unsigned)data.length());
    data.trim_head(2);
    std::printf("[PDCP-ENTITY] New size after removing PDCP header is %u bytes\n", (unsigned)data.length());
    return true;
  }
};

} // namespace srsgnb
