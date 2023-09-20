#
# Copyright 2021-2023 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

add_library(srsran_pusch_decoder STATIC pusch_decoder_impl.cpp)
target_link_libraries(srsran_pusch_decoder srsran_channel_coding srsvec)

add_library(srsran_pusch_demodulator STATIC pusch_demodulator_impl.cpp)

add_library(srsran_pusch_processor STATIC pusch_processor_impl.cpp)
target_link_libraries(srsran_pusch_processor srsran_upper_phy_support srsran_ran)

add_library(srsran_ulsch_demux STATIC ulsch_demultiplex_impl.cpp)
