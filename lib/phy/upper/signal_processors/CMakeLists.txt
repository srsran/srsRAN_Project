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
        dmrs_pbch_processor_impl.cpp
        dmrs_pdcch_processor_impl.cpp
        dmrs_pdsch_processor_impl.cpp
        dmrs_pusch_estimator_impl.cpp
        nzp_csi_rs_generator_impl.cpp
        port_channel_estimator_average_impl.cpp
        pucch/dmrs_pucch_processor_format1_impl.cpp
        pucch/dmrs_pucch_processor_format2_impl.cpp
        signal_processor_factories.cpp
        pss_processor_impl.cpp
        sss_processor_impl.cpp
        )

add_library(srsran_signal_processors STATIC ${SOURCES})
target_link_libraries(srsran_signal_processors srsvec srsran_phy_support srsran_upper_phy_support srsran_ran srsran_generic_funcs )

install(TARGETS srsran_signal_processors
        EXPORT srsran_export)