/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ofh/compression/iq_compressor.h"
#include "srsran/ofh/compression/iq_decompressor.h"
#include "srsran/srslog/logger.h"

namespace srsran {
namespace ofh {

/// Implementation of the IQ data compression by quantizing floating point numbers without compression.
class iq_compression_none_impl : public iq_compressor, public iq_decompressor
{
public:
  // Constructor.
  explicit iq_compression_none_impl(srslog::basic_logger& logger_, float iq_scaling_ = 1.0) :
    logger(logger_), iq_scaling(iq_scaling_)
  {
  }

  // See interface for the documentation.
  virtual void
  compress(span<compressed_prb> output, span<const cf_t> input, const ru_compression_params& params) override;

  // See interface for the documentation.
  virtual void
  decompress(span<cf_t> output, span<const compressed_prb> input, const ru_compression_params& params) override;

protected:
  /// \brief Prints to the log the root mean square (RMS) value of the given samples.
  ///
  /// \param[in] samples - Quantized samples.
  void log_post_quantization_rms(span<const int16_t> samples);

  srslog::basic_logger& logger;
  /// Scaling factor applied to IQ data prior to quantization.
  const float iq_scaling;
};

} // namespace ofh
} // namespace srsran
