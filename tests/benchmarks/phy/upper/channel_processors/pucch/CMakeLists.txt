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

add_executable(pucch_processor_benchmark pucch_processor_benchmark.cpp)
set(PUCCH_PROCESSOR_LIBRARIES
        srsran_pucch_processor
        srsran_channel_equalizer
        srsran_channel_processors
        srsran_transform_precoding
        srslog
        srsvec)

target_link_libraries(pucch_processor_benchmark ${PUCCH_PROCESSOR_LIBRARIES})
add_test(pucch_processor_benchmark pucch_processor_benchmark -m silent -R 1 -B 1 -T 2)
