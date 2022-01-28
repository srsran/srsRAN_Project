
add_library(srsgnb_du du.cc)
target_link_libraries(srsgnb_du srsgnb_support srsgnb_f1ap srsgnb_rlc srsgnb_mac srsgnb_du_manager)

add_executable(du-phy phy_main.cc)
target_link_libraries(du-phy)
