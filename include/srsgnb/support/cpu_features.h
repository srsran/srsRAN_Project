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
  /// CPU supports AVX512F instruction set.
  avx512f,
  /// CPU supports AVX512BW instruction set.
  avx512bw,
  /// CPU supports carry-less multiplication instruction PCLMUL.
  pclmul,
  /// CPU supports SSE 4.1.
  sse4_1
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
    case cpu_feature::avx512f:
      return __builtin_cpu_supports("avx512f");
    case cpu_feature::avx512bw:
      return __builtin_cpu_supports("avx512bw");
    case cpu_feature::pclmul:
      return __builtin_cpu_supports("pclmul");
    case cpu_feature::sse4_1:
      return __builtin_cpu_supports("sse4.1");
#endif // __x86_64__
    default:
      return false;
  }
}

} // namespace srsgnb
