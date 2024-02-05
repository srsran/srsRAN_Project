/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/asn1/asn1_utils.h"

namespace asn1 {

namespace detail {

template <typename IEsSetParam>
struct ie_field_value_item {
  using obj_set_type = IEsSetParam;
  using value_type   = typename IEsSetParam::value_c;
  const char* item_name() const { return "value"; }
  void        set_item(uint32_t id) { item = IEsSetParam::get_value(id); }

protected:
  value_type item;
};

template <typename ExtensionSetParam>
struct ie_field_ext_item {
  using obj_set_type = ExtensionSetParam;
  using value_type   = typename ExtensionSetParam::ext_c;
  const char* item_name() const { return "extension"; }
  void        set_item(uint32_t id) { item = ExtensionSetParam::get_ext(id); }

protected:
  value_type item;
};

template <class IEItem>
struct base_ie_field : public IEItem {
  using obj_set_type = typename IEItem::obj_set_type;
  using value_type   = typename IEItem::value_type;

  uint32_t          id() const { return obj_set_type::idx_to_id(value().type().value); }
  crit_e            crit() const { return obj_set_type::get_crit(id()); }
  value_type&       value() { return this->item; }
  const value_type& value() const { return this->item; }

  value_type*       operator->() { return &value(); }
  const value_type* operator->() const { return &value(); }
  value_type&       operator*() { return value(); }
  const value_type& operator*() const { return value(); }

  SRSASN_CODE pack(bit_ref& bref) const
  {
    HANDLE_CODE(pack_integer(bref, id(), (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit().pack(bref));
    HANDLE_CODE(value().pack(bref));
    return SRSASN_SUCCESS;
  }
  SRSASN_CODE unpack(cbit_ref& bref)
  {
    uint32_t id_val;
    HANDLE_CODE(unpack_integer(id_val, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    this->set_item(id_val);
    HANDLE_CODE(crit().unpack(bref));
    HANDLE_CODE(value().unpack(bref));
    return SRSASN_SUCCESS;
  }
  void to_json(json_writer& j) const
  {
    j.start_obj();
    j.write_int("id", id());
    j.write_str("criticality", crit().to_string());
    j.write_fieldname(this->item_name());
    asn1::to_json(j, value());
    j.end_obj();
  }
  bool load_info_obj(const uint32_t& id_)
  {
    if (not obj_set_type::is_id_valid(id_)) {
      return false;
    }
    this->set_item(id_);
    return value().type().value != obj_set_type::value_c::types_opts::nulltype;
  }
};

} // namespace detail

// ProtocolIE-Field{LAYER-PROTOCOL-IES : IEsSetParam} ::= SEQUENCE{{IEsSetParam}}
template <class IEsSetParam>
struct protocol_ie_field_s : public detail::base_ie_field<detail::ie_field_value_item<IEsSetParam>> {
};

// ProtocolIE-SingleContainer{LAYER-PROTOCOL-IES : IEsSetParam} ::= SEQUENCE{{IEsSetParam}}
template <class ies_set_paramT_>
struct protocol_ie_single_container_s : public protocol_ie_field_s<ies_set_paramT_> {
};

// ProtocolExtensionField{LAYER-PROTOCOL-EXTENSION : ExtensionSetParam} ::= SEQUENCE{{LAYER-PROTOCOL-EXTENSION}}
template <class ExtensionSetParam>
struct protocol_ext_field_s : public detail::base_ie_field<detail::ie_field_ext_item<ExtensionSetParam>> {
};

template <typename IEValue>
SRSASN_CODE pack_ie_container_item(bit_ref& bref, uint32_t id, crit_e crit, const IEValue& value)
{
  HANDLE_CODE(pack_integer(bref, id, (uint32_t)0u, (uint32_t)65535u, false, true));
  HANDLE_CODE(crit.pack(bref));
  {
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(value.pack(bref));
  }
  return SRSASN_SUCCESS;
}

template <typename IEValue>
void ie_container_item_to_json(json_writer& j, uint32_t id, crit_e crit, const char* value_name, const IEValue& value)
{
  j.start_obj();
  j.write_int("id", id);
  j.write_str("criticality", crit.to_string());
  j.write_fieldname(value_name);
  asn1::to_json(j, value);
  j.end_obj();
}

namespace detail {

template <typename T>
struct ie_value_item {
  using value_type = T;
  value_type value;

  value_type*       operator->() { return &value; }
  const value_type* operator->() const { return &value; }
  value_type&       operator*() { return value; }
  const value_type& operator*() const { return value; }
  const char*       item_name() const { return "value"; }

protected:
  value_type&       item() { return value; }
  const value_type& item() const { return value; }
};

template <typename T>
struct ie_ext_item {
  using value_type = T;
  value_type ext;

  value_type*       operator->() { return &ext; }
  const value_type* operator->() const { return &ext; }
  value_type&       operator*() { return ext; }
  const value_type& operator*() const { return ext; }
  const char*       item_name() const { return "extension"; }

protected:
  value_type&       item() { return ext; }
  const value_type& item() const { return ext; }
};

template <class IEItem>
struct base_ie_container_item : public IEItem {
  using value_type = typename IEItem::value_type;

  base_ie_container_item(uint32_t id_, crit_e crit_) : id(id_), crit(crit_) {}

  uint32_t id = 0;
  crit_e   crit;

  value_type*       operator->() { return &this->item(); }
  const value_type* operator->() const { return &this->item(); }
  value_type&       operator*() { return this->item(); }
  const value_type& operator*() const { return this->item(); }

  SRSASN_CODE pack(bit_ref& bref) const
  {
    HANDLE_CODE(pack_integer(bref, id, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit.pack(bref));
    {
      varlength_field_pack_guard varlen_scope(bref, true);
      HANDLE_CODE(this->item().pack(bref));
    }
    return SRSASN_SUCCESS;
  }
  SRSASN_CODE unpack(cbit_ref& bref)
  {
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit.unpack(bref));
    {
      varlength_field_unpack_guard varlen_scope(bref, true);
      HANDLE_CODE(this->item().unpack(bref));
    }
    return SRSASN_SUCCESS;
  }
  void to_json(json_writer& j) const
  {
    j.start_obj();
    j.write_int("id", id);
    j.write_str("criticality", crit.to_string());
    j.write_fieldname(this->item_name());
    asn1::to_json(j, this->item());
    j.end_obj();
  }
};

} // namespace detail

template <typename T>
struct protocol_ie_container_item_s : public detail::base_ie_container_item<detail::ie_value_item<T>> {
  using base_type = detail::base_ie_container_item<detail::ie_value_item<T>>;
  using base_type::base_type;
};

template <typename T>
struct protocol_ext_container_item_s : public detail::base_ie_container_item<detail::ie_ext_item<T>> {
  using base_type = detail::base_ie_container_item<detail::ie_ext_item<T>>;
  using base_type::base_type;
};

// ProtocolIE-Container{LAYER-PROTOCOL-IES : IEsSetParam} ::= SEQUENCE (SIZE (0..65535)) OF ProtocolIE-Field
template <class IEsSetParam>
using protocol_ie_container_l = dyn_seq_of<protocol_ie_field_s<IEsSetParam>, 0, 65535, true>;

// ProtocolExtensionContainer{LAYER-PROTOCOL-EXTENSION : ExtensionSetParam} ::= SEQUENCE (SIZE (1..65535)) OF
// ProtocolExtensionField
template <class ExtensionSetParam>
using protocol_ext_container_l = dyn_seq_of<protocol_ext_field_s<ExtensionSetParam>, 1, 65535, true>;

namespace detail {

struct empty_obj_set_item_c {
  struct types_opts {
    enum options { nulltype } value;
    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  types       type() const { return types::nulltype; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct base_empty_obj_set {
  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

} // namespace detail

/// Empty Protocol IE Object Set
struct protocol_ies_empty_o : public detail::base_empty_obj_set {
  using value_c = detail::empty_obj_set_item_c;

  // members lookup methods
  static value_c get_value(uint32_t id) { return {}; }
};

/// Empty Protocol Extension Object Set
struct protocol_ext_empty_o : public detail::base_empty_obj_set {
  using ext_c = detail::empty_obj_set_item_c;

  // members lookup methods
  static ext_c get_ext(uint32_t id) { return {}; }
};

/// Empty ProtocolExtensionContainer
struct protocol_ie_container_empty_l {
  template <class extT_>
  using ie_field_s = protocol_ext_container_item_s<extT_>;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const
  {
    uint32_t nof_ies = 0;
    pack_length(bref, nof_ies, 1u, 65535u, true);
    return SRSASN_SUCCESS;
  }
  SRSASN_CODE unpack(cbit_ref& bref)
  {
    uint32_t nof_ies = 0;
    unpack_length(nof_ies, bref, 1u, 65535u, true);
    if (nof_ies > 0) {
      return SRSASN_ERROR_DECODE_FAIL;
    }
    return SRSASN_SUCCESS;
  }
  void to_json(json_writer& j) const
  {
    j.start_obj();
    j.end_obj();
  }
};

using protocol_ext_container_empty_l = protocol_ie_container_empty_l;

template <typename ProtocolIEs>
class elementary_procedure_option
{
  ProtocolIEs protocol_ies;

public:
  bool ext = false;
  // ...

  ProtocolIEs*       operator->() { return &protocol_ies; }
  const ProtocolIEs* operator->() const { return &protocol_ies; }
  ProtocolIEs&       operator*() { return protocol_ies; }
  const ProtocolIEs& operator*() const { return protocol_ies; }

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const
  {
    bref.pack(ext, 1);
    HANDLE_CODE(protocol_ies.pack(bref));
    return SRSASN_SUCCESS;
  }
  SRSASN_CODE unpack(cbit_ref& bref)
  {
    bref.unpack(ext, 1);
    HANDLE_CODE(protocol_ies.unpack(bref));
    return SRSASN_SUCCESS;
  }
  void to_json(json_writer& j) const
  {
    j.start_obj();
    j.write_fieldname("protocolIEs");
    asn1::to_json(j, protocol_ies);
    j.end_obj();
  }
};

} // namespace asn1
