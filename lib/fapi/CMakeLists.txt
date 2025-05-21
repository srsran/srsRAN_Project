#
# Copyright 2021-2025 Software Radio Systems Limited
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

add_subdirectory(decorator_helpers)
add_subdirectory(message_buffering)
add_subdirectory(loggers)
add_subdirectory(validators)

add_library(srsran_fapi_decorators STATIC decorator_factory.cpp)
target_link_libraries(srsran_fapi_decorators PRIVATE srsran_fapi_message_buffering srsran_fapi_loggers)
target_include_directories(srsran_fapi_decorators PRIVATE ${CMAKE_SOURCE_DIR}/lib/fapi)

set(CELL_SOURCES
        fapi_config_message_interface_collection_impl.cpp
        config_message_gateway_impl.cpp
        config_message_validator.cpp)

add_library(srsran_fapi_config_gateway STATIC ${CELL_SOURCES})
target_link_libraries(srsran_fapi_config_gateway PRIVATE srslog)

add_library(srsran_fapi INTERFACE)
target_link_libraries(srsran_fapi INTERFACE srsran_fapi_decorators srsran_fapi_validators srsran_fapi_config_gateway)
