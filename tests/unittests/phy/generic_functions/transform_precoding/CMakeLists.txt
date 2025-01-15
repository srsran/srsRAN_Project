#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "phy")

if (USE_PHY_TESTVECTORS)
    add_executable(transform_precoder_vectortest transform_precoder_vectortest.cpp)

    target_link_libraries(
            transform_precoder_vectortest
            srsran_transform_precoding
            srsran_generic_funcs
            srslog
            srsvec
            gtest
            gtest_main)

    add_test_vector(transform_precoder_vectortest transform_precoder_test_data.tar.gz "")
endif (USE_PHY_TESTVECTORS)
