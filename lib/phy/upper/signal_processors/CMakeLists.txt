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

add_library(srsran_channel_estimator STATIC
    channel_estimator/factories.cpp
    channel_estimator/port_channel_estimator_average_impl.cpp
    channel_estimator/port_channel_estimator_helpers.cpp
)
target_link_libraries(srsran_channel_estimator srsvec srsran_phy_support)

add_library(srsran_nzp_csi_rs_generator STATIC
    nzp_csi_rs/factories.cpp
    nzp_csi_rs/nzp_csi_rs_generator_impl.cpp
)
target_link_libraries(srsran_nzp_csi_rs_generator srsran_ran)

add_library(srsran_dmrs_pdcch STATIC
    pdcch/factories.cpp
    pdcch/dmrs_pdcch_processor_impl.cpp
    dmrs_helper.cpp
)

add_library(srsran_dmrs_pdsch STATIC
    pdsch/factories.cpp
    pdsch/dmrs_pdsch_processor_impl.cpp
    dmrs_helper.cpp
)

add_library(srsran_dmrs_pucch STATIC
    pucch/factories.cpp
    pucch/dmrs_pucch_estimator_format2.cpp
    pucch/dmrs_pucch_estimator_formats3_4.cpp
)

add_library(srsran_dmrs_pusch STATIC
    pusch/factories.cpp
    pusch/dmrs_pusch_estimator_impl.cpp
    dmrs_helper.cpp
)

add_library(srsran_prs_generator STATIC
    prs/factories.cpp
    prs/prs_generator_impl.cpp
)

add_library(srsran_ptrs_generator STATIC
    ptrs/ptrs_pdsch_generator_factory.cpp
    ptrs/ptrs_pdsch_generator_impl.cpp
)

add_library(srsran_srs_estimator STATIC
    srs/srs_estimator_factory.cpp
    srs/srs_estimator_generic_impl.cpp
    srs/srs_validator_generic_impl.cpp
)
target_link_libraries(srsran_srs_estimator srsran_ran)

add_library(srsran_ssb_generators STATIC
    ssb/factories.cpp
    ssb/dmrs_pbch_processor_impl.cpp
    ssb/pss_processor_impl.cpp
    ssb/sss_processor_impl.cpp
    dmrs_helper.cpp
)
target_link_libraries(srsran_ssb_generators srsran_dft)

add_library(srsran_signal_processors INTERFACE)
target_link_libraries(srsran_signal_processors INTERFACE
    srsran_channel_estimator
    srsran_nzp_csi_rs_generator
    srsran_dmrs_pdcch
    srsran_dmrs_pdsch
    srsran_dmrs_pucch
    srsran_dmrs_pusch
    srsran_prs_generator
    srsran_ptrs_generator
    srsran_srs_estimator
    srsran_ssb_generators
)

add_to_exported_libs(
    srsran_signal_processors
    srsran_channel_estimator
    srsran_nzp_csi_rs_generator
    srsran_dmrs_pdcch
    srsran_dmrs_pdsch
    srsran_dmrs_pucch
    srsran_dmrs_pusch
    srsran_prs_generator
    srsran_ptrs_generator
    srsran_srs_estimator
    srsran_ssb_generators
)
