#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "phy")

add_executable(radio_ssb radio_ssb.cpp lower_phy_example_factory.cpp upper_phy_ssb_example.cpp)
target_link_libraries(radio_ssb
        srsgnb_radio
        srslog
        srsgnb_support
        resource_grid_pool
        srsgnb_lower_phy
        srsgnb_signal_processors
        srsgnb_channel_processors)

# Add lower PHY sample loopback test with ZeroMQ only if ZeroMQ is available.
if (ZEROMQ_FOUND)
    add_test(radio_ssb_zmq_20MHz_n7 radio_ssb -P zmq_20MHz_n7 -D 10 -L 1 -v warning)
    add_test(radio_ssb_zmq_20MHz_n78 radio_ssb -P zmq_20MHz_n78 -D 10 -L 1 -v warning)
endif (ZEROMQ_FOUND)
