
set_directory_properties(PROPERTIES LABELS "phy")

add_executable(lower_phy_sample lower_phy_sample.cpp)
target_link_libraries(lower_phy_sample srsgnb_radio srslog srsgnb_support srsgnb_lower_phy_modulation
        resource_grid_pool srsgnb_lower_phy srsgnb_generic_funcs_dft)

add_test(lower_phy_sample_zmq lower_phy_sample -P zmq_50MHz -D 10 -L 1 -v warning)
