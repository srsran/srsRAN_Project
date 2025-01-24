/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include <cstdint>

namespace srsran {

// LMF measurement ID.
enum class lmf_meas_id_t : uint32_t { min = 1, max = 65536 };

/// Convert lmf_meas_id type to integer.
inline uint32_t lmf_meas_id_to_uint(lmf_meas_id_t id)
{
  return static_cast<uint32_t>(id);
}

/// Convert integer to lmf_meas_id type.
inline lmf_meas_id_t uint_to_lmf_meas_id(uint32_t id)
{
  return static_cast<lmf_meas_id_t>(id);
}

// RAN measurement ID.
enum class ran_meas_id_t : uint32_t { min = 1, max = 65536 };

/// Convert ran_meas_id type to integer.
inline uint32_t ran_meas_id_to_uint(ran_meas_id_t id)
{
  return static_cast<uint32_t>(id);
}

/// Convert integer to ran_meas_id type.
inline ran_meas_id_t uint_to_ran_meas_id(uint32_t id)
{
  return static_cast<ran_meas_id_t>(id);
}

// Transmission-Reception Point (TRP) ID.
enum class trp_id_t : uint32_t { min = 1, max = 65535 };

/// Convert trp_id type to integer.
inline uint32_t trp_id_to_uint(trp_id_t id)
{
  return static_cast<uint32_t>(id);
}

/// Convert integer to trp_id type.
inline trp_id_t uint_to_trp_id(uint32_t id)
{
  return static_cast<trp_id_t>(id);
}

} // namespace srsran
