#!/bin/bash
#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set -e

# Checks the git status and tries to `git rm` the files removed from
# the source branch to the commit.
rm_deleted_by_them() {
    delete_name="deleted by them:"
    delete_size_next=${#delete_name}+1
    lines=$(git status)
    while read -r line; do
        status=${line:0:${#delete_name}}

        if [[ "$status" == "$delete_name" ]]; then
            path_name=${line:$delete_size_next:${#line}}
            git rm "$path_name"
        fi

    done <<<"$lines"
}

# Checks the git status and tries to `git rm` the files removed from
# the target branch to the commit.
rm_deleted_by_us() {
    echo "=================="
    echo "= Remove deleted ="
    echo "=================="

    delete_name="deleted by us:"
    delete_size_next=${#delete_name}+3
    lines=$(git status)
    while read -r line; do
        status=${line:0:${#delete_name}}

        if [[ "$status" == "$delete_name" ]]; then
            path_name=${line:$delete_size_next:${#line}}
            git rm "$path_name"
        fi

    done <<<"$lines"
}

# Fixes the conflicts using theirs and adds them to the commit.
fix_conflicts_using_theirs() {
    echo "================="
    echo "= Fix conflicts ="
    echo "================="

    conflict_name="both modified:"
    conflict_size_next=${#conflict_name}+3
    lines=$(git status)
    while read -r line; do
        status=${line:0:${#conflict_name}}

        if [[ "$status" == "$conflict_name" ]]; then
            path_name=${line:$conflict_size_next:${#line}}
            echo "Resolving using theirs $path_name"
            git diff ORIG_HEAD MERGE_HEAD "$path_name"
            git checkout --theirs "$path_name"
            git add "$path_name"
        fi

    done <<<"$lines"

    conflict_name="both added:"
    conflict_size_next=${#conflict_name}+6
    lines=$(git status)
    while read -r line; do
        status=${line:0:${#conflict_name}}

        if [[ "$status" == "$conflict_name" ]]; then
            path_name=${line:$conflict_size_next:${#line}}
            echo "Resolving using theirs $path_name"
            git diff ORIG_HEAD MERGE_HEAD "$path_name"
            git checkout --theirs "$path_name"
            git add "$path_name"
        fi

    done <<<"$lines"

}

remove_lfs_files() {
    echo "===================="
    echo "= Remove lfs files ="
    echo "===================="

    while read -r line; do
        git rm --cached "$line"
    done < <(git lfs ls-files | sed -r 's/^.{13}//')
}

update_headers() {
    echo "=================="
    echo "= Update headers ="
    echo "=================="

    # for actual source and header files
    find . -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.h.in" \) ! -path "*/external/*" ! -name "rfnoc_test.cc" -exec perl -0777 -pi -e "s{/\*.*?\*/}{/*
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

    # for CMake/YML files
    find . -type f -\( -name "CMakeLists.txt" -o -name "*.cmake" -o -name "*.yml" -o -name "*.sh" -o -name "Dockerfile" \) ! -path "*/configs/*" ! -path "*/.github/*" ! -path "*/.gitlab/*" ! -path "*/docker/open5gs/*" ! -name "FindBackward.cmake" ! -name "FindRapidJSON.cmake" ! -name "CheckCSourceRuns.cmake" ! -name "CheckFunctionExistsMath.cmake" -exec perl -0777 -pi -e "s/#[^!][\s\S]*?(?=\n.*?=|\n\n)/#
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
#/" {} \;
}

main() {
    # Check number of args
    if [ $# != 2 ] && [ $# != 3 ]; then
        echo >&2 "Please call script with source branch and target branch as argument"
        echo >&2 "E.g. ./auto_merge.sh <source_branch> <target_branch> [dryrun]"
        exit 1
    fi

    local source_branch=$1
    local target_branch=$2
    local mode="${3:-push}"

    # Checkout target branch
    git fetch -q origin "$target_branch"
    if ! git checkout -b "$target_branch" FETCH_HEAD; then
        echo "Error: Couldn't checkout ${target_branch}."
        exit 1
    fi

    # Merge
    echo "========="
    echo "= Merge ="
    echo "========="
    git fetch -q origin "$source_branch"
    # Git merge will always generate a merge commit because history is different in both branches
    if ! git merge "$source_branch" -m "Update main"; then
        # There are conflicts
        git status
        rm_deleted_by_us
        rm_deleted_by_them
        fix_conflicts_using_theirs
        git commit --no-edit
    fi
    remove_lfs_files
    update_headers
    git commit -a --amend --no-edit

    # Push
    if [ "$mode" = "push" ]; then
        git push origin "$target_branch"
    fi
}

main "$@"
