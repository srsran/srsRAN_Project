#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

# Do not build by default.
set_directory_properties(PROPERTIES EXCLUDE_FROM_ALL TRUE)

set(SOURCES
        cmdline_command_dispatcher.cpp)

add_library(srsran_cmdline_app_service STATIC ${SOURCES})
target_include_directories(srsran_cmdline_app_service PRIVATE ${CMAKE_SOURCE_DIR})
