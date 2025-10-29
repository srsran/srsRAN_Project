#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(p7)


add_library(srsran_phy_fapi_fastpath_adaptor STATIC phy_fapi_fastpath_adaptor_factory_impl.cpp
                                                    phy_fapi_fastpath_adaptor_impl.cpp
                                                    phy_fapi_sector_fastpath_adaptor_impl.cpp)

target_link_libraries(srsran_phy_fapi_fastpath_adaptor PRIVATE srsran_phy_to_fapi_p7_sector_fastpath_adaptor)
