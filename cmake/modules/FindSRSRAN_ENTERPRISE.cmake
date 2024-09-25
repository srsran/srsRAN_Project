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
if(NOT DEFINED SRSRAN_ENTERPRISE_PATH)
	message(FATAL_ERROR "SRSRAN_ENTERPRISE_PATH not provided. Run with \'-DSRSRAN_ENTERPRISE_PATH=<path_to_repo>\'.")
endif()
message(STATUS "srsRAN Enterprise provided paths: ${SRSRAN_ENTERPRISE_PATH}.")


set(SRSRAN_ENTERPRISE_PATH_BUILD ${SRSRAN_ENTERPRISE_PATH})
list(TRANSFORM SRSRAN_ENTERPRISE_PATH_BUILD APPEND "/build")

find_path(SRSRAN_ENTERPRISE_BINARY_DIR srsran_enterprise.cmake
    HINTS ${SRSRAN_ENTERPRISE_PATH_BUILD}
    NO_DEFAULT_PATH
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SRSRAN_ENTERPRISE
	FOUND_VAR SRSRAN_ENTERPRISE_FOUND
    REQUIRED_VARS
        SRSRAN_ENTERPRISE_BINARY_DIR
)
