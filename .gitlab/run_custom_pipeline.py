#!/usr/bin/env python3
"""
Run a custom pipeline by replicating a job
"""

import argparse
import re
import time
from typing import Dict, Tuple
import sys

GITLAB_URL = "https://gitlab.com"
NEEDS_REGEX = re.compile(r"Downloading artifacts for .* \((\d+)\)...", flags=re.MULTILINE)
VARIABLE_REGEX = re.compile(
    r"^(?:\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}\.\d{6}Z \d{2}O )?(\w+)=(.*)?$", flags=re.MULTILINE
)


try:
    import gitlab
    from gitlab.v4.objects import Project
except ImportError:
    print("Install Gitlab Python library: https://pypi.org/project/python-gitlab/")
    sys.exit(1)


def _parse_args() -> Tuple[str, str, str, str, str, int, bool]:
    parser = argparse.ArgumentParser(description="Run a custom pipeline")
    parser.add_argument(
        "--token",
        required=True,
        type=str,
        help="Gitlab private token: https://docs.gitlab.com/ee/user/profile/personal_access_tokens.html#create-a-personal-access-token",
    )
    parser.add_argument(
        "--project",
        required=False,
        type=str,
        default="softwareradiosystems/srsgnb",
        help="Project full name in group/name format (default: %(default)s)",
    )
    parser.add_argument("--branch", required=True, type=str, help="Remote branch in srsgnb repository.")
    parser.add_argument(
        "--job",
        required=True,
        type=str,
        help='Job name. Please use "" when the job name contains spaces',
    )
    parser.add_argument(
        "--test",
        required=False,
        type=str,
        default="",
        help="In E2E jobs, the name of the test to run (default: empty for all tests defined in the job). "
        "Use full name, f.e. tests/attach_detach.py::test_zmq[band:3-scs:15-bandwidth:50-udp-uplink]",
    )
    parser.add_argument(
        "--timeout", required=False, type=int, default=300, help="Search for job timeout (default: %(default)s)"
    )
    parser.add_argument(
        "--dryrun", action="store_true", help="Search the job but skip pipeline creation (default: false)"
    )
    args = parser.parse_args()
    return args.token, args.project, args.branch, args.job.strip(), args.test.strip(), args.timeout, args.dryrun


def _get_project(token: str, instance: str, project: str) -> Project:
    gl = gitlab.Gitlab(instance, private_token=token)
    return gl.projects.get(project)


def _search_job_by_name(project: Project, job_name: str, timeout: int) -> Dict[str, str]:
    variable_dict = {}

    print("⏳ Looking for the job...")
    time_to_reach = time.time() + timeout
    for pipeline in project.pipelines.list(iterator=True, source="schedule"):
        for job in pipeline.jobs.list(iterator=True):
            if job.name == job_name:
                variable_dict.update(_extract_variables_from_job(project, job.id))
                if not variable_dict:
                    continue  # If the variable dict is empty, keep searching
                if "TESTBED" not in variable_dict:
                    variable_dict["TESTBED"] = "none"
                return variable_dict
            if time.time() >= time_to_reach:
                print(
                    "⛔ Timeout reached looking for the job. Please review job's name or increase this timeout by setting --timeout ⛔"
                )
                sys.exit(1)

    return variable_dict


def _extract_variables_from_job(project: Project, job_id: int) -> Dict[str, str]:
    job = project.jobs.get(job_id)
    if "driver" in job.name:
        return {}  # Filter out driver jobs
    job_log = job.trace().decode("utf-8")
    if not job_log:
        return {}  # Filter out jobs without output - we can't extract variables
    print(f'🟢 Job "{job.name}" found (id: {job.id})')

    variable_dict = {}
    for needs_job_id in re.findall(NEEDS_REGEX, job_log):
        variable_dict.update(_extract_variables_from_job(project, needs_job_id))
    for item in re.findall(VARIABLE_REGEX, job_log):
        key, value = item
        if key.isupper() and "$" not in value:
            variable_dict[key] = value.strip()

    return variable_dict


def _create_pipeline(project: Project, branch: str, variables: Dict[str, str], test: str, dryrun: bool):
    variable_array = []
    if test and variables.get("TESTBED", "none") != "none":
        print(f"  - Requested test: {test}")
        variables["E2E_FILE_OR_DIR"] = test
    print("⏩ Creating pipeline with variables:")
    for key, value in variables.items():
        if value:
            print(f"  - {key}={value}")
            variable_array.append({"key": key, "value": value})
    if not dryrun:
        pipeline = project.pipelines.create({"ref": branch, "variables": variable_array})
        print("✅ Pipeline created: ", pipeline.web_url)
    else:
        print("🟰 Pipeline creation skipped due to dryrun mode")


def main():
    """
    Entrypoint runner.
    """
    try:
        token, project_name, branch, job, test, timeout, dryrun = _parse_args()
        project = _get_project(token=token, instance=GITLAB_URL, project=project_name)
        variable_dict = _search_job_by_name(project=project, job_name=job, timeout=timeout)
        _create_pipeline(project=project, branch=branch, variables=variable_dict, test=test, dryrun=dryrun)
    except KeyboardInterrupt:
        print()
        print("⛔ Process cancelled by user ⛔")
        sys.exit(1)


if __name__ == "__main__":
    main()
