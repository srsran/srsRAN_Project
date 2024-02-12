#!/usr/bin/env python3
#
# Copyright 2013-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

"""
Calculate fingerprint of binaries (tests) and compare between two fingerprints to find which tests have changed.
"""

import json
import os
from argparse import ArgumentParser
from csv import QUOTE_ALL

import pandas as pd

KEY_TYPES = ["md5", "mtime", "skip"]


def _get_md5_sum(file_list):
    args = " ".join(f'"{f}"' for f in file_list)
    stream = os.popen(f"md5sum -b {args}")
    result = [l.split()[0] for l in stream.readlines()]
    return result


def _get_mod_time(file):
    return os.path.getmtime(file)


def _get_file_info(files):
    md5 = _get_md5_sum(files)
    mtime = [_get_mod_time(f) for f in files]
    return {"filename": files, "md5": md5, "mtime": mtime}


def _find_executables(directory):
    stream = os.popen(f"find -H {directory} -type f -perm -100")
    result = [l.rstrip() for l in stream.readlines()]
    return result


def _get_ctests(directory):
    stream = os.popen(f"ctest --test-dir {directory} --show-only=json-v1")
    js = stream.read()
    data = json.loads(js)
    return data


def _ctest_dict_to_df(data):
    entries = []
    test_num = 1
    for test_info in data["tests"]:
        entry = {"filename": test_info["command"][0], "name": test_info["name"], "number": test_num}
        entries.append(entry)
        test_num = test_num + 1
    df = pd.DataFrame(entries)
    return df


def _main():
    parser = ArgumentParser()
    parser.add_argument(
        "-b", "--build-dir", required=True, metavar="BUILD_DIR", help="build directory from which ctest will be run"
    )
    parser.add_argument(
        "-i", "--input-fingerprint", default=None, metavar="IN_CSV", help="compare against fingerprints in FILE"
    )
    parser.add_argument(
        "-o", "--output-fingerprint", default=None, metavar="OUT_CSV", help="store current fingerprints to FILE"
    )
    parser.add_argument(
        "-n", "--names", action="store_true", default=False, help="print test names instead of index to stdout"
    )
    parser.add_argument(
        "-d", "--distinguishing-feature", choices=KEY_TYPES, default="md5", help="distinguishing feature for comparison"
    )

    args = parser.parse_args()

    all_exec_list = _find_executables(os.path.abspath(args.build_dir))
    all_info_dict = _get_file_info(all_exec_list)
    all_info_df = pd.DataFrame(all_info_dict)
    if args.output_fingerprint is not None:
        all_info_df.to_csv(args.output_fingerprint, index=False, header=True, quoting=QUOTE_ALL)
    if args.input_fingerprint is not None:
        old_info_df = pd.read_csv(args.input_fingerprint, header=0, quoting=QUOTE_ALL)
        full_info_df = all_info_df.merge(old_info_df, how="left", on="filename", suffixes=("", "_old"))
        if args.distinguishing_feature == "md5":
            full_info_df = full_info_df[
                (full_info_df["md5_old"].isna()) | (full_info_df["md5"] != full_info_df["md5_old"])
            ]
        elif args.distinguishing_feature == "mtime":
            full_info_df = full_info_df[
                (full_info_df["mtime_old"].isna()) | (full_info_df["mtime"] - full_info_df["mtime_old"] > 0.01)
            ]
        ctest_dict = _get_ctests(args.build_dir)
        ctest_df = _ctest_dict_to_df(ctest_dict)
        modified_ctest_df = ctest_df.merge(full_info_df, how="inner", on="filename")
        if args.names:
            output = "\n".join([str(num) for num in modified_ctest_df["name"].values])
        else:
            output = ",".join([str(num) for num in modified_ctest_df["number"].values])
        print(output)


if __name__ == "__main__":
    _main()
