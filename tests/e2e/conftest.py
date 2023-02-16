"""
Pytest configuration
"""


def pytest_html_report_title(report):
    """
    Change title
    """
    report.title = "E2E Suite"
