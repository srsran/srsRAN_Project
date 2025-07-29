/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/asn1/asn1_utils.h"
#include "srsran/adt/bounded_bitset.h"
#include "srsran/asn1/asn1_ap_utils.h"

using srsran::byte_buffer;
using srsran::span;

namespace asn1 {

/************************
        logging
************************/

void warn_assert(bool cond, const char* filename, int lineno)
{
  if (cond) {
    log_warning("Assertion in [{}][{}] failed.", filename, lineno);
  }
}

void invalid_enum_number(int value, const char* name)
{
  log_error("The provided enum value={} of type {} cannot be translated into a number", value, name);
}

void assert_choice_type(uint32_t val, uint32_t choice_id)
{
  if (SRSRAN_UNLIKELY(val != choice_id)) {
    log_invalid_access_choice_id(val, choice_id);
  }
}

/************************
     error handling
************************/

void log_error_code(SRSASN_CODE code, const char* filename, int line)
{
  switch (code) {
    case SRSASN_ERROR_ENCODE_FAIL:
      log_error("[{}][{}] Encoding failure.", filename, line);
      break;
    case SRSASN_ERROR_DECODE_FAIL:
      log_error("[{}][{}] Decoding failure.", filename, line);
      break;
    default:
      log_warning("[{}][{}] SRSASN_CODE={} not recognized.", filename, line, (int)code);
  }
}

const char* convert_enum_idx(const char* array[], uint32_t nof_types, uint32_t enum_val, const char* enum_type)
{
  if (enum_val >= nof_types) {
    if (enum_val == nof_types) {
      log_error("The enum of type {} was not initialized.", enum_type);
    } else {
      log_error("The enum value={} of type {} is not valid.", enum_val, enum_type);
    }
    return "";
  }
  return array[enum_val];
}

bool convert_enum_str(const char* array[],
                      uint32_t    nof_types,
                      const char* str,
                      uint32_t&   enum_val,
                      const char* enum_type)
{
  for (uint32_t i = 0; i < nof_types; ++i) {
    if (strcmp(str, array[i]) == 0) {
      enum_val = i;
      return true;
    }
  }
  log_error("The string '{}' is not a valid value for enum type {}.", str, enum_type);
  return false;
}

template <class ItemType>
ItemType map_enum_number(ItemType* array, uint32_t nof_types, uint32_t enum_val, const char* enum_type)
{
  if (enum_val >= nof_types) {
    if (enum_val == nof_types) {
      log_error("The enum of type {} is not initialized.", enum_type);
    } else {
      log_error("The enum value={} of type {} cannot be converted to a number.", enum_val, enum_type);
    }
    return 0;
  }
  return array[enum_val];
}
template const uint8_t
map_enum_number<const uint8_t>(const uint8_t* array, uint32_t nof_types, uint32_t enum_val, const char* enum_type);
template const uint16_t
map_enum_number<const uint16_t>(const uint16_t* array, uint32_t nof_types, uint32_t enum_val, const char* enum_type);
template const uint32_t
map_enum_number<const uint32_t>(const uint32_t* array, uint32_t nof_types, uint32_t enum_val, const char* enum_type);
template const uint64_t
map_enum_number<const uint64_t>(const uint64_t* array, uint32_t nof_types, uint32_t enum_val, const char* enum_type);
template const int8_t
map_enum_number<const int8_t>(const int8_t* array, uint32_t nof_types, uint32_t enum_val, const char* enum_type);
template const int16_t
map_enum_number<const int16_t>(const int16_t* array, uint32_t nof_types, uint32_t enum_val, const char* enum_type);
template const int32_t
map_enum_number<const int32_t>(const int32_t* array, uint32_t nof_types, uint32_t enum_val, const char* enum_type);
template const int64_t
map_enum_number<const int64_t>(const int64_t* array, uint32_t nof_types, uint32_t enum_val, const char* enum_type);
template const float
map_enum_number<const float>(const float* array, uint32_t nof_types, uint32_t enum_val, const char* enum_type);

/*********************
       bit_ref
*********************/

int bit_ref::distance_bytes() const
{
  return static_cast<int>(writer.length());
}

int bit_ref::distance() const
{
  return 8 * (static_cast<int>(writer.length()) - (offset == 0 ? 0 : 1)) + offset;
}

SRSASN_CODE bit_ref::pack(uint64_t val, uint32_t n_bits)
{
  srsran_assert(n_bits < 64, "Invalid number of bits passed to pack()");
  while (n_bits > 0) {
    if (offset == 0) {
      if (not writer.append(0)) {
        return SRSASN_ERROR;
      }
    }
    uint64_t mask = ((1ul << n_bits) - 1ul); // bitmap of n_bits ones.
    val           = val & mask;
    if (static_cast<uint32_t>(8U - offset) > n_bits) {
      // in case n_bits < number of bits left in current byte (ie, last iteration).
      auto shifted_val = static_cast<uint8_t>(val << (8u - offset - n_bits));
      writer.back() += shifted_val;
      offset += n_bits;
      n_bits = 0;
    } else {
      // in case, space in current byte is lower or equal to n_bits (ie, not the last iteration).
      auto shifted_val = static_cast<uint8_t>(val >> (n_bits - 8u + offset));
      writer.back() += shifted_val;
      n_bits -= 8U - offset;
      offset = 0;
    }
  }
  return SRSASN_SUCCESS;
}

SRSASN_CODE bit_ref::pack_bytes(srsran::span<const uint8_t> bytes)
{
  if (bytes.empty()) {
    return SRSASN_SUCCESS;
  }
  if (offset == 0) {
    // Aligned case
    HANDLE_CODE(writer.append(bytes) ? SRSASN_SUCCESS : SRSASN_ERROR_ENCODE_FAIL);
  } else {
    for (uint8_t byte : bytes) {
      HANDLE_CODE(pack(byte, 8U));
    }
  }
  return SRSASN_SUCCESS;
}

SRSASN_CODE bit_ref::pack_bytes(srsran::byte_buffer_view bytes)
{
  if (bytes.empty()) {
    return SRSASN_SUCCESS;
  }
  if (offset == 0) {
    // Aligned case.
    HANDLE_CODE(writer.append(bytes) ? SRSASN_SUCCESS : SRSASN_ERROR_ENCODE_FAIL);
  } else {
    for (uint8_t byte : bytes) {
      HANDLE_CODE(pack(byte, 8U));
    }
  }
  return SRSASN_SUCCESS;
}

SRSASN_CODE bit_ref::align_bytes_zero()
{
  offset = 0;
  return SRSASN_SUCCESS;
}

cbit_ref cbit_ref::subview(uint32_t offset_bytes, uint32_t len_bytes) const
{
  const uint32_t buffer_rem_bytes = buffer.end() - it;
  const uint32_t nof_bytes_needed = len_bytes + ((offset != 0) ? 1 : 0); // account for the last bits in the last byte.
  if (offset_bytes + nof_bytes_needed > buffer_rem_bytes) {
    log_error("subview: Buffer size limit was achieved");
    return cbit_ref(srsran::byte_buffer_view{});
  }

  auto     start_it = it + offset_bytes;
  cbit_ref v{srsran::byte_buffer_view(start_it, start_it + nof_bytes_needed)};
  v.offset = offset;
  return v;
}

int cbit_ref::distance_bytes() const
{
  return static_cast<int>(it - buffer.begin()) + (offset != 0 ? 1 : 0);
}

int cbit_ref::distance() const
{
  return (int)offset + 8 * static_cast<int>(it - buffer.begin());
}

int cbit_ref::distance(const cbit_ref& other) const
{
  return static_cast<int>(it - other.it) * 8 + (static_cast<int>(offset) - static_cast<int>(other.offset));
}

template <class T>
SRSASN_CODE cbit_ref::unpack(T& val, uint32_t n_bits)
{
  srsran_assert(n_bits <= sizeof(T) * 8, "unpack_bits() only supports up to {} bits", sizeof(T) * 8);
  val = 0;
  while (n_bits > 0) {
    if (it == buffer.end()) {
      log_error("{}: Buffer size limit {} was reached.", __FUNCTION__, buffer.length());
      return SRSASN_ERROR_DECODE_FAIL;
    }
    if ((uint32_t)(8 - offset) > n_bits) {
      uint8_t mask = (uint8_t)(1u << (8u - offset)) - (uint8_t)(1u << (8u - offset - n_bits));
      val += ((uint32_t)((*it) & mask)) >> (8u - offset - n_bits);
      offset += n_bits;
      n_bits = 0;
    } else {
      auto mask = static_cast<uint8_t>((1u << (8u - offset)) - 1u);
      val += ((uint64_t)((*it) & mask)) << (n_bits - 8 + offset);
      n_bits -= 8 - offset;
      offset = 0;
      ++it;
    }
  }
  return SRSASN_SUCCESS;
}
template SRSASN_CODE cbit_ref::unpack<bool>(bool&, uint32_t n_bits);
template SRSASN_CODE cbit_ref::unpack<uint8_t>(uint8_t&, uint32_t n_bits);
template SRSASN_CODE cbit_ref::unpack<uint16_t>(uint16_t&, uint32_t n_bits);
template SRSASN_CODE cbit_ref::unpack<uint32_t>(uint32_t&, uint32_t n_bits);
template SRSASN_CODE cbit_ref::unpack<uint64_t>(uint64_t&, uint32_t n_bits);

SRSASN_CODE cbit_ref::unpack_bytes(srsran::span<uint8_t> bytes)
{
  if (bytes.empty()) {
    return SRSASN_SUCCESS;
  }
  if (static_cast<std::ptrdiff_t>(bytes.size()) > buffer.end() - it) {
    log_error("unpack_bytes: Buffer size limit was achieved");
    return SRSASN_ERROR_DECODE_FAIL;
  }
  if (offset == 0) {
    // Aligned case
    std::copy(buffer.begin(), buffer.end(), bytes.begin());
    it += bytes.size();
  } else {
    // Unaligned case
    for (uint32_t i = 0; i < bytes.size(); ++i) {
      HANDLE_CODE(unpack(bytes[i], 8));
    }
  }
  return SRSASN_SUCCESS;
}

SRSASN_CODE cbit_ref::align_bytes()
{
  if (offset != 0) {
    if (it == buffer.end()) {
      log_error("{}: Buffer size limit {} was achieved.", __FUNCTION__, buffer.length());
      return SRSASN_ERROR_DECODE_FAIL;
    }
    ++it;
    offset = 0;
  }
  return SRSASN_SUCCESS;
}

SRSASN_CODE cbit_ref::advance_bits(uint32_t n_bits)
{
  uint32_t extra_bits     = (offset + n_bits) % 8;
  uint32_t bytes_required = ceil_frac(offset + n_bits, 8U);
  uint32_t bytes_offset   = (offset + n_bits) / 8U;

  if (bytes_required > buffer.end() - it) {
    log_error("advance_bytes: Buffer size limit was achieved");
    return SRSASN_ERROR_DECODE_FAIL;
  }
  it += bytes_offset;
  offset = extra_bits;
  return SRSASN_SUCCESS;
}

SRSASN_CODE cbit_ref::advance_bytes(uint32_t bytes)
{
  if (bytes + (offset != 0 ? 1 : 0) > buffer.end() - it) {
    log_error("advance_bytes: Buffer size limit was achieved");
    return SRSASN_ERROR_DECODE_FAIL;
  }
  it += bytes;
  return SRSASN_SUCCESS;
}

/*********************
     ext packing
*********************/

SRSASN_CODE pack_unsupported_ext_flag(bit_ref& bref, bool ext)
{
  HANDLE_CODE(bref.pack(ext, 1));
  if (ext) {
    log_error("ASN extensions not currently supported");
    return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

SRSASN_CODE unpack_unsupported_ext_flag(bool& ext, cbit_ref& bref)
{
  SRSASN_CODE ret = bref.unpack(ext, 1);
  if (ext) {
    log_error("ASN extensions not currently supported");
    return SRSASN_ERROR_DECODE_FAIL;
  }
  return ret;
}

/************************
      enum packing
************************/

// Pack without extension
SRSASN_CODE pack_enum(bit_ref& bref, uint32_t enum_val, uint32_t nbits)
{
  return bref.pack(enum_val, nbits);
}

// Pack with extension
SRSASN_CODE pack_enum(bit_ref& bref, uint32_t enum_val, uint32_t nbits, uint32_t nof_noext)
{
  bool ext = enum_val >= nof_noext;
  HANDLE_CODE(bref.pack((bool)ext, 1));
  SRSASN_CODE ret;
  if (not ext) {
    ret = bref.pack(enum_val, nbits);
  } else {
    ret = pack_norm_small_non_neg_whole_number(bref, enum_val - nof_noext);
  }
  return ret;
}

SRSASN_CODE pack_enum(bit_ref& bref, uint32_t e, uint32_t nof_types, uint32_t nof_exts, bool has_ext)
{
  if (e >= nof_types) {
    log_error(
        "The provided enum is not within the range of possible values ({}>={})", (unsigned)e, (unsigned)nof_types);
    return SRSASN_ERROR_ENCODE_FAIL;
  }
  SRSASN_CODE ret;
  if (has_ext) {
    uint32_t nof_bits = (uint32_t)ceilf(log2f(nof_types - nof_exts));
    ret               = pack_enum(bref, e, nof_bits, nof_types - nof_exts);
  } else {
    uint32_t nof_bits = (uint32_t)ceilf(log2f(nof_types));
    ret               = pack_enum(bref, e, nof_bits);
  }
  return ret;
}

ValOrError unpack_enum(uint32_t nof_types, uint32_t nof_exts, bool has_ext, cbit_ref& bref)
{
  ValOrError ret;
  if (has_ext) {
    uint32_t nof_bits = (uint32_t)ceilf(log2f(nof_types - nof_exts));
    bool     ext;
    ret.code = bref.unpack(ext, 1);
    if (ret.code != SRSASN_SUCCESS) {
      return ret;
    }
    if (not ext) {
      ret.code = bref.unpack(ret.val, nof_bits);
    } else {
      ret.code = unpack_norm_small_non_neg_whole_number(ret.val, bref);
      ret.val += nof_types - nof_exts;
    }
  } else {
    uint32_t nof_bits = (uint32_t)ceilf(log2f(nof_types));
    ret.code          = bref.unpack(ret.val, nof_bits);
  }
  if (ret.val >= nof_types) {
    log_error("The provided enum is not within the range of possible values ({}>={})",
              (unsigned)ret.val,
              (unsigned)nof_types);
    ret.code = SRSASN_ERROR_DECODE_FAIL;
  }
  return ret;
}

/************************
     PER encoding
************************/

/**
 * X.691 - Section 10.5
 * Encoder function for a constrained whole number
 * @tparam IntType type of integer (e.g. uint8_t, uint32_t, etc.)
 * @param bref bit_ref where to encode integer
 * @param n integer current value
 * @param lb lower bound
 * @param ub upper bound
 * @return success or failure
 */
template <class IntType>
SRSASN_CODE pack_constrained_whole_number(bit_ref& bref, IntType n, IntType lb, IntType ub, bool aligned)
{
  if (ub < lb or n < lb or n > ub) {
    log_error("The condition lb <= n <= ub ({} <= {} <= {}) was not met", (long)lb, (long)n, (long)ub);
    return SRSASN_ERROR_ENCODE_FAIL;
  }
  uint64_t ra = (uint64_t)(ub - lb) + 1; // NOTE: Can overflow if IntType is kept
  if (ra == 1) {
    return SRSASN_SUCCESS;
  }
  uint32_t n_bits   = (uint32_t)ceilf(log2f((float)ra)); // bit-field size
  IntType  toencode = n - lb;
  if (not aligned) {
    // UNALIGNED variant
    bref.pack(toencode, n_bits);
  } else {
    // ALIGNED variant
    SRSASN_CODE ret;
    if (ra < 256) {
      ret = bref.pack(toencode, n_bits);
    } else if (ra <= ASN_64K) {
      // both one and two octets cases
      uint32_t n_octets = ceil_frac(n_bits, 8u);
      HANDLE_CODE(bref.align_bytes_zero());
      HANDLE_CODE(bref.pack(0, n_octets * 8 - n_bits));
      HANDLE_CODE(bref.pack(toencode, n_bits));
      ret = bref.align_bytes_zero();
    } else {
      uint32_t n_bits_len = (uint32_t)ceilf(log2f(ceil_frac(n_bits, 8u)));
      n_bits              = (uint32_t)floorf(log2f(std::max(toencode, (IntType)1)) + 1);
      uint32_t n_octets   = ceil_frac(n_bits, 8u);
      HANDLE_CODE(bref.pack(n_octets - 1, n_bits_len));
      HANDLE_CODE(bref.align_bytes_zero());
      ret = bref.pack(toencode, n_octets * 8u);
    }
    return ret;
  }
  return SRSASN_SUCCESS;
}
template SRSASN_CODE pack_constrained_whole_number<int8_t>(bit_ref& bref, int8_t n, int8_t lb, int8_t ub, bool aligned);
template SRSASN_CODE
pack_constrained_whole_number<int16_t>(bit_ref& bref, int16_t n, int16_t lb, int16_t ub, bool aligned);
template SRSASN_CODE
pack_constrained_whole_number<int32_t>(bit_ref& bref, int32_t n, int32_t lb, int32_t ub, bool aligned);
template SRSASN_CODE
pack_constrained_whole_number<int64_t>(bit_ref& bref, int64_t n, int64_t lb, int64_t ub, bool aligned);
template SRSASN_CODE
pack_constrained_whole_number<uint8_t>(bit_ref& bref, uint8_t n, uint8_t lb, uint8_t ub, bool aligned);
template SRSASN_CODE
pack_constrained_whole_number<uint16_t>(bit_ref& bref, uint16_t n, uint16_t lb, uint16_t ub, bool aligned);
template SRSASN_CODE
pack_constrained_whole_number<uint32_t>(bit_ref& bref, uint32_t n, uint32_t lb, uint32_t ub, bool aligned);
template SRSASN_CODE
pack_constrained_whole_number<uint64_t>(bit_ref& bref, uint64_t n, uint64_t lb, uint64_t ub, bool aligned);

/**
 * X.691 - Section 10.5
 * Decoder function for a constrained whole number
 * @tparam IntType type of integer (e.g. uint8_t, uint32_t, etc.)
 * @param n integer with decoded value
 * @param bref bit_ref where to encode integer
 * @param lb lower bound
 * @param ub upper bound
 * @return success or failure
 */
template <class IntType>
SRSASN_CODE unpack_constrained_whole_number(IntType& n, cbit_ref& bref, IntType lb, IntType ub, bool aligned)
{
  if (ub < lb) {
    log_error("The condition lb <= ub ({} <= {}) was not met", (long)lb, (long)ub);
    return SRSASN_ERROR_DECODE_FAIL;
  }
  uint64_t ra = (uint64_t)(ub - lb) + 1; // NOTE: Can overflow if IntType is kept.
  if (ra == 1) {
    n = lb;
    return SRSASN_SUCCESS;
  }
  uint32_t n_bits = (uint32_t)ceilf(log2f((float)ra));
  if (not aligned) {
    // UNALIGNED variant
    HANDLE_CODE(bref.unpack(n, n_bits));
    n += lb;
  } else {
    // ALIGNED variant
    if (ra < 256) {
      HANDLE_CODE(bref.unpack(n, n_bits));
    } else if (ra <= ASN_64K) {
      uint32_t n_octets = ceil_frac(n_bits, 8u);
      HANDLE_CODE(bref.align_bytes());
      HANDLE_CODE(bref.unpack(n, n_octets * 8));
      HANDLE_CODE(bref.align_bytes());
    } else {
      uint32_t n_bits_len = (uint32_t)ceilf(log2f(ceil_frac(n_bits, 8u)));
      uint32_t n_octets;
      HANDLE_CODE(bref.unpack(n_octets, n_bits_len));
      n_octets += 1;
      HANDLE_CODE(bref.align_bytes());
      HANDLE_CODE(bref.unpack(n, n_octets * 8));
    }
    n += lb;
  }

  if (n > ub) {
    log_error("The condition lb <= n <= ub ({} <= {} <= {}) was not met\n", lb, n, ub);
    return SRSASN_ERROR_DECODE_FAIL;
  }

  return SRSASN_SUCCESS;
}
template SRSASN_CODE
unpack_constrained_whole_number<int8_t>(int8_t& n, cbit_ref& bref, int8_t lb, int8_t ub, bool aligned);
template SRSASN_CODE
unpack_constrained_whole_number<int16_t>(int16_t& n, cbit_ref& bref, int16_t lb, int16_t ub, bool aligned);
template SRSASN_CODE
unpack_constrained_whole_number<int32_t>(int32_t& n, cbit_ref& bref, int32_t lb, int32_t ub, bool aligned);
template SRSASN_CODE
unpack_constrained_whole_number<int64_t>(int64_t& n, cbit_ref& bref, int64_t lb, int64_t ub, bool aligned);
template SRSASN_CODE
unpack_constrained_whole_number<uint8_t>(uint8_t& n, cbit_ref& bref, uint8_t lb, uint8_t ub, bool aligned);
template SRSASN_CODE
unpack_constrained_whole_number<uint16_t>(uint16_t& n, cbit_ref& bref, uint16_t lb, uint16_t ub, bool aligned);
template SRSASN_CODE
unpack_constrained_whole_number<uint32_t>(uint32_t& n, cbit_ref& bref, uint32_t lb, uint32_t ub, bool aligned);
template SRSASN_CODE
unpack_constrained_whole_number<uint64_t>(uint64_t& n, cbit_ref& bref, uint64_t lb, uint64_t ub, bool aligned);

/**
 * X.691 - Section 10.6
 * Encoder function for a normally small non-negative whole number
 * @tparam IntType type of integer (e.g. uint8_t, uint32_t, etc.)
 * @param bref bit_ref where to encode integer
 * @param n integer current value
 * @return success or failure
 */
template <typename UintType>
SRSASN_CODE pack_norm_small_non_neg_whole_number(bit_ref& bref, UintType n)
{
  if (n <= 63) {
    HANDLE_CODE(bref.pack(n, 7)); // [1 bit: 0 | 6 bit: n]
  } else {
    HANDLE_CODE(bref.pack(1, 1));
    log_error("Long small integers not supported");
    return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
template <typename UintType>
SRSASN_CODE unpack_norm_small_non_neg_whole_number(UintType& n, cbit_ref& bref)
{
  bool        ext;
  SRSASN_CODE ret = bref.unpack(ext, 1);
  HANDLE_CODE(ret);
  if (not ext) {
    ret = bref.unpack(n, 6);
  } else {
    log_error("Long small integers not supported");
    return SRSASN_ERROR_DECODE_FAIL;
  }
  return ret;
}
template SRSASN_CODE pack_norm_small_non_neg_whole_number<uint8_t>(bit_ref& bref, uint8_t n);
template SRSASN_CODE pack_norm_small_non_neg_whole_number<uint16_t>(bit_ref& bref, uint16_t n);
template SRSASN_CODE pack_norm_small_non_neg_whole_number<uint32_t>(bit_ref& bref, uint32_t n);
template SRSASN_CODE pack_norm_small_non_neg_whole_number<uint64_t>(bit_ref& bref, uint64_t n);
template SRSASN_CODE unpack_norm_small_non_neg_whole_number<uint8_t>(uint8_t& n, cbit_ref& bref);
template SRSASN_CODE unpack_norm_small_non_neg_whole_number<uint16_t>(uint16_t& n, cbit_ref& bref);
template SRSASN_CODE unpack_norm_small_non_neg_whole_number<uint32_t>(uint32_t& n, cbit_ref& bref);
template SRSASN_CODE unpack_norm_small_non_neg_whole_number<uint64_t>(uint64_t& n, cbit_ref& bref);

template <typename IntType>
IntType unconstrained_whole_number_length(IntType n)
{
  return (IntType)ceilf((log2f(n) + 1) / 8.0f);
}

/**
 * X.691 - Section 10.8
 * Encoder function for an unconstrained whole number
 * @tparam IntType type of integer (e.g. int8_t, uint32_t, etc.)
 * @param bref bit_ref where to encode integer
 * @param n integer current value
 * @return success or failure
 */
template <typename IntType>
SRSASN_CODE pack_unconstrained_whole_number(bit_ref& bref, IntType n, bool aligned)
{
  uint32_t len = unconstrained_whole_number_length(n);
  if (aligned) {
    HANDLE_CODE(bref.align_bytes_zero());
  }
  HANDLE_CODE(bref.pack(n, len * 8));

  return SRSASN_SUCCESS;
}
template <typename IntType>
SRSASN_CODE unpack_unconstrained_whole_number(IntType& n, cbit_ref& bref, uint32_t len, bool aligned)
{
  if (aligned) {
    HANDLE_CODE(bref.align_bytes());
  }
  HANDLE_CODE(bref.unpack(n, len * 8));

  return SRSASN_SUCCESS;
}
template SRSASN_CODE pack_unconstrained_whole_number<int8_t>(bit_ref& bref, int8_t n, bool aligned);
template SRSASN_CODE pack_unconstrained_whole_number<int16_t>(bit_ref& bref, int16_t n, bool aligned);
template SRSASN_CODE pack_unconstrained_whole_number<int32_t>(bit_ref& bref, int32_t n, bool aligned);
template SRSASN_CODE pack_unconstrained_whole_number<int64_t>(bit_ref& bref, int64_t n, bool aligned);
template SRSASN_CODE unpack_unconstrained_whole_number<int8_t>(int8_t& n, cbit_ref& bref, uint32_t len, bool aligned);
template SRSASN_CODE unpack_unconstrained_whole_number<int16_t>(int16_t& n, cbit_ref& bref, uint32_t len, bool aligned);
template SRSASN_CODE unpack_unconstrained_whole_number<int32_t>(int32_t& n, cbit_ref& bref, uint32_t len, bool aligned);
template SRSASN_CODE unpack_unconstrained_whole_number<int64_t>(int64_t& n, cbit_ref& bref, uint32_t len, bool aligned);
template SRSASN_CODE pack_unconstrained_whole_number<uint8_t>(bit_ref& bref, uint8_t n, bool aligned);
template SRSASN_CODE pack_unconstrained_whole_number<uint16_t>(bit_ref& bref, uint16_t n, bool aligned);
template SRSASN_CODE pack_unconstrained_whole_number<uint32_t>(bit_ref& bref, uint32_t n, bool aligned);
template SRSASN_CODE pack_unconstrained_whole_number<uint64_t>(bit_ref& bref, uint64_t n, bool aligned);
template SRSASN_CODE unpack_unconstrained_whole_number<uint8_t>(uint8_t& n, cbit_ref& bref, uint32_t len, bool aligned);
template SRSASN_CODE
unpack_unconstrained_whole_number<uint16_t>(uint16_t& n, cbit_ref& bref, uint32_t len, bool aligned);
template SRSASN_CODE
unpack_unconstrained_whole_number<uint32_t>(uint32_t& n, cbit_ref& bref, uint32_t len, bool aligned);
template SRSASN_CODE
unpack_unconstrained_whole_number<uint64_t>(uint64_t& n, cbit_ref& bref, uint32_t len, bool aligned);

/*********************
   varlength_packing
*********************/

template <typename IntType>
SRSASN_CODE pack_length(bit_ref& bref, IntType n, IntType lb, IntType ub, bool aligned)
{
  if (ub >= ASN_64K) {
    return pack_length(bref, n, aligned);
  }
  return pack_constrained_whole_number(bref, n, lb, ub, aligned);
}
template SRSASN_CODE pack_length<uint8_t>(bit_ref& bref, uint8_t n, uint8_t lb, uint8_t ub, bool aligned);
template SRSASN_CODE pack_length<uint16_t>(bit_ref& bref, uint16_t n, uint16_t lb, uint16_t ub, bool aligned);
template SRSASN_CODE pack_length<uint32_t>(bit_ref& bref, uint32_t n, uint32_t lb, uint32_t ub, bool aligned);
template SRSASN_CODE pack_length<uint64_t>(bit_ref& bref, uint64_t n, uint64_t lb, uint64_t ub, bool aligned);
template SRSASN_CODE pack_length<int8_t>(bit_ref& bref, int8_t n, int8_t lb, int8_t ub, bool aligned);
template SRSASN_CODE pack_length<int16_t>(bit_ref& bref, int16_t n, int16_t lb, int16_t ub, bool aligned);
template SRSASN_CODE pack_length<int32_t>(bit_ref& bref, int32_t n, int32_t lb, int32_t ub, bool aligned);
template SRSASN_CODE pack_length<int64_t>(bit_ref& bref, int64_t n, int64_t lb, int64_t ub, bool aligned);

template <typename IntType>
SRSASN_CODE unpack_length(IntType& n, cbit_ref& bref, IntType lb, IntType ub, bool aligned)
{
  if (ub >= ASN_64K) {
    uint32_t    len;
    SRSASN_CODE ret = unpack_length(len, bref, aligned);
    n               = len;
    return ret;
  }
  return unpack_constrained_whole_number(n, bref, lb, ub, aligned);
}
template SRSASN_CODE unpack_length<uint8_t>(uint8_t& n, cbit_ref& bref, uint8_t lb, uint8_t ub, bool aligned);
template SRSASN_CODE unpack_length<uint16_t>(uint16_t& n, cbit_ref& bref, uint16_t lb, uint16_t ub, bool aligned);
template SRSASN_CODE unpack_length<uint32_t>(uint32_t& n, cbit_ref& bref, uint32_t lb, uint32_t ub, bool aligned);
template SRSASN_CODE unpack_length<uint64_t>(uint64_t& n, cbit_ref& bref, uint64_t lb, uint64_t ub, bool aligned);
template SRSASN_CODE unpack_length<int8_t>(int8_t& n, cbit_ref& bref, int8_t lb, int8_t ub, bool aligned);
template SRSASN_CODE unpack_length<int16_t>(int16_t& n, cbit_ref& bref, int16_t lb, int16_t ub, bool aligned);
template SRSASN_CODE unpack_length<int32_t>(int32_t& n, cbit_ref& bref, int32_t lb, int32_t ub, bool aligned);
template SRSASN_CODE unpack_length<int64_t>(int64_t& n, cbit_ref& bref, int64_t lb, int64_t ub, bool aligned);

/**
 * X.691 - Section 10.9
 * Pack unconstrained length determinant
 * @param bref
 * @param val "n" length value to pack
 * @param aligned
 * @return
 */
SRSASN_CODE pack_length(bit_ref& bref, uint32_t val, bool aligned)
{
  if (not aligned) {
    if (val < 128) {
      HANDLE_CODE(bref.pack(val, 8)); // first bit is 0
    } else if (val < ASN_16K) {
      HANDLE_CODE(bref.pack(0b10, 2));
      HANDLE_CODE(bref.pack(val, 14));
    } else {
      log_error("Not handling sizes longer than 16383 octets");
      return SRSASN_ERROR_ENCODE_FAIL;
    }
  } else {
    if (val > ASN_64K) {
      // TODO: Error message
      return SRSASN_ERROR_ENCODE_FAIL;
    }
    HANDLE_CODE(bref.align_bytes_zero());
    if (val < 128) {
      return bref.pack(val, 8); // first bit is 0
    } else if (val < ASN_16K) {
      HANDLE_CODE(bref.pack(0b10, 2));
      return bref.pack(val, 14);
    } else {
      HANDLE_CODE(bref.pack(0b11, 2));
      uint32_t m = val / ASN_16K;
      HANDLE_CODE(bref.pack(m, 6));
      val = val - m * ASN_16K;
      return bref.pack(val, 16); // TODO: TODO
    }
  }
  return SRSASN_SUCCESS;
}

SRSASN_CODE unpack_length(uint32_t& val, cbit_ref& bref, bool aligned)
{
  bool ext;
  if (not aligned) {
    SRSASN_CODE ret = bref.unpack(ext, 1);
    HANDLE_CODE(ret);
    if (not ext) {
      ret = bref.unpack(val, 7);
    } else {
      ret = bref.unpack(ext, 1);
      HANDLE_CODE(ret);
      if (not ext) {
        ret = bref.unpack(val, 14);
      } else {
        log_error("Not handling octet strings longer than 16383 octets");
        val = 0;
        return SRSASN_ERROR_DECODE_FAIL;
      }
    }
    return ret;
  } else {
    HANDLE_CODE(bref.align_bytes());
    HANDLE_CODE(bref.unpack(val, 8));
    if (val < 128) {
      // "n" < 128
      // first bit was set to zero
      return SRSASN_SUCCESS;
    }
    val -= 128;
    if (val < 64) {
      // 128 <= "n" < 16K
      // second bit is set to zero as well
      uint32_t val_octet_2;
      HANDLE_CODE(bref.unpack(val_octet_2, 8));
      val = (val << 8u) + val_octet_2;
      return SRSASN_SUCCESS;
    }
    log_error("Not handling octet strings longer than 16383 octets");
    return SRSASN_ERROR_DECODE_FAIL;
  }
}

/************************
        Integer
************************/

template <typename IntType>
SRSASN_CODE pack_integer(bit_ref& bref, IntType n, IntType lb, IntType ub, bool has_ext, bool aligned)
{
  // pack ext marker
  bool within_bounds = n >= lb and n <= ub;
  if (has_ext) {
    HANDLE_CODE(bref.pack(not within_bounds, 1));
  } else if (not within_bounds) {
    log_error("The condition lb <= n <= ub ({} <= {} <= {}) was not met", (long)lb, (long)n, (long)ub);
    return SRSASN_ERROR_ENCODE_FAIL;
  }
  bool lower_bounded = lb != std::numeric_limits<IntType>::min() or std::is_unsigned<IntType>::value;
  bool upper_bounded = ub != std::numeric_limits<IntType>::max();

  if (within_bounds and lower_bounded and upper_bounded) {
    // constrained case
    if (lb == ub) {
      return SRSASN_SUCCESS;
    }
    // TODO: Check if we are in the indefinite length case, and pack length prefix if needed
    //    if(indefinite_length) {
    //      HANDLE_CODE(pack_length(bref, n, 1, ceilf(log2f()), aligned));
    //    }
    HANDLE_CODE(pack_constrained_whole_number(bref, n, (IntType)lb, (IntType)ub, aligned));
  } else {
    // See X.691 - 12.2.6
    if (not within_bounds or (not lower_bounded and not upper_bounded)) {
      HANDLE_CODE(pack_length(bref, unconstrained_whole_number_length(n), aligned));
      HANDLE_CODE(pack_unconstrained_whole_number(bref, n, aligned));
    } else {
      // pack as semi-constrained
      // TODO
    }
  }
  return SRSASN_SUCCESS;
}
template SRSASN_CODE
pack_integer<uint8_t>(bit_ref& bref, uint8_t n, uint8_t lb, uint8_t ub, bool has_ext, bool aligned);
template SRSASN_CODE
pack_integer<uint16_t>(bit_ref& bref, uint16_t n, uint16_t lb, uint16_t ub, bool has_ext, bool aligned);
template SRSASN_CODE
pack_integer<uint32_t>(bit_ref& bref, uint32_t n, uint32_t lb, uint32_t ub, bool has_ext, bool aligned);
template SRSASN_CODE
pack_integer<uint64_t>(bit_ref& bref, uint64_t n, uint64_t lb, uint64_t ub, bool has_ext, bool aligned);
template SRSASN_CODE pack_integer<int8_t>(bit_ref& bref, int8_t n, int8_t lb, int8_t ub, bool has_ext, bool aligned);
template SRSASN_CODE
pack_integer<int16_t>(bit_ref& bref, int16_t n, int16_t lb, int16_t ub, bool has_ext, bool aligned);
template SRSASN_CODE
pack_integer<int32_t>(bit_ref& bref, int32_t n, int32_t lb, int32_t ub, bool has_ext, bool aligned);
template SRSASN_CODE
pack_integer<int64_t>(bit_ref& bref, int64_t n, int64_t lb, int64_t ub, bool has_ext, bool aligned);

// unconstrained specialization case
template <typename IntType>
SRSASN_CODE pack_unconstrained_integer(bit_ref& bref, IntType n, bool has_ext, bool aligned)
{
  return pack_integer(
      bref, n, std::numeric_limits<IntType>::min(), std::numeric_limits<IntType>::max(), has_ext, aligned);
}
template SRSASN_CODE pack_unconstrained_integer<int8_t>(bit_ref& bref, int8_t n, bool has_ext, bool aligned);
template SRSASN_CODE pack_unconstrained_integer<int16_t>(bit_ref& bref, int16_t n, bool has_ext, bool aligned);
template SRSASN_CODE pack_unconstrained_integer<int32_t>(bit_ref& bref, int32_t n, bool has_ext, bool aligned);
template SRSASN_CODE pack_unconstrained_integer<int64_t>(bit_ref& bref, int64_t n, bool has_ext, bool aligned);

template <typename IntType>
SRSASN_CODE unpack_integer(IntType& n, cbit_ref& bref, IntType lb, IntType ub, bool has_ext, bool aligned)
{
  bool within_bounds = true;
  if (has_ext) {
    HANDLE_CODE(bref.unpack(within_bounds, 1));
    within_bounds = not within_bounds;
  }
  bool lower_bounded = lb != std::numeric_limits<IntType>::min() or std::is_unsigned<IntType>::value;
  bool upper_bounded = ub != std::numeric_limits<IntType>::max();

  if (within_bounds and lower_bounded and upper_bounded) {
    // constrained case
    if (lb == ub) {
      return SRSASN_SUCCESS;
    }
    // TODO: Check if we are in the indefinite length case, and pack length prefix if needed
    HANDLE_CODE(unpack_constrained_whole_number(n, bref, (IntType)lb, (IntType)ub, aligned));
  } else {
    // See X.691 - 12.2.6
    if (not within_bounds or (not lower_bounded and not upper_bounded)) {
      uint32_t len;
      HANDLE_CODE(unpack_length(len, bref, aligned));
      HANDLE_CODE(unpack_unconstrained_whole_number(n, bref, len, aligned));
    } else {
      // pack as semi-constrained
      // TODO
    }
  }
  return SRSASN_SUCCESS;
}
template SRSASN_CODE
unpack_integer<uint8_t>(uint8_t& n, cbit_ref& bref, uint8_t lb, uint8_t ub, bool has_ext, bool aligned);
template SRSASN_CODE
unpack_integer<uint16_t>(uint16_t& n, cbit_ref& bref, uint16_t lb, uint16_t ub, bool has_ext, bool aligned);
template SRSASN_CODE
unpack_integer<uint32_t>(uint32_t& n, cbit_ref& bref, uint32_t lb, uint32_t ub, bool has_ext, bool aligned);
template SRSASN_CODE
unpack_integer<uint64_t>(uint64_t& n, cbit_ref& bref, uint64_t lb, uint64_t ub, bool has_ext, bool aligned);
template SRSASN_CODE
unpack_integer<int8_t>(int8_t& n, cbit_ref& bref, int8_t lb, int8_t ub, bool has_ext, bool aligned);
template SRSASN_CODE
unpack_integer<int16_t>(int16_t& n, cbit_ref& bref, int16_t lb, int16_t ub, bool has_ext, bool aligned);
template SRSASN_CODE
unpack_integer<int32_t>(int32_t& n, cbit_ref& bref, int32_t lb, int32_t ub, bool has_ext, bool aligned);
template SRSASN_CODE
unpack_integer<int64_t>(int64_t& n, cbit_ref& bref, int64_t lb, int64_t ub, bool has_ext, bool aligned);

// unconstrained specialization case
template <typename IntType>
SRSASN_CODE unpack_unconstrained_integer(IntType& n, cbit_ref& bref, bool has_ext, bool aligned)
{
  return unpack_integer(
      n, bref, std::numeric_limits<IntType>::min(), std::numeric_limits<IntType>::max(), has_ext, aligned);
}
template SRSASN_CODE unpack_unconstrained_integer<int8_t>(int8_t& n, cbit_ref& bref, bool has_ext, bool aligned);
template SRSASN_CODE unpack_unconstrained_integer<int16_t>(int16_t& n, cbit_ref& bref, bool has_ext, bool aligned);
template SRSASN_CODE unpack_unconstrained_integer<int32_t>(int32_t& n, cbit_ref& bref, bool has_ext, bool aligned);
template SRSASN_CODE unpack_unconstrained_integer<int64_t>(int64_t& n, cbit_ref& bref, bool has_ext, bool aligned);

// standalone packer
template <class IntType>
integer_packer<IntType>::integer_packer(IntType lb_, IntType ub_, bool has_ext_, bool aligned_) :
  lb(lb_), ub(ub_), has_ext(has_ext_), aligned(aligned_)
{
}

template <class IntType>
SRSASN_CODE integer_packer<IntType>::pack(bit_ref& bref, IntType n)
{
  return pack_integer(bref, n, lb, ub, has_ext, aligned);
}
template <class IntType>
SRSASN_CODE integer_packer<IntType>::unpack(IntType& n, cbit_ref& bref)
{
  return unpack_integer(n, bref, lb, ub, has_ext, aligned);
}
template struct integer_packer<int8_t>;
template struct integer_packer<int16_t>;
template struct integer_packer<int32_t>;
template struct integer_packer<int64_t>;
template struct integer_packer<uint8_t>;
template struct integer_packer<uint16_t>;
template struct integer_packer<uint32_t>;
template struct integer_packer<uint64_t>;

/************************
    common octstring
************************/

uint64_t octet_string_helper::to_uint(srsran::span<const uint8_t> buf)
{
  size_t nbytes = buf.size();
  if (nbytes > 8U) {
    log_error("OCTET STRING of size={} does not fit in an uint64_t", nbytes);
    return 0;
  }
  uint64_t val = 0;
  for (unsigned i = 0; i < nbytes; ++i) {
    val += ((uint64_t)buf[nbytes - 1 - i]) << (uint64_t)(i * 8U);
  }
  return val;
}

uint64_t octet_string_helper::to_uint(const byte_buffer& buf)
{
  size_t nbytes = buf.length();
  if (nbytes > 8) {
    log_error("OCTET STRING with size={} does not fit in an uint64_t", nbytes);
    return 0;
  }
  uint64_t val = 0;
  auto     it  = buf.begin();
  for (unsigned i = 0; i < nbytes; ++i) {
    val |= (static_cast<uint64_t>(*it) & 0xffu) << static_cast<uint64_t>(i * 8U);
    ++it;
  }
  return val;
}

void octet_string_helper::to_octet_string(srsran::span<uint8_t> buf, uint64_t number)
{
  uint64_t nbytes = buf.size();
  if (nbytes < 8 and (static_cast<uint64_t>(1U) << (8U * nbytes)) <= number) {
    log_error("Integer={} does not fit in an OCTET STRING of size={}", number, nbytes);
    return;
  }
  for (uint32_t i = 0; i < nbytes; ++i) {
    buf[nbytes - 1 - i] = (number >> (uint64_t)(i * 8U)) & 0xffu;
  }
}

void octet_string_helper::to_octet_string(srsran::byte_buffer& buf, uint64_t number)
{
  buf           = byte_buffer{byte_buffer::fallback_allocation_tag{}};
  size_t nbytes = sizeof(number);
  for (uint32_t i = 0; i < nbytes; ++i) {
    if (not buf.append((number >> (uint64_t)((nbytes - 1 - i) * 8U)) & 0xffu)) {
      log_error("Failed to append octet string byte to buffer");
      return;
    }
  }
}

static void to_hex(char* cstr, uint8_t val)
{
  sprintf(cstr, "%02x", val);
}

std::string octet_string_helper::to_hex_string(srsran::span<const uint8_t> buf)
{
  std::string s;
  s.resize(buf.size() * 2);
  char cstr[3];
  for (uint32_t i = 0; i < buf.size(); i++) {
    to_hex(cstr, buf[i]);
    s.replace(i * 2, 2, cstr);
  }
  return s;
}

std::string octet_string_helper::to_hex_string(const byte_buffer& buf)
{
  std::string s;
  s.resize(buf.length() * 2);
  char     cstr[3];
  unsigned i = 0;
  for (uint8_t b : buf) {
    to_hex(cstr, b);
    s.replace(i * 2, 2, cstr);
    ++i;
  }
  return s;
}

unsigned octet_string_helper::hex_string_to_octets(srsran::span<uint8_t> buf, const std::string& str)
{
  srsran_assert(buf.size() >= ceil_frac(str.size(), (size_t)2U), "out-of-bounds access");
  if (str.size() % 2 != 0) {
    log_error("The provided hex string size={} is not a multiple of 2.", str.size());
  }
  char cstr[] = "\0\0\0";
  for (unsigned i = 0; i < str.size(); i += 2) {
    memcpy(&cstr[0], &str[i], 2);
    buf[i / 2] = strtoul(cstr, nullptr, 16);
  }
  return (str.size() + 1) / 2;
}

void octet_string_helper::append_hex_string(byte_buffer& buf, const std::string& str)
{
  if (str.size() % 2 != 0) {
    log_error("The provided hex string size={} is not a multiple of 2.", str.size());
  }
  char cstr[] = "\0\0\0";
  for (unsigned i = 0; i < str.size(); i += 2) {
    memcpy(&cstr[0], &str[i], 2);
    if (not buf.append(strtoul(cstr, nullptr, 16))) {
      log_error("Failed to append octet string byte to buffer");
    }
  }
}

/************************
     dyn_octstring
************************/

template <bool Al>
unbounded_octstring<Al>::unbounded_octstring(const unbounded_octstring& other) noexcept :
  // Use fallback allocator, because operator= should never fail.
  srsran::byte_buffer(fallback_allocation_tag{}, other)
{
}

template <bool Al>
unbounded_octstring<Al>& unbounded_octstring<Al>::operator=(const unbounded_octstring& other) noexcept
{
  if (this != &other) {
    // Use fallback allocator, because operator= should never fail.
    *this = byte_buffer{fallback_allocation_tag{}, other};
  }
  return *this;
}

template <bool Al>
unbounded_octstring<Al>& unbounded_octstring<Al>::operator=(unbounded_octstring&& other) noexcept
{
  *static_cast<byte_buffer*>(this) = std::move(other);
  return *this;
}

template <bool Al>
SRSASN_CODE unbounded_octstring<Al>::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_length(bref, length(), aligned));
  for (uint8_t b : *this) {
    HANDLE_CODE(bref.pack(b, 8));
  }
  return SRSASN_SUCCESS;
}

template <bool Al>
SRSASN_CODE unbounded_octstring<Al>::unpack(cbit_ref& bref)
{
  unsigned len;
  HANDLE_CODE(unpack_length(len, bref, aligned));
  clear();
  for (unsigned i = 0; i != len; ++i) {
    uint8_t b;
    HANDLE_CODE(bref.unpack(b, 8));
    HANDLE_CODE(append(b) ? SRSASN_SUCCESS : SRSASN_ERROR_DECODE_FAIL);
  }
  return SRSASN_SUCCESS;
}

template <bool Al>
std::string unbounded_octstring<Al>::to_string() const
{
  return octet_string_helper::to_hex_string(*this);
}

template <bool Al>
unbounded_octstring<Al>& unbounded_octstring<Al>::from_string(const std::string& hexstr)
{
  // clears previous buffer.
  *this = byte_buffer{byte_buffer::fallback_allocation_tag{}};

  // appends hex string to buffer.
  octet_string_helper::append_hex_string(*this, hexstr);

  return *this;
}

template <bool Al>
unbounded_octstring<Al>& unbounded_octstring<Al>::from_bytes(span<const uint8_t> bytes)
{
  // clears previous buffer.
  *this = byte_buffer{byte_buffer::fallback_allocation_tag{}};

  // appends bytes to buffer.
  if (not this->append(bytes)) {
    log_error("Failed to append byte to buffer");
  }

  return *this;
}

template <bool Al>
uint64_t unbounded_octstring<Al>::to_number() const
{
  return octet_string_helper::to_uint(*this);
}

template <bool Al>
unbounded_octstring<Al>& unbounded_octstring<Al>::from_number(uint64_t val)
{
  octet_string_helper::to_octet_string(*this, val);
  return *this;
}

template class unbounded_octstring<true>;
template class unbounded_octstring<false>;

/*********************
     bitstring
*********************/

namespace bitstring_utils {

/**
 * Pack ASN1 bitstring length prefix. Accommodates for cases: fixed/unbounded/bounded, aligned/unaligned, with/out ext
 */
SRSASN_CODE pack_length_prefix(bit_ref& bref,
                               uint32_t len,
                               uint32_t lb         = 0,
                               uint32_t ub         = std::numeric_limits<uint32_t>::max(),
                               bool     has_ext    = false,
                               bool     is_aligned = false)
{
  if (has_ext and ub == std::numeric_limits<uint32_t>::max()) {
    log_error("has extension marker but it is an unbounded prefix size");
    return SRSASN_ERROR_ENCODE_FAIL;
  }
  bool within_bounds = len >= lb and len <= ub;
  if (not within_bounds and not has_ext) {
    log_error("bitstring length={} is not within bounds [{}, {}]", len, lb, ub);
    return SRSASN_ERROR_ENCODE_FAIL;
  }

  // encode ext bit
  if (has_ext) {
    HANDLE_CODE(bref.pack(not within_bounds, 1));
  }

  // do not encode prefix if fixed size
  if (lb == ub and within_bounds) {
    return SRSASN_SUCCESS;
  }

  // pack as unbounded if unbounded bitstring or ext is active
  if (ub == std::numeric_limits<uint32_t>::max() or not within_bounds) {
    return pack_length(bref, len);
  }

  // pack as bounded bitstring
  uint32_t len_bits = (uint32_t)ceilf(log2(ub - lb));
  HANDLE_CODE(bref.pack(len - lb, len_bits));
  if (is_aligned) {
    HANDLE_CODE(bref.align_bytes_zero());
  }
  return SRSASN_SUCCESS;
}

SRSASN_CODE pack_bitfield(bit_ref& bref, const uint8_t* buf, uint32_t nbits, uint32_t lb, uint32_t ub, bool is_aligned)
{
  if (nbits == 0) {
    log_error("Invalid bitstring size={}", nbits);
    return SRSASN_ERROR_ENCODE_FAIL;
  }
  if (is_aligned and (lb != ub or ub > 16)) {
    bref.align_bytes_zero();
  }
  uint32_t n_octs = ceil_frac(nbits, 8u);
  uint32_t offset = ((nbits - 1) % 8) + 1;
  HANDLE_CODE(bref.pack(buf[n_octs - 1], offset));
  for (uint32_t i = 1; i < n_octs; ++i) {
    HANDLE_CODE(bref.pack(buf[n_octs - 1 - i], 8));
  }
  return SRSASN_SUCCESS;
}

/**
 * Pack ASN1 bitstring. Accommodates for cases: fixed/unbounded/bounded, aligned/unaligned, with/out ext
 */
SRSASN_CODE
pack(bit_ref& bref, const uint8_t* data, uint32_t len, uint32_t lb, uint32_t ub, bool has_ext, bool is_aligned)
{
  HANDLE_CODE(bitstring_utils::pack_length_prefix(bref, len, lb, ub, has_ext, is_aligned));
  return pack_bitfield(bref, data, len, lb, ub, is_aligned);
}

// Unpack prefix, excluding ext bit

/**
 * Unpack ASN1 bitstring length prefix. Accommodates for cases: fixed/unbounded/bounded, aligned/unaligned, with/out ext
 */
SRSASN_CODE unpack_length_prefix(uint32_t& len, cbit_ref& bref, uint32_t lb, uint32_t ub, bool has_ext, bool is_aligned)
{
  bool ext = false;
  if (has_ext) {
    HANDLE_CODE(bref.unpack(ext, 1));
  }

  // unbounded or ext is set
  if (ub == std::numeric_limits<uint32_t>::max() or ext) {
    return unpack_length(len, bref, is_aligned);
  }

  // fixed length with no ext
  if (lb == ub) {
    len = lb;
    return SRSASN_SUCCESS;
  }

  // constrained, simple case
  HANDLE_CODE(unpack_length(len, bref, lb, ub, is_aligned));
  //  if (is_aligned) {
  //    HANDLE_CODE(bref.align_bytes_zero());
  //  }
  //  uint32_t len_bits = ceilf(log2f(ub - lb));
  //  HANDLE_CODE(bref.unpack(len, len_bits));
  //  len += lb;
  return SRSASN_SUCCESS;
}

// for both fixed, constrained and unconstrained scenarios
SRSASN_CODE unpack_bitfield(uint8_t* buf, cbit_ref& bref, uint32_t n, uint32_t lb, uint32_t ub, bool is_aligned)
{
  if (n > ASN_64K) {
    log_error("bitstrings longer than 64K not supported");
    return SRSASN_ERROR_DECODE_FAIL;
  }
  if (n == 0) {
    // empty bit string
    return SRSASN_SUCCESS;
  }
  if (is_aligned and (lb != ub or ub > 16)) {
    bref.align_bytes();
  }
  // unpack exactly "n" bits
  uint32_t n_octs = ceil_frac(n, 8u);
  uint32_t offset = ((n - 1) % 8) + 1;
  HANDLE_CODE(bref.unpack(buf[n_octs - 1], offset));
  for (uint32_t i = 1; i < n_octs; ++i) {
    HANDLE_CODE(bref.unpack(buf[n_octs - 1 - i], 8));
  }
  return SRSASN_SUCCESS;
}

void from_number(uint8_t* ptr, uint64_t number, uint32_t nbits)
{
  if (nbits > 64u) {
    log_error("bitstring of size={} does not fit in an uint64_t", nbits);
    return;
  }
  uint32_t nof_bytes = ceil_frac(nbits, 8u);
  for (uint32_t i = 0; i < nof_bytes; ++i) {
    ptr[i] = (number >> (i * 8u)) & 0xffu;
  }
  uint32_t offset = nbits % 8; // clean up any extra set bit
  if (offset > 0) {
    ptr[nof_bytes - 1] &= (uint8_t)((1u << offset) - 1u);
  }
}

std::string to_string(const uint8_t* ptr, uint32_t nbits)
{
  std::string str(nbits, '\0');
  for (uint32_t i = 0; i < nbits; ++i) {
    str[i] = bitstring_utils::get(ptr, nbits - 1 - i) ? '1' : '0';
  }
  return str;
}

uint64_t to_number(const uint8_t* ptr, uint32_t nbits)
{
  if (nbits > 64u) {
    log_error("bitstring of size={} does not fit in an uint64_t", nbits);
    return 0;
  }
  uint64_t val       = 0;
  uint32_t nof_bytes = ceil_frac(nbits, 8u);
  for (uint32_t i = 0; i < nof_bytes; ++i) {
    val += static_cast<uint64_t>(ptr[i]) << static_cast<uint64_t>(i * 8u);
  }
  return val;
}

} // namespace bitstring_utils

/*********************
     choice utils
*********************/
void log_invalid_access_choice_id(uint32_t val, uint32_t choice_id)
{
  log_error("The access choide id is invalid ({}!={})", (size_t)val, (size_t)choice_id);
}
void log_invalid_choice_id(uint32_t val, const char* choice_type)
{
  log_error("Invalid choice id={} for choice type {}", (size_t)val, choice_type);
}

/*********************
   printable string
*********************/

namespace asn_string_utils {

size_t get_nof_bits_per_char(size_t lb, size_t ub, bool aligned)
{
  size_t N = ub - lb + 1;
  auto   b = (size_t)ceilf(log2(N)); // B
  if (aligned) {
    b = (size_t)pow(2, ceilf(log2(b))); // B2
  }
  return b;
}

bool is_octet_aligned(size_t bits_per_char, size_t alb, size_t aub, bool aligned)
{
  size_t max_nof_bits = bits_per_char * aub;
  return aligned and (max_nof_bits > 16 or (alb != aub and max_nof_bits == 16));
}

constexpr bool is_length_encoded(size_t alb, size_t aub, bool aligned)
{
  return alb != aub or aub >= ASN_64K;
}

SRSASN_CODE
pack(bit_ref& bref, const std::string& s, size_t lb, size_t ub, size_t alb, size_t aub, bool ext, bool aligned)
{
  bool within_limits = s.size() >= alb and s.size() <= aub;
  if (not within_limits) {
    // TODO: print error
    // NOTE: This should be valid for exts
    log_error("The PrintableString size={} is not within the limits [{}, {}]", s.size(), alb, aub);
    return SRSASN_ERROR_ENCODE_FAIL;
  }
  size_t b              = asn_string_utils::get_nof_bits_per_char(lb, ub, aligned);
  bool   octet_aligned  = asn_string_utils::is_octet_aligned(b, alb, aub, aligned);
  bool   length_encoded = asn_string_utils::is_length_encoded(alb, aub, aligned);

  if (ext) {
    HANDLE_CODE(bref.pack(0, 1));
  }
  if (length_encoded) {
    pack_length(bref, s.size(), alb, aub, octet_aligned);
  }
  if (octet_aligned) {
    bref.align_bytes_zero();
  }
  for (char c : s) {
    HANDLE_CODE(bref.pack(c, b));
  }
  return SRSASN_SUCCESS;
}

SRSASN_CODE unpack(std::string& s, cbit_ref& bref, size_t lb, size_t ub, size_t alb, size_t aub, bool ext, bool aligned)
{
  size_t b              = asn_string_utils::get_nof_bits_per_char(lb, ub, aligned);
  bool   octet_aligned  = asn_string_utils::is_octet_aligned(b, alb, aub, aligned);
  bool   length_encoded = asn_string_utils::is_length_encoded(alb, aub, aligned);
  if (ext) {
    bool is_ext;
    HANDLE_CODE(bref.unpack(is_ext, 1));
    if (is_ext) {
      log_error("Extension of PrintableString not supported");
      return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (length_encoded) {
    size_t n;
    unpack_length(n, bref, alb, aub, octet_aligned);
    s.resize(n);
  }
  if (octet_aligned) {
    bref.align_bytes();
  }
  for (char& c : s) {
    HANDLE_CODE(bref.unpack(c, b));
  }
  return SRSASN_SUCCESS;
}

} // namespace asn_string_utils

/*********************
      ext group
*********************/

bool& ext_groups_packer_guard::operator[](uint32_t idx)
{
  if (idx >= groups.size()) {
    uint32_t prev_size = groups.size();
    groups.resize(idx + 1);
    std::fill(&groups[prev_size], &groups[groups.size()], false);
  }
  return groups[idx];
}

SRSASN_CODE ext_groups_packer_guard::pack(asn1::bit_ref& bref) const
{
  // pack number of groups
  int32_t i = groups.size() - 1;
  for (; i >= 0; --i) {
    if (groups[i]) {
      break;
    }
  }
  uint32_t nof_groups = (uint32_t)i + 1u;
  HANDLE_CODE(pack_norm_small_non_neg_whole_number(bref, nof_groups - 1));

  // pack each group presence flag
  for (uint32_t j = 0; j < nof_groups; ++j) {
    HANDLE_CODE(bref.pack(groups[j], 1));
  }
  return SRSASN_SUCCESS;
}

ext_groups_unpacker_guard::ext_groups_unpacker_guard(uint32_t nof_supported_groups_) :
  nof_supported_groups(nof_supported_groups_)
{
  resize(nof_supported_groups);
}

bool& ext_groups_unpacker_guard::operator[](uint32_t idx)
{
  if (idx >= groups.size()) {
    // only resizes for unknown extensions
    resize(idx + 1);
  }
  return groups[idx];
}

void ext_groups_unpacker_guard::resize(uint32_t new_size)
{
  // always grows
  uint32_t prev_size = groups.size();
  groups.resize(std::max(new_size, nof_supported_groups));
  std::fill(&groups[prev_size], &groups[groups.size()], false);
}

ext_groups_unpacker_guard::~ext_groups_unpacker_guard()
{
  // consume all the unknown extensions
  for (uint32_t i = nof_supported_groups; i < nof_unpacked_groups; ++i) {
    if (groups[i]) {
      varlength_field_unpack_guard scope(*bref_tracker);
    }
  }
}

SRSASN_CODE ext_groups_unpacker_guard::unpack(cbit_ref& bref)
{
  bref_tracker = &bref;
  // unpack nof of ext groups
  uint32_t nof_ext_groups = 0;
  HANDLE_CODE(unpack_norm_small_non_neg_whole_number(nof_ext_groups, bref));
  nof_unpacked_groups += nof_ext_groups + 1;
  resize(nof_unpacked_groups);

  // unpack each group presence flag
  for (uint32_t i = 0; i < nof_unpacked_groups; ++i) {
    HANDLE_CODE(bref.unpack(groups[i], 1));
  }
  return SRSASN_SUCCESS;
}

/*********************
     Open Field
*********************/

varlength_field_pack_guard::varlength_field_pack_guard(bit_ref& bref, bool align_) : brefstart(bref)
{
  bref         = bit_ref(varlen_buffer);
  bref_tracker = &bref;
  align        = align_;
}

varlength_field_pack_guard::~varlength_field_pack_guard()
{
  // fill the spare bits
  uint32_t leftover = 7U - ((bref_tracker->distance() - (uint32_t)1U) % (uint32_t)8U);
  bref_tracker->pack(0, leftover);

  // check how many bytes were written in total
  uint32_t nof_bytes = bref_tracker->distance() / (uint32_t)8U;

  // go back in time to pack length
  pack_length(brefstart, nof_bytes, align);

  // pack encoded bytes
  brefstart.pack_bytes(varlen_buffer);
  *bref_tracker = brefstart;
}

varlength_field_unpack_guard::varlength_field_unpack_guard(cbit_ref& bref, bool align) :
  len([&bref, align]() {
    uint32_t len_;
    unpack_length(len_, bref, align);
    return len_;
  }()),
  bref0(bref),
  bref_tracker(&bref)
{
  bref = bref.subview(0, len);
}

varlength_field_unpack_guard::~varlength_field_unpack_guard()
{
  if (len * 8 < (unsigned)bref_tracker->distance(bref0)) {
    log_error("The number of bits unpacked exceeds the variable length field size ({} > {})",
              bref_tracker->distance(bref0),
              len * 8);
    return;
  }

  // Ignore padding bits, and skip to the end of the varlength field.
  bref0.advance_bytes(len);
  *bref_tracker = bref0;
}

/*******************
    JsonWriter
*******************/

json_writer::json_writer() : ident(""), sep(NONE) {}

void json_writer::write_fieldname(const char* fieldname)
{
  static constexpr const char* septable[] = {",\n", "\n", ""};

  fmt::format_to(std::back_inserter(buffer), "{}{}", septable[sep], sep != NONE ? ident : "");
  if (strlen(fieldname) != 0) {
    fmt::format_to(std::back_inserter(buffer), "\"{}\": ", fieldname);
  }
  sep = NONE;
}

void json_writer::write_str(const char* fieldname, const char* value)
{
  write_fieldname(fieldname);
  fmt::format_to(std::back_inserter(buffer), "\"{}\"", value);
  sep = COMMA;
}

void json_writer::write_str(const char* fieldname, const std::string& value)
{
  write_str(fieldname, value.c_str());
}

void json_writer::write_str(const char* value)
{
  write_str("", value);
}

void json_writer::write_str(const std::string& value)
{
  write_str("", value);
}

void json_writer::write_int(const char* fieldname, int64_t value)
{
  write_fieldname(fieldname);
  fmt::format_to(std::back_inserter(buffer), "{}", value);
  sep = COMMA;
}
void json_writer::write_int(int64_t value)
{
  write_int("", value);
}

void json_writer::write_bool(const char* fieldname, bool value)
{
  write_fieldname(fieldname);
  fmt::format_to(std::back_inserter(buffer), "{}", value ? "true" : "false");
  sep = COMMA;
}
void json_writer::write_bool(bool value)
{
  write_bool("", value);
}

void json_writer::write_float(const char* fieldname, float value)
{
  write_fieldname(fieldname);
  fmt::format_to(std::back_inserter(buffer), "{}", value);
  sep = COMMA;
}

void json_writer::write_float(float value)
{
  write_float("", value);
}
void json_writer::write_null(const char* fieldname)
{
  write_fieldname(fieldname);
  fmt::format_to(std::back_inserter(buffer), "null");
  sep = COMMA;
}
void json_writer::write_null()
{
  write_null("");
}

void json_writer::start_obj(const char* fieldname)
{
  write_fieldname(fieldname);
  fmt::format_to(std::back_inserter(buffer), "{{");
  ident += "  ";
  sep = NEWLINE;
}
void json_writer::end_obj()
{
  ident.erase(ident.size() - 2, 2);
  fmt::format_to(std::back_inserter(buffer), "\n{}}}", ident);
  sep = COMMA;
}

void json_writer::start_array(const char* fieldname)
{
  write_fieldname(fieldname);
  fmt::format_to(std::back_inserter(buffer), "[");
  ident += "  ";
  sep = NEWLINE;
}
void json_writer::end_array()
{
  ident.erase(ident.size() - 2, 2);
  fmt::format_to(std::back_inserter(buffer), "\n{}]", ident);
  sep = COMMA;
}

std::string json_writer::to_string() const
{
  return std::string(buffer.data(), buffer.size());
}

/************************
   General Layer Types
************************/

uint32_t detail::base_empty_obj_set::idx_to_id(uint32_t idx)
{
  asn1::log_error("object set is empty\n");
  return 0;
}
bool detail::base_empty_obj_set::is_id_valid(const uint32_t& id)
{
  asn1::log_error("object set is empty\n");
  return false;
}
crit_e detail::base_empty_obj_set::get_crit(const uint32_t& id)
{
  return {};
}
presence_e detail::base_empty_obj_set::get_presence(const uint32_t& id)
{
  return {};
}

void detail::empty_obj_set_item_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.end_obj();
}
SRSASN_CODE detail::empty_obj_set_item_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  return SRSASN_SUCCESS;
}
SRSASN_CODE detail::empty_obj_set_item_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  return SRSASN_SUCCESS;
}

const char* detail::empty_obj_set_item_c::types_opts::to_string() const
{
  log_error("The enum value=0 of type protocol_ies_empty_o::value_c::types is not valid.");
  return "";
}

SRSASN_CODE pack_unconstrained_real(bit_ref& bref, float n, bool aligned)
{
  if (aligned) {
    HANDLE_CODE(bref.align_bytes_zero());
  }

  // Handle special cases
  if (std::isinf(n)) {
    pack_length(bref, 1, aligned);
    bref.pack(n > 0 ? 0x40 : 0x41, 8);
    return SRSASN_SUCCESS;
  } else if (std::isnan(n)) {
    pack_length(bref, 1, aligned);
    bref.pack(0x42, 8);
    return SRSASN_SUCCESS;
  } else if (n == 0.0f) {
    pack_length(bref, 0, aligned);
    return SRSASN_SUCCESS;
  }

  uint32_t* bits_ptr = (uint32_t*)&n;
  uint32_t  bits     = *bits_ptr;
  bool      sign     = (bits >> 31) & 0x1;
  int8_t    exponent = ((bits >> 23) & 0xff);
  uint32_t  mantissa = (bits & 0x7fffff) | 0x800000;

  uint8_t info_octet = 0x80; // Base-2
  if (sign) {
    info_octet |= 0x1 << 6;
  }

  uint32_t trailing_zeros = srsran::detail::bitset_builtin_helper<unsigned>::zero_lsb_count(mantissa);
  mantissa >>= trailing_zeros;
  // the inverse of the trailing zeros gives the number of bits to shift
  // the mantissa to the right to make it a whole number, this number must be added to the exponent
  uint8_t inv_trailing_zeros = 23 - trailing_zeros;
  exponent -= (127 + inv_trailing_zeros);
  uint8_t asn1_exponent = exponent & 0xff;

  uint8_t mantissa_bit_len = 23 + 1 - trailing_zeros;
  uint8_t mantissa_oct_len = (mantissa_bit_len + 7) / 8;

  uint32_t len = mantissa_oct_len + 2;
  pack_length(bref, len, aligned);
  bref.pack(info_octet, 8);
  bref.pack(asn1_exponent, 8);
  for (int8_t i = mantissa_oct_len - 1; i >= 0; --i) {
    uint8_t octet = (mantissa >> (i * 8)) & 0xff;
    bref.pack(octet, 8);
  }
  return SRSASN_SUCCESS;
}

SRSASN_CODE unpack_unconstrained_real(float& n, cbit_ref& bref, bool aligned)
{
  if (aligned) {
    HANDLE_CODE(bref.align_bytes());
  }
  uint32_t len;
  HANDLE_CODE(unpack_length(len, bref, aligned));

  // Handle special case 0
  if (len == 0) {
    n = 0;
    return SRSASN_SUCCESS;
  }

  if (len == 2) {
    return SRSASN_ERROR_DECODE_FAIL;
  }

  uint8_t buf[10];
  for (uint32_t i = 0; i < len; i++) {
    HANDLE_CODE(bref.unpack(buf[i], 8));
  }

  uint8_t info_octet = buf[0];

  // Handle special cases
  if (info_octet == 0x40) {
    n = std::numeric_limits<float>::infinity(); // Positive infinity
    return SRSASN_SUCCESS;
  } else if (info_octet == 0x41) {
    n = -std::numeric_limits<float>::infinity(); // Negative infinity
    return SRSASN_SUCCESS;
  } else if (info_octet == 0x42) {
    n = std::numeric_limits<float>::quiet_NaN(); // NaN
    return SRSASN_SUCCESS;
  }

  uint8_t exponent_length = (info_octet & 0x3) + 1;
  uint8_t mantissa_length = len - exponent_length - 1;
  if (exponent_length != 1) {
    // Currently we support decoding only float with 1 byte exponent.
    return SRSASN_ERROR_DECODE_FAIL;
  }
  if (mantissa_length > 3) {
    // Currently we support decoding only float with 3 byte mantissa.
    return SRSASN_ERROR_DECODE_FAIL;
  }

  bool     sign          = (info_octet >> 6) & 0x1;
  int8_t   asn1_exponent = static_cast<int8_t>(buf[1]);
  uint32_t mantissa      = 0;

  for (size_t i = 2; i < len; ++i) {
    mantissa = (mantissa << 8) | buf[i];
  }
  uint8_t leading_zeros = srsran::detail::bitset_builtin_helper<unsigned>::zero_msb_count(mantissa) - 8;
  mantissa <<= leading_zeros;

  // Build the IEEE 754 float value from the sign, exponent, and mantissa
  uint32_t ieee_exponent = asn1_exponent + 127;
  uint32_t ieee_mantissa = mantissa & 0x7fffff;

  if (ieee_mantissa) {
    ieee_exponent += (23 - leading_zeros);
  }

  uint32_t ieee_bits = (sign << 31) | (ieee_exponent << 23) | ieee_mantissa;
  float*   bits_ptr  = (float*)&ieee_bits;
  n                  = *bits_ptr;
  return SRSASN_SUCCESS;
}

} // namespace asn1
