"""
Utils
"""

import os


def get_current_pytest_suite_name():
    """
    Get current suite name
    """
    try:
        return os.environ.get("PYTEST_CURRENT_TEST").split("::")[0]
    except AttributeError:
        return ""


def get_current_pytest_test_name():
    """
    Get current test name
    """
    try:
        return os.environ.get("PYTEST_CURRENT_TEST").split("::")[1].split(" ")[0]
    except AttributeError:
        return ""
