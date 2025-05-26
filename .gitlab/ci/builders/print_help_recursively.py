#!/usr/bin/env python3
#
# Copyright 2013-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

"""
Traverse the help output of an application recursively for all subcommands
"""

from argparse import ArgumentParser
import subprocess
import re

HELP_ARG = "--help"
SUBCOMMANDS_KEYWORD = "Subcommands:"

re_parser = re.compile(r'\s+([^\s]+)\s+')

def _parse_subcommands(stdout):
    result = []

    # find the subcommand section in the help output
    subcmd_section = stdout.split(SUBCOMMANDS_KEYWORD,1)

    if len(subcmd_section) > 1:
        subcmd_lines = subcmd_section[1].strip("\n").splitlines()
        for line in subcmd_lines:
            # find the subcommand name in the first capture group
            m = re_parser.match(line)
            if m :
                subcmd = m.group(1)
                result.append(subcmd)
            else:
                print("Warning: No matching subcommand found in line:")
                print(f"> {line}")
    return result

def _format_output(output):
    # Format and fine-tune the help output, e.g. remove the startup banner
    lines = output.splitlines()
    for line in lines:
        # ignore startup banner
        if line.find("--==") != -1 and line.find("==--") != -1:
            continue
        print(line)

def _traverse_help(app, cur_subcmd, help_arg):
    cmd = [app] + cur_subcmd + [help_arg]
    result = subprocess.run(cmd, capture_output=True, text=True, check=True)
    stdout = result.stdout
    _format_output(stdout)
    subcmd_children = _parse_subcommands(stdout)
    for subcmd_child in subcmd_children:
        _traverse_help(app, cur_subcmd + [subcmd_child], help_arg)

def _main():
    parser = ArgumentParser()
    parser.add_argument("app", nargs=1)

    args = parser.parse_args()

    app = args.app[0]

    _traverse_help(app, [], HELP_ARG)


if __name__ == "__main__":
    _main()
