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
from pytest_html.html_report import HTMLReport


def pytest_html_report_title(report: HTMLReport):
    """
    Change title
    """
    report.title = "E2E Suite"
