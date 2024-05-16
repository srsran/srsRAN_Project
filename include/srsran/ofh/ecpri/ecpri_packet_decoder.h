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

#include "srsran/adt/span.h"
#include "srsran/ofh/ecpri/ecpri_packet_properties.h"
#include <variant>

namespace srsran {
namespace ecpri {

/// eCPRI packet parameter.
struct packet_parameters {
  /// Common header.
  common_header header;
  /// eCPRI type parameters.
  std::variant<realtime_control_parameters, iq_data_parameters> type_params;
};

/// eCPRI packet decoder interface.
class packet_decoder
{
public:
  /// Default destructor.
  virtual ~packet_decoder() = default;

  /// \brief Decodes the given packet, filling the eCPRI params and returning a span to the eCPRI payload.
  ///
  /// If the decoder detects an unsupported value, it will return an empty span and the params value will be undefined.
  ///
  /// \param[in] packet eCPRI packet to decode.
  /// \param[out] params eCPRI decoded parameters.
  /// \return A span to the eCPRI payload on success, otherwise an empty span in case of error.
  virtual span<const uint8_t> decode(span<const uint8_t> packet, packet_parameters& params) = 0;
};

} // namespace ecpri
} // namespace srsran
