"""
Utils
"""

import os


def get_current_pytest_suite_name():
    """
    Get current pytest name
    """
    testname = ""
    pytest_env = os.environ.get("PYTEST_CURRENT_TEST")
    if pytest_env is not None:
        testname = pytest_env.split("::")[1]
    return testname


def get_current_pytest_test_name():
    """
    Get current pytest name
    """
    testname = ""
    pytest_env = os.environ.get("PYTEST_CURRENT_TEST")
    if pytest_env is not None:
        tts = pytest_env.split("::")
        tts.pop(0)
        testname = "::".join(tts)
    return testname.split(" ", maxsplit=1)[0]
