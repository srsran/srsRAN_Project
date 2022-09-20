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

#include "srsgnb/adt/slot_array.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/f1_interface/common/f1c_types.h"
#include "srsgnb/ngap/ngc_types.h"
#include <unordered_map>

namespace srsgnb {
namespace srs_cu_cp {

class ngap_ue
{
public:
  ngap_ue(du_index_t du_index_, ue_index_t ue_index_, ue_ngap_id_t ue_ngap_id_) :
    du_index(du_index_), ue_index(ue_index_), ue_ngap_id(ue_ngap_id_)
  {
  }

  const du_index_t   du_index;
  const ue_index_t   ue_index;
  const ue_ngap_id_t ue_ngap_id;
};

/// \brief Stores the list of UE contexts currently registered in the NGAP.
class ngap_ue_manager
{
public:
  ngap_ue& add_ue(du_index_t du_index, ue_index_t ue_index)
  {
    ue_ngap_id_t ue_ngap_id = get_ue_ngap_id(du_index, ue_index);
    uint64_t     ue_id      = ue_ngap_id_to_uint(ue_ngap_id);

    srsgnb_assert(not ues.contains(ue_id), "Duplicate ue_ngap_id={} detected", ue_id);

    ues.emplace(ue_id, du_index, ue_index, uint_to_ue_ngap_id(ue_id));

    return ues[ue_id];
  }

  void remove_ue(ue_ngap_id_t ue_ngap_id)
  {
    uint64_t ue_id = ue_ngap_id_to_uint(ue_ngap_id);
    srsgnb_assert(ues.contains(ue_id), "ue_ngap_id={} does not exist", ue_id);
    ues.erase(ue_id);
  }

  ngap_ue&       operator[](ue_ngap_id_t ue_ngap_id) { return ues[ue_ngap_id_to_uint(ue_ngap_id)]; }
  const ngap_ue& operator[](ue_ngap_id_t ue_ngap_id) const { return ues[ue_ngap_id_to_uint(ue_ngap_id)]; }

  du_index_t get_du_index(ue_ngap_id_t ue_ngap_id) const
  {
    uint64_t ue_id = ue_ngap_id_to_uint(ue_ngap_id);
    return int_to_du_index((ue_id - (ue_id % MAX_NOF_UES)) / MAX_NOF_UES);
  }

  ue_index_t get_ue_index(ue_ngap_id_t ue_ngap_id) const
  {
    uint64_t ue_id = ue_ngap_id_to_uint(ue_ngap_id);
    return int_to_ue_index(ue_id % MAX_NOF_UES);
  }

private:
  slot_array<ngap_ue, MAX_NOF_CU_UES> ues;
};

} // namespace srs_cu_cp
} // namespace srsgnb