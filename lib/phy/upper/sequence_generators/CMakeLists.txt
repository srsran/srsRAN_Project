#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        low_papr_sequence_generator_impl.cpp
        pseudo_random_generator_impl.cpp
        )

include_directories(../../../../include/srsgnb/phy/upper/sequence_generators)

add_library(sequence_generators STATIC ${SOURCES})
