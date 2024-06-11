/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include <cstdint>

namespace srsran {

using nr_cell_id_t = uint64_t;

struct gnb_id_t {
  uint32_t id;         ///< gNodeB identifier.
  uint8_t  bit_length; ///< Length of gNB identity in bits. Values {22,...,32}.

  bool contains_nci(nr_cell_id_t nci) const { return (nci >> (36U - bit_length)) == id; }

  bool operator==(const gnb_id_t& rhs) const { return id == rhs.id && bit_length == rhs.bit_length; }
  bool operator!=(const gnb_id_t& rhs) const { return id != rhs.id || bit_length != rhs.bit_length; }
};

} // namespace srsran
