/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/optional.h"
#include "srsran/asn1/f1ap/f1ap.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/up_transport_layer_info.h"
#include <string>
#include <vector>

namespace srsran {
namespace srs_cu_cp {

/// \brief Convert F1AP Cause to \c cause_t type.
/// \param f1ap_cause The F1AP Cause.
/// \return The cause_t type.
inline cause_t f1ap_cause_to_cause(asn1::f1ap::cause_c f1ap_cause)
{
  cause_t cause;

  switch (f1ap_cause.type()) {
    case asn1::f1ap::cause_c::types_opts::radio_network:
      cause = cause_t::radio_network;
      return cause;
      break;
    case asn1::f1ap::cause_c::types_opts::transport:
      cause = cause_t::transport;
      return cause;
      break;
    case asn1::f1ap::cause_c::types_opts::protocol:
      cause = cause_t::protocol;
      return cause;
      break;
    case asn1::f1ap::cause_c::types_opts::misc:
      cause = cause_t::misc;
      return cause;
      break;
    default:
      cause = cause_t::nulltype;
      return cause;
  }
}

/// \brief Convert \c cause_t type to F1AP cause.
/// \param cause The cause_t type.
/// \return The F1AP cause.
inline asn1::f1ap::cause_c cause_to_f1ap_cause(cause_t cause)
{
  asn1::f1ap::cause_c f1ap_cause;

  switch (cause) {
    case cause_t::radio_network:
      f1ap_cause.set(asn1::f1ap::cause_c::types_opts::radio_network);
      return f1ap_cause;
      break;
    case cause_t::transport:
      f1ap_cause.set(asn1::f1ap::cause_c::types_opts::transport);
      return f1ap_cause;
      break;
    case cause_t::protocol:
      f1ap_cause.set(asn1::f1ap::cause_c::types_opts::protocol);
      return f1ap_cause;
      break;
    case cause_t::misc:
      f1ap_cause.set(asn1::f1ap::cause_c::types_opts::misc);
      return f1ap_cause;
      break;
    default:
      f1ap_cause.set(asn1::f1ap::cause_c::types_opts::nulltype);
      return f1ap_cause;
  }
}

/// \brief Convert F1AP NRCGI to NR Cell Identity.
/// \param f1ap_cgi The F1AP NRCGI.
/// \return The NR Cell Identity.
inline nr_cell_id_t f1ap_nrcgi_to_nr_cell_identity(asn1::f1ap::nr_cgi_s& f1ap_cgi)
{
  nr_cell_id_t nci;
  nci.packed = f1ap_cgi.nr_cell_id.to_number();

  nci.cell_idenity = (f1ap_cgi.nr_cell_id.to_number() & 0xfffc0000);
  nci.gnb_identity = (f1ap_cgi.nr_cell_id.to_number() & 0X3ff0);

  return nci;
}

} // namespace srs_cu_cp
} // namespace srsran
