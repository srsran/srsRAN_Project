#
# Copyright 2021-2024 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
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
