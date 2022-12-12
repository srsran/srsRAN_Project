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

namespace srsgnb {

/// CPU feature list.
enum class cpu_feature {
#ifdef __x86_64__
  /// CPU supports AVX2 instruction set.
  avx2,
  /// CPU supports carry-less multiplication instruction PCLMUL.
  pclmul
#endif // __x86_64__
#ifdef __arm__
// Write here ARM features...
#endif // __arm__
};

/// \brief Query the CPU whether a CPU feature is supported in runtime.
///
/// \param[in] feature Selects the given feature to query.
/// \return True if the feature is supported.
inline bool cpu_supports_feature(cpu_feature feature)
{
  switch (feature) {
#ifdef __x86_64__
    case cpu_feature::avx2:
      return __builtin_cpu_supports("avx2");
    case cpu_feature::pclmul:
    default:
      return __builtin_cpu_supports("pclmul");
#endif // __x86_64__
#ifdef __arm__
// Handle here ARM features...
#endif // __arm__
  }
}

} // namespace srsgnb
