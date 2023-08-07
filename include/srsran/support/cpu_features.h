/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/support/format_utils.h"
#include "fmt/format.h"
#include <vector>

namespace srsran {

/// CPU feature list.
enum class cpu_feature {
#ifdef __x86_64__
  /// CPU supports AVX instruction set.
  avx,
  /// CPU supports AVX2 instruction set.
  avx2,
  /// CPU supports AVX512F instruction set.
  avx512f,
  /// CPU supports AVX512BW instruction set.
  avx512bw,
  /// CPU supports AVX512VL instruction set.
  avx512vl,
  /// CPU supports AVX512DQ instruction set.
  avx512dq,
  /// CPU supports AVX512CD instruction set.
  avx512cd,
  /// CPU supports carry-less multiplication instruction PCLMUL.
  pclmul,
  /// CPU supports SSE 4.1.
  sse4_1,
  /// CPU supports fused multiply-add (FMA). This refers to FMA3 supported by GCC via "-mfma" flag.
  fma,
#endif // __x86_64__
#ifdef __aarch64__
  /// \brief CPU supports NEON.
  ///
  /// NEON is supported if \c __ARM_NEON is defined in compilation time.
  neon,
#endif // __aarch64__
};

constexpr const char* to_string(cpu_feature feature)
{
  switch (feature) {
#ifdef __x86_64__
    case cpu_feature::avx:
      return "avx";
    case cpu_feature::avx2:
      return "avx2";
    case cpu_feature::avx512f:
      return "avx512f";
    case cpu_feature::avx512bw:
      return "avx512bw";
    case cpu_feature::avx512vl:
      return "avx512vl";
    case cpu_feature::avx512dq:
      return "avx512dq";
    case cpu_feature::avx512cd:
      return "avx512cd";
    case cpu_feature::pclmul:
      return "pclmul";
    case cpu_feature::sse4_1:
      return "sse4.1";
    case cpu_feature::fma:
      return "fma";
#endif // __x86_64__
#ifdef __aarch64__
    case cpu_feature::neon:
      return "neon";
#endif // __aarch64__
  }
  return "invalid_cpu_feature";
}

/// \brief Query the CPU whether a CPU feature is supported in runtime.
///
/// \param[in] feature Selects the given feature to query.
/// \return True if the feature is supported.
inline bool cpu_supports_feature(cpu_feature feature)
{
  switch (feature) {
#ifdef __x86_64__
    case cpu_feature::avx:
      return __builtin_cpu_supports("avx");
    case cpu_feature::avx2:
      return __builtin_cpu_supports("avx2");
    case cpu_feature::avx512f:
      return __builtin_cpu_supports("avx512f");
    case cpu_feature::avx512bw:
      return __builtin_cpu_supports("avx512bw");
    case cpu_feature::avx512vl:
      return __builtin_cpu_supports("avx512vl");
    case cpu_feature::avx512dq:
      return __builtin_cpu_supports("avx512dq");
    case cpu_feature::avx512cd:
      return __builtin_cpu_supports("avx512cd");
    case cpu_feature::pclmul:
      return __builtin_cpu_supports("pclmul");
    case cpu_feature::sse4_1:
      return __builtin_cpu_supports("sse4.1");
    case cpu_feature::fma:
      return __builtin_cpu_supports("fma");
#endif // __x86_64__
#ifdef __aarch64__
#ifdef __ARM_NEON
    case cpu_feature::neon:
      return true;
#endif // __ARM_NEON
#endif // __aarch64__
    default:
      return false;
  }
}

static const std::vector<cpu_feature> cpu_features_included = {
#ifdef __x86_64__
#ifdef __SSE4_1__
    cpu_feature::sse4_1,
#endif // __SSE4_1__
#ifdef __AVX2__
    cpu_feature::avx,
#endif // __AVX2__
#ifdef __AVX2__
    cpu_feature::avx2,
#endif // __AVX2__
#ifdef __AVX512F__
    cpu_feature::avx512f,
#endif // __AVX512F__
#ifdef __AVX512BW__
    cpu_feature::avx512bw,
#endif // __AVX512BW__
#ifdef __AVX512VL__
    cpu_feature::avx512vl,
#endif // __AVX512VL__
#ifdef __AVX512DQ__
    cpu_feature::avx512dq,
#endif // __AVX512DQ__
#ifdef __AVX512CD__
    cpu_feature::avx512cd,
#endif // __AVX512CD__
#ifdef __FMA__
    cpu_feature::fma,
#endif // __FMA__
#ifdef __PCLMUL__
    cpu_feature::pclmul,
#endif // __PCLMUL__
#endif // __x86_64__
#ifdef __aarch64__
#ifdef __ARM_NEON
    cpu_feature::neon,
#endif // __ARM_NEON
#endif // __aarch64__
};

inline std::string get_cpu_feature_info()
{
  fmt::memory_buffer buffer;
  for (cpu_feature feature : cpu_features_included) {
#ifdef __x86_64__
    format_to(
        buffer, "{}{}{}", buffer.size() == 0 ? "" : " ", feature, cpu_supports_feature(feature) ? "(ok)" : "(na)");
#endif // __x86_64__
#ifdef __aarch64__
    format_to(buffer, "{}{}", buffer.size() == 0 ? "" : " ", feature);
#endif // __aarch64__
  }
  return std::string{srsran::to_c_str(buffer)};
}

inline bool cpu_supports_included_features()
{
  for (cpu_feature feature : cpu_features_included) {
    if (!cpu_supports_feature(feature)) {
      return false;
    }
  }
  return true;
}

} // namespace srsran

namespace fmt {
template <>
struct formatter<srsran::cpu_feature> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::cpu_feature feature, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "{}", srsran::to_string(feature));
  }
};
} // namespace fmt
