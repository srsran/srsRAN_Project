#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(du_resource_manager du_ran_resource_manager_impl.cpp)
target_link_libraries(du_resource_manager srsgnb_du_config_validators)
