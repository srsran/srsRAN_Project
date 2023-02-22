#
# Copyright 2021-2023 Software Radio Systems Limited
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

add_executable(radio_ssb radio_ssb.cpp lower_phy_example_factory.cpp upper_phy_ssb_example.cpp)
target_link_libraries(radio_ssb
        srsran_radio
        srslog
        srsran_support
        srsran_phy_support
        srsran_lower_phy
        srsran_signal_processors
        srsran_channel_processors)

# Add lower PHY sample loopback test with ZeroMQ only if ZeroMQ is available.
if (ZEROMQ_FOUND)
    add_test(radio_ssb_zmq_20MHz_n7 radio_ssb -P zmq_20MHz_n7 -D 10 -L 1 -v warning)
    add_test(radio_ssb_zmq_20MHz_n78 radio_ssb -P zmq_20MHz_n78 -D 10 -L 1 -v warning)
endif (ZEROMQ_FOUND)
