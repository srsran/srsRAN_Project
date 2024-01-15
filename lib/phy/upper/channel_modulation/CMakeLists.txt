#
# Copyright 2021-2024 Software Radio Systems Limited
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

set(SOURCES
        channel_modulation_factories.cpp
        demodulation_mapper_impl.cpp
        demodulation_mapper_qam16.cpp
        demodulation_mapper_qam64.cpp
        demodulation_mapper_qam256.cpp
        demodulation_mapper_qpsk.cpp
        evm_calculator_generic_impl.cpp
        modulation_mapper_impl.cpp)

add_library(srsran_channel_modulation STATIC ${SOURCES})
target_link_libraries(srsran_channel_modulation srsran_support log_likelihood_ratio)

install(TARGETS srsran_channel_modulation
        EXPORT srsran_export)
