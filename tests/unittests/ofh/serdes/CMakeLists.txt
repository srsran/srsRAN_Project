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

set_directory_properties(PROPERTIES LABELS "serdes")

add_executable(ofh_cplane_packet_builder_dynamic_compression_impl_test ofh_cplane_packet_builder_dynamic_compression_impl_test.cpp)
target_link_libraries(ofh_cplane_packet_builder_dynamic_compression_impl_test srsran_ofh_message_serdes srsran_support gtest gtest_main)
gtest_discover_tests(ofh_cplane_packet_builder_dynamic_compression_impl_test)

add_executable(ofh_cplane_packet_builder_impl_test ofh_cplane_packet_builder_impl_test.cpp)
target_link_libraries(ofh_cplane_packet_builder_impl_test srsran_ofh_message_serdes srsran_support gtest gtest_main)
gtest_discover_tests(ofh_cplane_packet_builder_impl_test)

add_executable(ofh_cplane_packet_builder_static_compression_impl_test ofh_cplane_packet_builder_static_compression_impl_test.cpp)
target_link_libraries(ofh_cplane_packet_builder_static_compression_impl_test srsran_ofh_message_serdes srsran_support gtest gtest_main)
gtest_discover_tests(ofh_cplane_packet_builder_static_compression_impl_test)

add_executable(ofh_uplane_packet_builder_dynamic_impl_test ofh_uplane_packet_builder_dynamic_impl_test.cpp)
target_link_libraries(ofh_uplane_packet_builder_dynamic_impl_test srsran_ru_ofh srsran_support srsran_phy_support srsran_channel_precoder srslog gtest gtest_main)
gtest_discover_tests(ofh_uplane_packet_builder_dynamic_impl_test)

add_executable(ofh_uplane_packet_builder_static_impl_test ofh_uplane_packet_builder_static_impl_test.cpp)
target_link_libraries(ofh_uplane_packet_builder_static_impl_test srsran_ru_ofh srsran_support srsran_phy_support srsran_channel_precoder srslog gtest gtest_main)
gtest_discover_tests(ofh_uplane_packet_builder_static_impl_test)

add_executable(ofh_uplane_packet_decoder_dynamic_impl ofh_uplane_packet_decoder_dynamic_impl_test.cpp)
target_link_libraries(ofh_uplane_packet_decoder_dynamic_impl srsran_ofh_message_serdes srsran_support gtest gtest_main)
gtest_discover_tests(ofh_uplane_packet_decoder_dynamic_impl)

add_executable(ofh_uplane_packet_decoder_static_impl ofh_uplane_packet_decoder_static_impl_test.cpp)
target_link_libraries(ofh_uplane_packet_decoder_static_impl srsran_ofh_message_serdes srsran_support gtest gtest_main)
gtest_discover_tests(ofh_uplane_packet_decoder_static_impl)
