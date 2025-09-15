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

# This function reads the COPYRIGHT file, parse the external info and call sbom
function(parse_copyright_file FILE)
  if(NOT EXISTS "${FILE}")
    message(FATAL_ERROR "COPYRIGHT file not found: ${FILE}")
  endif()

  file(READ "${FILE}" _raw)
  string(REPLACE "\r\n" "\n" _lines "${_raw}") # Normalize line endings
  string(REPLACE "\n\n\n" ";" _lines "${_lines}") # Split by sections when multiple new lines are present
  string(REPLACE "\n" " " _lines "${_lines}") # Remove single new lines inside sections

  foreach(_line IN LISTS _lines)
    if(_line MATCHES "^Files:")
      string(REGEX MATCH "Files: +(.*) Copyright: +(.*) License: +(.*)" _ ${_line})
      set(_files_str "${CMAKE_MATCH_1}")
      set(_copy_str "${CMAKE_MATCH_2}")
      set(_license_str "${CMAKE_MATCH_3}")

      # Module name
      string(STRIP "${_files_str}" _files_str)
      string(REPLACE " " ";" _file_list "${_files_str}")
      get_module_name_from_files("${_file_list}" _module_name)

      if("${_module_name}" STREQUAL "*" OR ${_module_name} STREQUAL "docs")
        continue()
      endif()

      # Supplier
      string(REGEX REPLACE
        "^[0-9][0-9][0-9][0-9](-present|-[0-9][0-9][0-9][0-9])?[, ]*"
        ""
        _copy_clean
        "${_copy_str}"
      )

      # Version
      string(TOUPPER "${_module_name}" _upper)
      set(_var "SBOM_${_upper}_VERSION")
      if(NOT DEFINED ${_var})
        message(FATAL_ERROR "Required variable `${_var}` is not defined. ")
      endif()
      set(_version "${${_var}}")

      # Add to SBOM
      sbom_add(
        PACKAGE "${_module_name}"
        SUPPLIER "${_copy_clean}"
        LICENSE "${_license_str}"
        VERSION "${_version}"
      )
    endif()
  endforeach()
endfunction(parse_copyright_file)

function(get_module_name_from_files FILE_LIST MODULE)
  list(LENGTH FILE_LIST _count)

  if(_count EQUAL 1)
    list(GET FILE_LIST 0 _f)
    get_filename_component(_name "${_f}" NAME_WE)
    set(${MODULE} "${_name}" PARENT_SCOPE)
    return()
  endif()

  list(GET FILE_LIST 0 _first)
  get_filename_component(_common "${_first}" DIRECTORY)

  foreach(_f IN LISTS FILE_LIST)
    # if the file is empty, skip it
    if("${_f}" STREQUAL "")
      continue()
    endif()

    get_filename_component(_dir "${_f}" DIRECTORY)

    while(NOT _dir MATCHES "^${_common}")
      get_filename_component(_common "${_common}" DIRECTORY)
    endwhile()
  endforeach()

  get_filename_component(_mod "${_common}" NAME)
  set(${MODULE} "${_mod}" PARENT_SCOPE)
endfunction()
