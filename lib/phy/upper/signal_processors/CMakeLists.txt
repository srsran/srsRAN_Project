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

set(SOURCES
        dmrs_helper.cpp
        prs/factories.cpp
        prs/prs_generator_impl.cpp
        ptrs/ptrs_pdsch_generator_impl.cpp
        ptrs/ptrs_pdsch_generator_factory.cpp
        pucch/dmrs_pucch_estimator_format2.cpp
        pucch/dmrs_pucch_estimator_formats3_4.cpp
        srs/srs_estimator_factory.cpp
        srs/srs_estimator_generic_impl.cpp
        srs/srs_validator_generic_impl.cpp
        dmrs_pbch_processor_impl.cpp
        dmrs_pdcch_processor_impl.cpp
        dmrs_pdsch_processor_impl.cpp
        dmrs_pusch_estimator_impl.cpp
        nzp_csi_rs_generator_impl.cpp
        port_channel_estimator_average_impl.cpp
        port_channel_estimator_helpers.cpp
        signal_processor_factories.cpp
        pss_processor_impl.cpp
        sss_processor_impl.cpp
)

add_library(srsran_signal_processors STATIC ${SOURCES})
target_link_libraries(srsran_signal_processors srsvec srsran_phy_support srsran_upper_phy_support srsran_ran srsran_dft)

add_to_exported_libs(srsran_signal_processors)
