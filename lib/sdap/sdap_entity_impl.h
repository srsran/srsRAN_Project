#ifndef SRSGNB_SDAP_SDAP_ENTITY_IMPL_H
#define SRSGNB_SDAP_SDAP_ENTITY_IMPL_H

#include "srsgnb/sdap/sdap_entity.h"
#include <cstdio>

namespace srsgnb {

class sdap_entity_impl : public sdap_entity
{
public:
  bool decapsulate(byte_buffer& data) override
  {
    std::printf("[SDAP-ENTITY] Removing SDAP header from packet of size = %u\n", (unsigned)data.length());
//    data.trim_front(3);
    std::printf("[SDAP-ENTITY] New size after removing SDAP header is %u bytes\n", (unsigned)data.length());
    return true;
  }
};

} // namespace srsgnb

#endif // SRSGNB_SDAP_SDAP_ENTITY_IMPL_H
