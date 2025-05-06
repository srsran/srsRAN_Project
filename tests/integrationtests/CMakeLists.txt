#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

# Suppress "<ciso646> is deprecated in C++17" warnings caused by gtest when compiling with gcc 15.1.
ADD_CXX_COMPILER_FLAG_IF_AVAILABLE(-Wno-deprecated HAVE_NO_DEPRECATED)

add_subdirectory(du_high)
add_subdirectory(du_high_cu)
add_subdirectory(e2ap)
add_subdirectory(ngap)
add_subdirectory(ofh)
add_subdirectory(phy)
add_subdirectory(rlc)
