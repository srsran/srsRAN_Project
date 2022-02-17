#ifndef SRSGNB_ADT_BYTE_BUFFER_H
#define SRSGNB_ADT_BYTE_BUFFER_H

#include <vector>
#include <cstdint>

namespace srsgnb {

/// Type to store a dynamic amount of contiguous bytes.
using byte_buffer = std::vector<uint8_t>;

} // namespace srsgnb

#endif // SRSGNB_ADT_BYTE_BUFFER_H