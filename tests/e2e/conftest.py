#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

"""
Pytest configuration
"""
import pytest
import os
from collections import OrderedDict
from pytest_metadata.plugin import metadata_key


def pytest_configure(config):
    md = config.stash[metadata_key]
    md.clear()
    md.update(
        OrderedDict(
            {
                "CI_COMMIT_SHA": os.getenv("CI_COMMIT_SHA", "N/A"),
                "CI_JOB_NAME": os.getenv("CI_JOB_NAME", "N/A"),
                "CI_JOB_ID": os.getenv("CI_JOB_ID", "N/A"),
                "CI_PIPELINE_ID": os.getenv("CI_PIPELINE_ID", "N/A"),
            }
        )
    )


def pytest_addoption(parser: pytest.Parser):
    """
    Add Viavi options to pytest
    """
    parser.addoption(
        "--viavi-manual-campaign-filename", action="store", default="default_filename", help="Viavi campaign filename"
    )
    parser.addoption("--viavi-manual-test-name", action="store", default="default_test", help="Viavi test name")
    parser.addoption("--viavi-manual-test-timeout", action="store", type=int, default=1800, help="Viavi test timeout")
    parser.addoption("--viavi-manual-gnb-arguments", action="store", type=str, default="", help="Viavi gnb arguments")
