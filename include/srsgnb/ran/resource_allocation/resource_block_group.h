/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RESOURCE_BLOCK_GROUP_H
#define SRSGNB_RESOURCE_BLOCK_GROUP_H

#include "srsgnb/adt/bounded_bitset.h"
#include "srsgnb/ran/resource_allocation/rb_interval.h"

namespace srsgnb {

/// Maximum number of resource block groups per BWP.
const size_t MAX_NOF_RBGS = 18;

/// Nominal RBG-size (P value) as per TS 38.214, 6.1.2.2.
enum nominal_rbg_size : unsigned { P2 = 2, P4 = 4, P8 = 8, P16 = 16 };

/// Bitset of RBGs with size up to 18.
using rbg_bitmap = bounded_bitset<MAX_NOF_RBGS, true>;

/// TS 38.214, Table 6.1.2.2.1-1 - Nominal RBG size P.
nominal_rbg_size get_nominal_rbg_size(unsigned bwp_nof_prb, bool config_1_or_2);

/// TS 38.214 - total number of RBGs for a uplink bandwidth part of size "bwp_nof_prb" PRBs.
unsigned get_nof_rbgs(crb_interval bwp_rb_dims, nominal_rbg_size P);
unsigned get_nof_rbgs(crb_interval bwp_rb_dims, bool config1_or_2);

/// Calculates size of indexed RBG in number of RBs.
uint32_t get_rbg_size(crb_interval bwp_rbs, nominal_rbg_size P, uint32_t rbg_idx);

} // namespace srsgnb

#endif // SRSGNB_RESOURCE_BLOCK_GROUP_H
