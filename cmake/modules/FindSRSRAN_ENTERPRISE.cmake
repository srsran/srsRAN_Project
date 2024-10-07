#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

#[=======================================================================[.rst:
FindSRSRANENTERPRISE
-------

Finds the SRSRAN Enterprise exported libraries.

Imported Targets
^^^^^^^^^^^^^^^^

This module provides access to the targets exported by srsRAN Enterprise. They will be
available under the namespace ``srsran_enterprise::``.


Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``SRSRAN_ENTERPRISE_FOUND``
  True if the SRSRAN libraries were found.
``SRSRAN_ENTERPRISE_BINARY_DIR``
  Full path to the top level of the SRSRAN build tree.

Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``SRSRAN_ENTERPRISE_BINARY_DIR``
  Full path to the top level of the SRSRAN build tree.

#]=======================================================================]
if(NOT DEFINED SRSRAN_ENTERPRISE_BINARY_DIR)
    message(FATAL_ERROR "SRSRAN_ENTERPRISE_BUILD_PATH not provided. Run with \'-DSRSRAN_ENTERPRISE_BINARY_DIR=<path_to_repo_build_folder>\'.")
endif()
message(STATUS "srsRAN Enterprise binaries provided path: ${SRSRAN_ENTERPRISE_BINARY_DIR}.")

find_path(SRSRAN_ENTERPRISE_BINARY_DIR srsran_enterprise.cmake
    HINTS ${SRSRAN_ENTERPRISE_BINARY_DIR}
    NO_DEFAULT_PATH
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SRSRAN_ENTERPRISE
    FOUND_VAR SRSRAN_ENTERPRISE_FOUND
    REQUIRED_VARS
    SRSRAN_ENTERPRISE_BINARY_DIR
)
