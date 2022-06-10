#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "phy")

add_executable(lower_phy_sample lower_phy_sample.cpp lower_phy_sample_factory.cpp upper_phy_sample.cpp)
target_link_libraries(lower_phy_sample
        srsgnb_radio
        srslog
        srsgnb_support
        resource_grid_pool
        srsgnb_lower_phy
        srsgnb_signal_processors
        srsgnb_channel_processors)

# Add lower PHY sample loopback test with ZeroMQ only if ZeroMQ is available.
if (ZEROMQ_FOUND)
    add_test(lower_phy_sample_zmq lower_phy_sample -P zmq_50MHz -D 10 -L 1 -v warning)
endif (ZEROMQ_FOUND)
