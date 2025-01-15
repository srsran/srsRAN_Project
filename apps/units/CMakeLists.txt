#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

# Do not build by default.
set_directory_properties(PROPERTIES EXCLUDE_FROM_ALL TRUE)

add_subdirectory(flexible_o_du)
add_subdirectory(o_cu_cp)
add_subdirectory(o_cu_up)
