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

#include "srsran/ofh/compression/iq_compressor.h"
#include "srsran/ofh/compression/iq_decompressor.h"
#include <memory>

namespace srsran {
namespace ofh {

/// \brief Creates and returns an IQ data compressor.
///
/// \param[in] type       Compression type.
/// \param[in] logger     Logger object.
/// \param[in] iq_scaling Scaling factor applied to IQ data prior to quantization.
/// \param[in] impl_type  Request for a specific implementation type ("auto", "avx2", "avx512", "generic").
/// \return IQ data compressor.
std::unique_ptr<iq_compressor> create_iq_compressor(compression_type      type,
                                                    srslog::basic_logger& logger,
                                                    float                 iq_scaling = 1.0,
                                                    const std::string&    impl_type  = "auto");

/// Creates and returns a selector of IQ data compressors.
std::unique_ptr<iq_compressor>
create_iq_compressor_selector(std::array<std::unique_ptr<iq_compressor>, NOF_COMPRESSION_TYPES_SUPPORTED> compressors);

/// \brief Creates and returns an IQ data decompressor.
///
/// \param[in] type       Compression type.
/// \param[in] impl_type  Request for a specific implementation type ("auto", "avx2", "avx512", "generic").
/// \return IQ data decompressor.
std::unique_ptr<iq_decompressor>
create_iq_decompressor(compression_type type, srslog::basic_logger& logger, const std::string& impl_type = "auto");

/// Creates and returns a selector of IQ data decompressors.
std::unique_ptr<iq_decompressor> create_iq_decompressor_selector(
    std::array<std::unique_ptr<iq_decompressor>, NOF_COMPRESSION_TYPES_SUPPORTED> decompressors);

} // namespace ofh
} // namespace srsran
