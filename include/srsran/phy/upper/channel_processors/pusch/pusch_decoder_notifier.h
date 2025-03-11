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

namespace srsran {

struct pusch_decoder_result;

/// PUSCH decoder interface to notify the decoding of a transport block.
class pusch_decoder_notifier
{
public:
  /// Default destructor.
  virtual ~pusch_decoder_notifier() = default;

  /// \brief Notifies the completion of a PUSCH transport block decoding.
  /// \param[in] result PUSCH decoder result.
  virtual void on_sch_data(const pusch_decoder_result& result) = 0;
};

} // namespace srsran
