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

#include "srsgnb/adt/slotted_array.h"
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
  ngc_ue(cu_cp_ue_id_t cu_cp_ue_id, ran_ue_id_t ran_ue_id, ngc_rrc_ue_notifier& rrc_ue_notifier_) :
    rrc_ue_notifier(rrc_ue_notifier_)
  {
    ue_ctxt.ran_ue_id = ran_ue_id;
  }

  ngc_rrc_ue_notifier& get_rrc_ue_notifier() { return rrc_ue_notifier; }

  void set_amf_ue_id(amf_ue_id_t amf_ue_id) { ue_ctxt.amf_ue_id = amf_ue_id; }
  void set_amf_ue_id(std::underlying_type_t<amf_ue_id_t> amf_ue_id_uint)
  {
    set_amf_ue_id(uint_to_amf_ue_id(amf_ue_id_uint));
  }
  amf_ue_id_t get_amf_ue_id() { return ue_ctxt.amf_ue_id; }

  ran_ue_id_t get_ran_ue_id() { return ue_ctxt.ran_ue_id; }

private:
  ngc_ue_context       ue_ctxt;
  ngc_rrc_ue_notifier& rrc_ue_notifier;
};

/// \brief Stores the list of UE contexts currently registered in the NGC.
class ngc_ue_manager
{
public:
  ngc_ue_manager();
  ~ngc_ue_manager() = default;

  ngc_ue& add_ue(cu_cp_ue_id_t cu_cp_ue_id, ngc_rrc_ue_notifier& rrc_ue_notifier);

  void remove_ue(cu_cp_ue_id_t cu_cp_ue_id);

  size_t get_nof_ues() const { return ues.size(); }

  cu_cp_ue_id_t get_cu_cp_ue_id(std::underlying_type_t<ran_ue_id_t> ran_ue_id_uint);
  cu_cp_ue_id_t get_cu_cp_ue_id(ran_ue_id_t ran_ue_id) { return get_cu_cp_ue_id(ran_ue_id_to_uint(ran_ue_id)); }

  bool contains(std::underlying_type_t<cu_cp_ue_id_t> cu_cp_ue_id_uint) { return ues.contains(cu_cp_ue_id_uint); }
  bool contains(cu_cp_ue_id_t cu_cp_ue_id) { return contains(cu_cp_ue_id_to_uint(cu_cp_ue_id)); }

  ngc_ue&       operator[](std::underlying_type_t<cu_cp_ue_id_t> cu_cp_ue_id_uint) { return ues[cu_cp_ue_id_uint]; }
  const ngc_ue& operator[](std::underlying_type_t<cu_cp_ue_id_t> cu_cp_ue_id_uint) const
  {
    return ues[cu_cp_ue_id_uint];
  }

  ngc_ue&       operator[](cu_cp_ue_id_t cu_cp_ue_id) { return ues[cu_cp_ue_id_to_uint(cu_cp_ue_id)]; }
  const ngc_ue& operator[](cu_cp_ue_id_t cu_cp_ue_id) const { return ues[cu_cp_ue_id_to_uint(cu_cp_ue_id)]; }

private:
  /// \brief Get the next available RAN UE ID.
  /// \return The RAN UE ID.
  ran_ue_id_t get_next_ran_ue_id();

  /// \brief Find the RAN UE ID by a given UE NGAP ID.
  /// \param[in] cu_cp_ue_id The UE NGAP ID used to find the RAN UE ID.
  /// \return The RAN UE ID.
  ran_ue_id_t find_ran_ue_id(cu_cp_ue_id_t cu_cp_ue_id);

  srslog::basic_logger&                        logger;
  slotted_array<ngc_ue, MAX_NOF_CU_UES>        ues;
  slotted_array<cu_cp_ue_id_t, MAX_NOF_CU_UES> ran_ue_id_to_cu_cp_ue_id; // cu_cp_ue_ids indexed by ran_ue_ids
};

} // namespace srs_cu_cp
} // namespace srsgnb
