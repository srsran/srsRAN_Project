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

set_directory_properties(PROPERTIES LABELS "ecpri")

add_executable(ecpri_packet_builder_impl_test ecpri_packet_builder_test.cpp)
target_link_libraries(ecpri_packet_builder_impl_test srsran_ofh_ecpri srsran_support srslog gtest gtest_main)
gtest_discover_tests(ecpri_packet_builder_impl_test)

add_executable(ecpri_packet_decoder_impl_test ecpri_packet_decoder_impl_test.cpp)
target_link_libraries(ecpri_packet_decoder_impl_test srsran_ofh_ecpri srsran_support srslog gtest gtest_main)
gtest_discover_tests(ecpri_packet_decoder_impl_test)
