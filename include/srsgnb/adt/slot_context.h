/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_ADT_SLOT_CONTEXT_H
#define SRSGNB_ADT_SLOT_CONTEXT_H

namespace srsgnb {

/**
 * @brief Describes the slot context
 */
struct slot_context_t {
  unsigned numerology; ///< Current numerology
  unsigned frame;      ///< Frame index
  unsigned slot;       ///< Slot index
};

} // namespace srsgnb

#endif // SRSGNB_ADT_SLOT_CONTEXT_H