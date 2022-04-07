add_subdirectory(modulation)

add_library(srsgnb_lower_phy STATIC lower_phy_factory.cpp lower_phy_impl.cpp)