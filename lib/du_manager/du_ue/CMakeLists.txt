#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES cell_group_config.cpp)
add_library(du_manager_ue STATIC ${SOURCES})
target_link_libraries(du_manager_ue srsgnb_ran)
