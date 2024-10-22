#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(srsran_pucch_processor STATIC factories.cpp pucch_processor_impl.cpp)

add_library(srsran_pucch_detector STATIC
        factories.cpp
        pucch_detector_format0.cpp
        pucch_detector_format1.cpp)

add_library(srsran_pucch_demodulator STATIC factories.cpp pucch_demodulator_impl.cpp)
