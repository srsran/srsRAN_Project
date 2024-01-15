/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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