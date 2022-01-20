#ifndef SRSGNB_ADT_BYTE_BUFFER_H
#define SRSGNB_ADT_BYTE_BUFFER_H

#include <vector>

namespace srsgnb {

/// Type to store a dynamic amount of contiguous bytes.
using byte_buffer = std::vector<unsigned char>;

} // namespace srsgnb

#endif // SRSGNB_ADT_BYTE_BUFFER_H