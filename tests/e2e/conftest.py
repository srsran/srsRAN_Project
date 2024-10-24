#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

"""
Pytest configuration
"""
import pytest
from pytest_html.html_report import HTMLReport


def pytest_html_report_title(report: HTMLReport):
    """
    Change title
    """
    report.title = "E2E Suite"


def pytest_addoption(parser: pytest.Parser):
    """
    Add Viavi options to pytest
    """
    parser.addoption(
        "--viavi-manual-campaign-filename", action="store", default="default_filename", help="Viavi campaign filename"
    )
    parser.addoption("--viavi-manual-test-name", action="store", default="default_test", help="Viavi test name")
    parser.addoption("--viavi-manual-test-timeout", action="store", type=int, default=1800, help="Viavi test timeout")
    parser.addoption(
        "--viavi-manual-extra-gnb-arguments", action="store", type=str, default="", help="Viavi gnb extra arguments"
    )
