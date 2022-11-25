/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ngc_ue_manager.h"

using namespace srsgnb;
using namespace srs_cu_cp;

ngc_ue& ngc_ue_manager::add_ue(ue_ngap_id_t ue_ngap_id, ngc_rrc_ue_notifier& rrc_ue_notifier)
{
  uint64_t ue_id = ue_ngap_id_to_uint(ue_ngap_id);

  srsgnb_assert(not ues.contains(ue_id), "Duplicate ue_ngap_id={} detected", ue_id);

  ues.emplace(ue_id, ue_ngap_id, rrc_ue_notifier);

  return ues[ue_id];
}

void ngc_ue_manager::remove_ue(ue_ngap_id_t ue_ngap_id)
{
  uint64_t ue_id = ue_ngap_id_to_uint(ue_ngap_id);
  srsgnb_assert(ues.contains(ue_id), "ue_ngap_id={} does not exist", ue_id);
  ues.erase(ue_id);
}
