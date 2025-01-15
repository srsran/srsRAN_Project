#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(TRX_DRIVER_HEADER trx_driver.h)

if (NOT DEFINED TRX_DRIVER_DIR)
    message(SEND_ERROR "Missing ${TRX_DRIVER_HEADER} directory variable. Rerun cmake with -DTRX_DRIVER_DIR=path_to_directory")
endif (NOT DEFINED TRX_DRIVER_DIR)

find_file(TRX_DRIVER_HEADER_PATH
        NAMES ${TRX_DRIVER_HEADER}
        HINTS ${TRX_DRIVER_DIR}
        PATHS ${TRX_DRIVER_DIR}
        REQUIRED
        )

message(STATUS "Found ${TRX_DRIVER_HEADER} in TRX_DRIVER_DIR=${TRX_DRIVER_HEADER_PATH}")
