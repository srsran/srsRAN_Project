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

#include "srsran/ofh/compression/compression_factory.h"
#include "iq_compression_bfp_impl.h"
#include "iq_compression_death_impl.h"
#include "iq_compression_none_impl.h"
#include "iq_compressor_selector.h"
#include "iq_decompressor_selector.h"
#include "srsran/support/cpu_features.h"
#include "srsran/support/error_handling.h"

#ifdef __x86_64__
#include "iq_compression_bfp_avx2.h"
#include "iq_compression_bfp_avx512.h"
#include "iq_compression_none_avx512.h"
#endif

#ifdef HAVE_NEON
#include "iq_compression_bfp_neon.h"
#endif // HAVE_NEON

using namespace srsran;
using namespace ofh;

std::unique_ptr<iq_compressor> srsran::ofh::create_iq_compressor(compression_type      type,
                                                                 srslog::basic_logger& logger,
                                                                 float                 iq_scaling,
                                                                 const std::string&    impl_type)
{
  switch (type) {
    case compression_type::none:
#ifdef __x86_64__
    {
      bool supports_avx512 = cpu_supports_feature(cpu_feature::avx512f) &&
                             cpu_supports_feature(cpu_feature::avx512vl) && cpu_supports_feature(cpu_feature::avx512bw);
      if (((impl_type == "avx512") || (impl_type == "auto")) && supports_avx512) {
        return std::make_unique<iq_compression_none_avx512>(logger, iq_scaling);
      }
    }
#endif
      return std::make_unique<iq_compression_none_impl>(logger, iq_scaling);
    case compression_type::BFP:
#ifdef __x86_64__
    {
      bool supports_avx2   = cpu_supports_feature(cpu_feature::avx2);
      bool supports_avx512 = cpu_supports_feature(cpu_feature::avx512f) &&
                             cpu_supports_feature(cpu_feature::avx512vl) &&
                             cpu_supports_feature(cpu_feature::avx512bw) &&
                             cpu_supports_feature(cpu_feature::avx512dq) && cpu_supports_feature(cpu_feature::avx512cd);
      if (((impl_type == "avx512") || (impl_type == "auto")) && supports_avx512) {
        return std::make_unique<iq_compression_bfp_avx512>(logger, iq_scaling);
      }
      if (((impl_type == "avx2") || (impl_type == "auto")) && supports_avx2) {
        return std::make_unique<iq_compression_bfp_avx2>(logger, iq_scaling);
      }
    }
#endif
#ifdef HAVE_NEON
      if ((impl_type == "neon") || (impl_type == "auto")) {
        return std::make_unique<iq_compression_bfp_neon>(logger, iq_scaling);
      }
#endif // HAVE_NEON
      return std::make_unique<iq_compression_bfp_impl>(logger, iq_scaling);
    case compression_type::block_scaling:
      return std::make_unique<iq_compression_death_impl>();
    case compression_type::mu_law:
      return std::make_unique<iq_compression_death_impl>();
    case compression_type::modulation:
      return std::make_unique<iq_compression_death_impl>();
    case compression_type::bfp_selective:
      return std::make_unique<iq_compression_death_impl>();
    case compression_type::mod_selective:
      return std::make_unique<iq_compression_death_impl>();
    default:
      report_fatal_error("Compression of {} type is not implemented", to_string(type));
  }
}

std::unique_ptr<iq_decompressor>
srsran::ofh::create_iq_decompressor(compression_type type, srslog::basic_logger& logger, const std::string& impl_type)
{
  switch (type) {
    case compression_type::none:
#ifdef __x86_64__
    {
      bool supports_avx512 = cpu_supports_feature(cpu_feature::avx512f) &&
                             cpu_supports_feature(cpu_feature::avx512vl) && cpu_supports_feature(cpu_feature::avx512bw);
      if (((impl_type == "avx512") || (impl_type == "auto")) && supports_avx512) {
        return std::make_unique<iq_compression_none_avx512>(logger);
      }
    }
#endif
      return std::make_unique<iq_compression_none_impl>(logger);
    case compression_type::BFP:
#ifdef __x86_64__
    {
      bool supports_avx2   = cpu_supports_feature(cpu_feature::avx2);
      bool supports_avx512 = cpu_supports_feature(cpu_feature::avx512f) &&
                             cpu_supports_feature(cpu_feature::avx512vl) && cpu_supports_feature(cpu_feature::avx512bw);
      if (((impl_type == "avx512") || (impl_type == "auto")) && supports_avx512) {
        return std::make_unique<iq_compression_bfp_avx512>(logger);
      }
      if (((impl_type == "avx2") || (impl_type == "auto")) && supports_avx2) {
        return std::make_unique<iq_compression_bfp_avx2>(logger);
      }
    }
#endif
#ifdef HAVE_NEON
      if ((impl_type == "neon") || (impl_type == "auto")) {
        return std::make_unique<iq_compression_bfp_neon>(logger);
      }
#endif // HAVE_NEON
      return std::make_unique<iq_compression_bfp_impl>(logger);
    case compression_type::block_scaling:
      return std::make_unique<iq_compression_death_impl>();
    case compression_type::mu_law:
      return std::make_unique<iq_compression_death_impl>();
    case compression_type::modulation:
      return std::make_unique<iq_compression_death_impl>();
    case compression_type::bfp_selective:
      return std::make_unique<iq_compression_death_impl>();
    case compression_type::mod_selective:
      return std::make_unique<iq_compression_death_impl>();
    default:
      report_fatal_error("Compression of {} type is not implemented", to_string(type));
  }
}

std::unique_ptr<iq_decompressor> srsran::ofh::create_iq_decompressor_selector(
    std::array<std::unique_ptr<iq_decompressor>, NOF_COMPRESSION_TYPES_SUPPORTED> decompressors)
{
  return std::make_unique<iq_decompressor_selector>(std::move(decompressors));
}

std::unique_ptr<iq_compressor> srsran::ofh::create_iq_compressor_selector(
    std::array<std::unique_ptr<iq_compressor>, NOF_COMPRESSION_TYPES_SUPPORTED> compressors)
{
  return std::make_unique<iq_compressor_selector>(std::move(compressors));
}
