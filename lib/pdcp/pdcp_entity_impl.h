#ifndef SRSGNB_PDCP_PDCP_ENTITY_IMPL_H
#define SRSGNB_PDCP_PDCP_ENTITY_IMPL_H

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

#endif // SRSGNB_PDCP_PDCP_ENTITY_IMPL_H