#!/usr/bin/env python3

import argparse
import pathlib
import sys
from dataclasses import dataclass, field
from typing import Dict, List

try:
    import yaml
except ImportError:
    print("Install pyyaml: https://pypi.org/project/PyYAML/")
    exit(1)

try:
    import gitlab
except ImportError:
    print("Install Gitlab Python library: https://pypi.org/project/python-gitlab/")
    exit(1)


# pylint: disable=too-many-instance-attributes
@dataclass
class _TestDefinition:
    """ """

    id: str = ""
    campaign_filename: str = ""
    test_name: str = ""
    description: str = ""
    gnb_extra_config: Dict = field(default_factory=dict)


@dataclass
class _BuildDefinition:
    tag: str
    os: str
    compiler: str
    srs_target: str
    build_args: str
    dpdk_version: str
    uhd_version: str


BUILD_DEFINITIONS: Dict[str, _BuildDefinition] = {
    "standard": _BuildDefinition(
        tag="amd64-avx2-avx512",
        os="ubuntu-24.04",
        compiler="gcc",
        srs_target="gnb_split_7_2",
        build_args='-DCMAKE_BUILD_TYPE=Release -DFORCE_DEBUG_INFO=True -DENABLE_UHD=False -DENABLE_DPDK=True -DENABLE_ZEROMQ=False -DENABLE_FFTW=False -DENABLE_MKL=True -DMARCH="x86-64-v4"',
        dpdk_version="23.11.4_avx512",
        uhd_version="",
    ),
    "rtsan": _BuildDefinition(
        tag="amd64-avx2-avx512",
        os="ubuntu-24.04-rtsan",
        compiler="clang",
        srs_target="gnb_split_7_2",
        build_args='-DCMAKE_BUILD_TYPE=Release -DFORCE_DEBUG_INFO=True -DENABLE_UHD=False -DENABLE_DPDK=True -DENABLE_ZEROMQ=False -DENABLE_FFTW=False -DENABLE_MKL=True -DMARCH="x86-64-v4" -DENABLE_RTSAN=True -DENABLE_WERROR=False',
        dpdk_version="23.11.4_avx512",
        uhd_version="",
    ),
}


# pylint: disable=too-many-instance-attributes
@dataclass
class _ArgsDefinition:
    """ """

    testlist: bool = False
    token: str = ""
    branch: str = ""
    testid: str = ""
    campaign_path: str = ""
    timeout: int = ""
    gnb_cli: str = ""
    build_mode: str = ""


def _convert_extra_config_into_command(extra_config: dict) -> str:
    """
    Convert extra config into command
    """
    cmd_args = ""
    for key, value in sorted(extra_config.items(), key=lambda item: isinstance(item[1], dict)):
        if isinstance(value, dict):
            cmd_args += f"{key} " + _convert_extra_config_into_command(value)
        else:
            cmd_args += f"--{key}={value} "
    return cmd_args


def get_viavi_tests():
    viavi_test_declaration = (
        pathlib.Path(__file__).parent / ".." / "tests" / "e2e" / "tests" / "viavi" / "test_declaration.yml"
    ).resolve()
    with open(viavi_test_declaration, "r", encoding="utf-8") as file:
        data = yaml.safe_load(file)

    test_list: List[_TestDefinition] = []
    for test in data["tests"]:
        test_definition = _TestDefinition()
        test_definition.id = test["id"]
        test_definition.campaign_filename = test["campaign_filename"]
        test_definition.test_name = test["test_name"]
        test_definition.description = test.get("description", "")
        test_definition.gnb_extra_config = test.get("gnb_extra_config", "")
        test_list.append(test_definition)
    return test_list


def validate_args(args) -> _ArgsDefinition:
    args_definition = _ArgsDefinition()
    args_definition.testlist = args.testlist
    args_definition.token = args.token
    args_definition.branch = args.branch
    args_definition.testid = args.testid
    args_definition.timeout = args.timeout
    args_definition.gnb_cli = args.srsgnb_cli
    args_definition.build_mode = args.build_mode

    if not args_definition.testlist:
        fail_validate = False
        if not args_definition.token:
            print("Token is required to run a test.")
            fail_validate = True
        if not args_definition.branch:
            print("Branch is required to run a test.")
            fail_validate = True
        if not args_definition.testid:
            print("Testid is required to run a test.")
            fail_validate = True

        if fail_validate:
            exit(1)
    return args_definition


def show_test_list():
    test_list = get_viavi_tests()
    print("Available tests:")
    for test in test_list:
        print(f"    ⊛ id: {test.id}")
        print(f"        · Description: {test.description}")
        print()


def run_test(args_definition: _ArgsDefinition, test_definition: _TestDefinition):
    build_definition = BUILD_DEFINITIONS[args_definition.build_mode]
    timeout = args_definition.timeout if args_definition.timeout else 972800
    branch = args_definition.branch
    private_token = args_definition.token

    retina_launcher_args = f'--viavi-manual-campaign-filename "{test_definition.campaign_filename}" --viavi-manual-test-name "{test_definition.id}" --viavi-manual-test-timeout {timeout}'
    if args_definition.gnb_cli:
        retina_launcher_args += f' --viavi-manual-gnb-arguments "{args_definition.gnb_cli}"'
        print("")
        print(
            "⚠️  Using srsgnb-cli overwrites the configuration defined in the test_declaration.yml for the test. Please review your new config carefully!!"
        )
        print("⚠️  OLD configuration: ", _convert_extra_config_into_command(test_definition.gnb_extra_config))
        print("⚠️  NEW configuration: ", args_definition.gnb_cli)
        print("")
        if input("Do you want to continue with the new configuration? (yes/no): ").strip().lower() not in ("y", "yes"):
            print("Exiting as per user request.")
            sys.exit(0)
        print("")

    variables = [
        {"key": "INFRASTRUCTURE_TAG", "value": build_definition.tag},
        {"key": "OS", "value": build_definition.os},
        {"key": "COMPILER", "value": build_definition.compiler},
        {"key": "BUILD_ARGS", "value": build_definition.build_args},
        {"key": "SRS_TARGET", "value": build_definition.srs_target},
        {"key": "UHD_VERSION", "value": build_definition.uhd_version},
        {"key": "DPDK_VERSION", "value": build_definition.dpdk_version},
        {"key": "TEST_MODE", "value": "none"},
        {"key": "TESTBED", "value": "viavi"},
        {"key": "MARKERS", "value": "viavi_manual"},
        {"key": "KEYWORDS", "value": ""},
        {"key": "RETINA_LAUNCHER_ARGS", "value": retina_launcher_args},
        {"key": "RETINA_PARAM_ARGS", "value": "gnb.all.pcap=True gnb.all.rlc_enable=True gnb.all.rlc_rb_type=srb"},
        {"key": "E2E_LOG_LEVEL", "value": "warning"},
        {"key": "GROUP", "value": "viavi"},
        {"key": "CLUSTER", "value": "retina-e2e-amd64"},
        {"key": "PIPELINE_DESCRIPTION", "value": "Viavi manual test"},
    ]

    print(f"Creating Viavi pipeline for branch {branch}...")
    print(f"    - Test ID: {test_definition.id}")
    print(f"    - Build mode: {args_definition.build_mode}")
    print(f"      - OS {build_definition.os}")
    print(f"      - BUILD_ARGS {build_definition.build_args}")
    print(f"      - DPDK_VERSION {build_definition.dpdk_version}")

    gl = gitlab.Gitlab("https://gitlab.com", private_token=private_token)
    project = gl.projects.get("softwareradiosystems/srsgnb")
    pipeline = project.pipelines.create({"ref": branch, "variables": variables})

    pipeline_url = pipeline.web_url

    print(f"🟢 Pipeline created: {pipeline_url}")


def main():
    """
    Entrypoint runner.
    """
    parser = argparse.ArgumentParser(description="List the available tests (--testlist) or run a test.")
    parser.add_argument(
        "--testlist",
        action="store_true",
        help="List of tests to run.",
    )

    parser.add_argument(
        "--token",
        help="[REQUIRED] Gitlab private token: https://docs.gitlab.com/ee/user/profile/personal_access_tokens.html#create-a-personal-access-token",
    )

    parser.add_argument("--branch", help="[REQUIRED] Remote branch in srsgnb repository.")

    parser.add_argument(
        "--testid",
        help="[REQUIRED] Testid in the campaign.",
    )

    parser.add_argument(
        "--srsgnb-cli",
        default="",
        help='Arguments passed to the gnb binary. E.g: "log --all_level=info". This overwrites the arguments in the test_declaration.yml file.',
    )

    parser.add_argument(
        "--build-mode",
        help='Build mode for gnb. Default: "rtsan"',
        default="rtsan",
        choices=BUILD_DEFINITIONS.keys(),
    )

    parser.add_argument("--timeout", help="Timeout in seconds for the test")

    args_definition = validate_args(parser.parse_args())
    if args_definition.testlist:
        show_test_list()
    else:
        test_list = get_viavi_tests()
        for test_definition in test_list:
            if test_definition.id == args_definition.testid:
                run_test(args_definition, test_definition)
                break
        else:
            print(f"Testid {args_definition.testid} not found.")
        exit(1)


if __name__ == "__main__":
    main()
