#!/bin/bash
#
# Copyright 2013-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set -e

echo "=================="
echo "= Update headers ="
echo "=================="

# for CMake/YML files
find . -type f -\( -name "CMakeLists.txt" -o -name "*.cmake" -o -name "*.yml" -o -name "*.sh" -o -name "*.py" -o -name "*.toml" -o -name "Dockerfile" -o -name "srsran_performance" -o -name ".gdbinit" \) \
    ! -path "*/configs/*" ! -path "*/.github/*" ! -path "*/.gitlab/*" ! -path "*/docker/open5gs/*" ! -name "FindBackward.cmake" \
    -print0 | while IFS= read -r -d '' file; do

    # Check header format
    found_header=false
    while IFS= read -r line; do
        if [ -z "$line" ] && [ "$found_header" = false ]; then
            continue # Ignore empty lines before first comment block
        elif [[ "$line" =~ ^#.*$ ]]; then
            found_header=true
            continue # This line start with #. Keep reading
        elif [[ -z "$line" ]]; then
            break # Empty line after the header block. The format is valid and exit.
        else
            echo "$file: Header (or empty line after it) is missing."
            exit 1
        fi
    done <"$file"

    # Replace header
    perl -0777 -pi -e "s/#[^!][\s\S]*?(?=\n.*?=|\n\n)/#
# Copyright 2021-$(date +%Y) Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and\/or modify
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
# and at http:\/\/www.gnu.org\/licenses\/.
#/" "$file"

done

# for actual source and header files
find . -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.h.in" \) \
    ! -path "*/external/*" ! -name "rfnoc_test.cc" \
    -exec perl -0777 -pi -e "s{/\*.*?\*/}{/*
 *
 * Copyright 2021-$(date +%Y) Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */}s" {} \;
