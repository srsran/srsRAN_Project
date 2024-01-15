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

#include "srsran/ofh/ecpri/ecpri_factories.h"
#include "ecpri_packet_builder_impl.h"
#include "ecpri_packet_decoder_impl.h"

using namespace srsran;
using namespace ecpri;

std::unique_ptr<packet_builder> srsran::ecpri::create_ecpri_packet_builder()
{
  return std::make_unique<packet_builder_impl>();
}

std::unique_ptr<packet_decoder>
srsran::ecpri::create_ecpri_packet_decoder_using_payload_size(srslog::basic_logger& logger)
{
  return std::make_unique<packet_decoder_use_header_payload_size>(logger);
}

std::unique_ptr<packet_decoder>
srsran::ecpri::create_ecpri_packet_decoder_ignoring_payload_size(srslog::basic_logger& logger)
{
  return std::make_unique<packet_decoder_ignore_header_payload_size>(logger);
}
