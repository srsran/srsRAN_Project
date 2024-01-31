/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/asn1/e1ap/common.h"
using namespace asn1;
using namespace asn1::e1ap;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// PrivateIE-ID ::= CHOICE
void private_ie_id_c::set(types::options e)
{
  type_ = e;
}
uint32_t& private_ie_id_c::set_local()
{
  set(types::local);
  return c;
}
void private_ie_id_c::set_global()
{
  set(types::global);
}
void private_ie_id_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::local:
      j.write_int("local", c);
      break;
    case types::global:
      break;
    default:
      log_invalid_choice_id(type_, "private_ie_id_c");
  }
  j.end_obj();
}
SRSASN_CODE private_ie_id_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::local:
      HANDLE_CODE(pack_integer(bref, c, (uint32_t)0u, (uint32_t)65535u, false, true));
      break;
    case types::global:
      break;
    default:
      log_invalid_choice_id(type_, "private_ie_id_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE private_ie_id_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::local:
      HANDLE_CODE(unpack_integer(c, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
      break;
    case types::global:
      break;
    default:
      log_invalid_choice_id(type_, "private_ie_id_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* private_ie_id_c::types_opts::to_string() const
{
  static const char* names[] = {"local", "global"};
  return convert_enum_idx(names, 2, value, "private_ie_id_c::types");
}

// PrivateIE-Field{E1AP-PRIVATE-IES : IEsSetParam} ::= SEQUENCE{{E1AP-PRIVATE-IES}}
template <class ies_set_paramT_>
SRSASN_CODE private_ie_field_s<ies_set_paramT_>::pack(bit_ref& bref) const
{
  HANDLE_CODE(id.pack(bref));
  HANDLE_CODE(crit.pack(bref));
  HANDLE_CODE(value.pack(bref));

  return SRSASN_SUCCESS;
}
template <class ies_set_paramT_>
SRSASN_CODE private_ie_field_s<ies_set_paramT_>::unpack(cbit_ref& bref)
{
  HANDLE_CODE(id.unpack(bref));
  HANDLE_CODE(crit.unpack(bref));
  HANDLE_CODE(value.unpack(bref));

  return SRSASN_SUCCESS;
}
template <class ies_set_paramT_>
void private_ie_field_s<ies_set_paramT_>::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("id");
  id.to_json(j);
  j.write_str("criticality", crit.to_string());
  j.end_obj();
}

template <class valueT_>
private_ie_container_item_s<valueT_>::private_ie_container_item_s(private_ie_id_c id_, crit_e crit_) :
  id(id_), crit(crit_)

{
}
template <class valueT_>
SRSASN_CODE private_ie_container_item_s<valueT_>::pack(bit_ref& bref) const
{
  HANDLE_CODE(id.pack(bref));
  HANDLE_CODE(crit.pack(bref));
  {
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(value.pack(bref));
  }
  return SRSASN_SUCCESS;
}
template <class valueT_>
SRSASN_CODE private_ie_container_item_s<valueT_>::unpack(cbit_ref& bref)
{
  HANDLE_CODE(id.unpack(bref));
  HANDLE_CODE(crit.unpack(bref));
  {
    varlength_field_unpack_guard varlen_scope(bref, true);
    HANDLE_CODE(value.unpack(bref));
  }
  return SRSASN_SUCCESS;
}
template <class valueT_>
void private_ie_container_item_s<valueT_>::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("id");
  id.to_json(j);
  j.write_str("criticality", crit.to_string());
  j.end_obj();
}