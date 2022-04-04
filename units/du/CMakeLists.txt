
add_library(srsgnb_du ../../lib/du_high/du_high.cpp)
target_link_libraries(srsgnb_du srsgnb_support srsgnb_rlc srsgnb_f1ap srsgnb_mac srsgnb_du_manager)

add_executable(du-phy phy_main.cpp)
target_link_libraries(du-phy)
