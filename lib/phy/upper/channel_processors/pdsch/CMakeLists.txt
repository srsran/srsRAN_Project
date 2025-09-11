#
# Copyright 2021-2025 Software Radio Systems Limited
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

add_library(srsran_pdsch_processor STATIC
        factories.cpp
        pdsch_block_processor_impl.cpp
        pdsch_block_processor_hw_impl.cpp
        pdsch_encoder_hw_impl.cpp
        pdsch_encoder_impl.cpp
        pdsch_modulator_impl.cpp
        pdsch_processor_flexible_impl.cpp
        pdsch_processor_impl.cpp
        pdsch_processor_validator_impl.cpp)

target_link_libraries(srsran_pdsch_processor
        srsran_channel_modulation
        srsran_phy_support
        srsran_ran
        srsran_upper_phy_support)
