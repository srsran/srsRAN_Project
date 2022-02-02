#ifndef SRSGNB_ADT_BIT_BUFFER_H
#define SRSGNB_ADT_BIT_BUFFER_H

#include "srsgnb/adt/span.h"

namespace srsgnb {

/// Type to store a dynamic amount of contiguous unpacked bits.
using bit_buffer = span<unsigned char>;

} // namespace srsgnb

#endif // SRSGNB_ADT_BIT_BUFFER_H