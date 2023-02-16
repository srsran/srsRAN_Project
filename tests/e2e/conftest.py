#
# Copyright 2013-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

"""
Pytest configuration
"""

def pytest_html_report_title(report):
    """
    Change title
    """
    report.title = "E2E Suite"
