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

add_executable(dmrs_pdsch_processor_benchmark dmrs_pdsch_processor_benchmark.cpp)
target_link_libraries(dmrs_pdsch_processor_benchmark
        srsvec
        srslog
        srsran_dmrs_pdsch
        srsran_phy_support
        srsran_sequence_generators
        srsran_channel_precoder)
add_test(dmrs_pdsch_processor_benchmark dmrs_pdsch_processor_benchmark -D -s -R 1)


add_executable(srs_estimator_benchmark srs_estimator_benchmark.cpp)
target_link_libraries(srs_estimator_benchmark
        srsvec
        srslog
        srsran_channel_precoder
        srsran_srs_estimator
        srsran_phy_support
        srsran_dft
        srsran_sequence_generators)
add_test(srs_estimator_benchmark srs_estimator_benchmark -D -s -R 1)
