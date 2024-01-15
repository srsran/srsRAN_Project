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

set_directory_properties(PROPERTIES LABELS "phy")

add_executable(polar_chain_test polar_chain_test.cpp)
target_link_libraries(polar_chain_test srsran_channel_coding srsvec srslog)
add_test(polar_chain_test_bch polar_chain_test -n9 -k56 -e864 -i0 -s101 -o1)
add_test(polar_chain_test_dci polar_chain_test -n9 -k40 -e100 -i0 -s101 -o1)
add_test(polar_chain_test_uci polar_chain_test -n10 -k20 -e256 -i1 -s101 -o1)
add_test(polar_chain_test_uci2 polar_chain_test -n10 -k18 -e45 -i1 -s101 -o1)
add_test(polar_chain_test_uci3 polar_chain_test -n10 -k18 -e38 -i1 -s101 -o1)

add_executable(polar_interleaver_test polar_interleaver_test.cpp)
target_link_libraries(polar_interleaver_test srsran_channel_coding srsvec srslog)
add_test(polar_interleaver_test polar_interleaver_test)
