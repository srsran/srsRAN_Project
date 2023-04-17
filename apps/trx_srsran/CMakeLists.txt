#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Find TRX header
find_package(TRX_DRIVER MODULE REQUIRED)

if (NOT (ZEROMQ_FOUND AND ENABLE_ZEROMQ))
    message(SEND_ERROR "ZeroMQ is required to build TRX driver.")
endif ()


# Add shared library.
add_library(trx_srsran SHARED trx_srsran.cpp)
target_link_libraries(trx_srsran PRIVATE
        srsran_radio
        srsran_support
        srslog)
target_include_directories(trx_srsran PRIVATE
        ${SRSRAN_INCLUDE_DIR}
        ${TRX_DRIVER_DIR}
        "${SRSRAN_SOURCE_DIR}/external/fmt/include/"
        "${SRSRAN_SOURCE_DIR}/external/")

# Add dynamic library test.
add_executable(trx_srsran_test trx_srsran_test.cpp)
target_link_libraries(trx_srsran_test dl)
target_include_directories(trx_srsran_test PRIVATE ${TRX_DRIVER_DIR})
add_test(trx_srsran_test trx_srsran_test ./libtrx_srsran.so)

# Make sure libtrx_srsran is compiled when trx_srsran_test is the only target.
add_dependencies(trx_srsran_test trx_srsran)
