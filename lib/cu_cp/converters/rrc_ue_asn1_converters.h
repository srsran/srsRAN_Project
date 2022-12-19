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

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/asn1/e1ap/e1ap.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/e1/cu_cp/e1_cu_cp.h"
#include <string>
#include <vector>

namespace srsgnb {
namespace srs_cu_cp {

/// @brief Convert a RRC Establishment Cause to a NGAP RRC Establishment Cause.
/// @param establishment_cause The RRC Establishment Cause.
/// @return The NGAP RRC Establishment Cause.
inline asn1::ngap::rrcestablishment_cause_opts rrc_establishment_cause_to_ngap_rrcestablishment_cause(
    const asn1::rrc_nr::establishment_cause_opts& establishment_cause)
{
  asn1::ngap::rrcestablishment_cause_opts rrcestablishment_cause = {};
  switch (establishment_cause.value) {
    case asn1::rrc_nr::establishment_cause_opts::options::emergency:
      rrcestablishment_cause.value = asn1::ngap::rrcestablishment_cause_opts::emergency;
      break;
    case asn1::rrc_nr::establishment_cause_opts::options::high_prio_access:
      rrcestablishment_cause.value = asn1::ngap::rrcestablishment_cause_opts::high_prio_access;
      break;
    case asn1::rrc_nr::establishment_cause_opts::options::mt_access:
      rrcestablishment_cause.value = asn1::ngap::rrcestablishment_cause_opts::mt_access;
      break;
    case asn1::rrc_nr::establishment_cause_opts::options::mo_sig:
      rrcestablishment_cause.value = asn1::ngap::rrcestablishment_cause_opts::mo_sig;
      break;
    case asn1::rrc_nr::establishment_cause_opts::options::mo_data:
      rrcestablishment_cause.value = asn1::ngap::rrcestablishment_cause_opts::mo_data;
      break;
    case asn1::rrc_nr::establishment_cause_opts::options::mo_voice_call:
      rrcestablishment_cause.value = asn1::ngap::rrcestablishment_cause_opts::mo_voice_call;
      break;
    case asn1::rrc_nr::establishment_cause_opts::options::mo_video_call:
      rrcestablishment_cause.value = asn1::ngap::rrcestablishment_cause_opts::mo_video_call;
      break;
    case asn1::rrc_nr::establishment_cause_opts::options::mo_sms:
      rrcestablishment_cause.value = asn1::ngap::rrcestablishment_cause_opts::mo_sms;
      break;
    case asn1::rrc_nr::establishment_cause_opts::options::mps_prio_access:
      rrcestablishment_cause.value = asn1::ngap::rrcestablishment_cause_opts::mps_prio_access;
      break;
    case asn1::rrc_nr::establishment_cause_opts::options::mcs_prio_access:
      rrcestablishment_cause.value = asn1::ngap::rrcestablishment_cause_opts::mcs_prio_access;
      break;
    default:
      rrcestablishment_cause.value = asn1::ngap::rrcestablishment_cause_opts::nulltype;
      break;
  }

  return rrcestablishment_cause;
}

} // namespace srs_cu_cp
} // namespace srsgnb
