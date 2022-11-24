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
#include "srsgnb/f1c/common/f1c_types.h"
#include "srsgnb/ngap/ngc.h"
#include "srsgnb/ngap/ngc_types.h"
#include <unordered_map>

namespace srsgnb {
namespace srs_cu_cp {

class ngc_ue
{
public:
  ngc_ue(ue_ngap_id_t ue_ngap_id, ngc_rrc_ue_notifier& rrc_ue_notifier_) : rrc_ue_notifier(rrc_ue_notifier_)
  {
    ue_ctxt.du_index = get_du_index_from_ue_ngap_id(ue_ngap_id);
    ue_ctxt.ue_index = get_ue_index_from_ue_ngap_id(ue_ngap_id);
  }

  ngc_ue_context       ue_ctxt;
  ngc_rrc_ue_notifier& rrc_ue_notifier;
};

/// \brief Stores the list of UE contexts currently registered in the NGC.
class ngc_ue_manager
{
public:
  ngc_ue_context& add_ue(ue_ngap_id_t ue_ngap_id, ngc_rrc_ue_notifier& rrc_ue_notifier);

  void remove_ue(ue_ngap_id_t ue_ngap_id);

  size_t get_nof_ues() const { return ues.size(); }

  bool contains(std::underlying_type_t<ue_ngap_id_t> ue_ngap_id_uint) { return ues.contains(ue_ngap_id_uint); }
  bool contains(ue_ngap_id_t ue_ngap_id) { return contains(ue_ngap_id_to_uint(ue_ngap_id)); }

  void set_amf_ue_id(std::underlying_type_t<ue_ngap_id_t> ue_ngap_id_uint, ue_amf_id_t amf_ue_id)
  {
    ues[ue_ngap_id_uint].ue_ctxt.amf_ue_id = amf_ue_id;
  };

  void set_amf_ue_id(std::underlying_type_t<ue_ngap_id_t> ue_ngap_id_uint,
                     std::underlying_type_t<ue_ngap_id_t> amf_ue_id_uint)
  {
    set_amf_ue_id(ue_ngap_id_uint, uint_to_ue_amf_id(amf_ue_id_uint));
  };

  void set_amf_ue_id(ue_ngap_id_t ue_ngap_id, ue_amf_id_t amf_ue_id)
  {
    set_amf_ue_id(ue_ngap_id_to_uint(ue_ngap_id), amf_ue_id);
  };

  ue_amf_id_t get_amf_ue_id(std::underlying_type_t<ue_ngap_id_t> ue_ngap_id_uint)
  {
    return ues[ue_ngap_id_uint].ue_ctxt.amf_ue_id;
  };

  ue_amf_id_t get_amf_ue_id(ue_ngap_id_t ue_ngap_id) { return ues[ue_ngap_id_to_uint(ue_ngap_id)].ue_ctxt.amf_ue_id; };

  ngc_ue&       operator[](ue_ngap_id_t ue_ngap_id) { return ues[ue_ngap_id_to_uint(ue_ngap_id)]; }
  const ngc_ue& operator[](ue_ngap_id_t ue_ngap_id) const { return ues[ue_ngap_id_to_uint(ue_ngap_id)]; }

private:
  slot_array<ngc_ue, MAX_NOF_CU_UES> ues;
};

} // namespace srs_cu_cp
} // namespace srsgnb
