import argparse
try:
    import gitlab
except ImportError:
    print("Install Gitlab Python library: https://pypi.org/project/python-gitlab/")
    exit(1)

def main():
    """
    Entrypoint runner.
    """
    parser = argparse.ArgumentParser(description="Viavi CI runner.")
    parser.add_argument("--token", required=True, help="Gitlab private token: https://docs.gitlab.com/ee/user/profile/personal_access_tokens.html#create-a-personal-access-token")
    parser.add_argument("--branch", required=True, help="srsgnb git branch.")
    parser.add_argument("--campaign-path", required=True, help="Campaign path. E.g: C:\\ci\\CI 4x4 ORAN-FH.xml")
    parser.add_argument("--testname", required=True, help='Testname in the campaign. E.g: "32UE static DL + UL UDP - Dell"')
    parser.add_argument("--timeout", required=True, help="Timeout in seconds for the test")
    parser.add_argument("--loglevel", default="warning", required=False, choices=["info", "warning", "error"], help="srsgnb loglevel")
    args = parser.parse_args()

    private_token = args.token
    branch = args.branch
    campaign_name = args.campaign_path
    test_name = args.testname
    timeout = args.timeout
    loglevel = args.loglevel

    INFRASTRUCTURE_TAG = "on-prem-amd64-avx2-avx512"
    OS_NAME = "ubuntu-24.04"
    COMPILER = "gcc"
    TESTMODE = "none"

    MAKE_ARGS = "-j7"
    BUILD_ARGS = "-DCMAKE_BUILD_TYPE=Release -DAUTO_DETECT_ISA=True -DENABLE_UHD=False -DENABLE_DPDK=True -DENABLE_ZEROMQ=False -DENABLE_AVX512=True"
    DPDK_VERSION = "23.11"

    TESTBED = "viavi"
    MARKERS = "viavi_manual"

    PYARGS = f"--viavi-manual-campaign-filename \"{campaign_name}\" --viavi-manual-test-name \"{test_name}\" --viavi-manual-test-timeout {timeout}"
    RETINA_ARGS = "gnb.all.pcap=False gnb.all.enable_metrics=False"

    variables = [
        {
            'key': 'INFRASTRUCTURE_TAG',
            'value': INFRASTRUCTURE_TAG
        },
        {
            'key': 'OS',
            'value': OS_NAME
        },
        {
            'key': 'COMPILER',
            'value': COMPILER
        },
        {
            'key': 'TEST_MODE',
            'value': TESTMODE
        },
        {
            'key': 'BUILD_ARGS',
            'value': BUILD_ARGS
        },
        {
            'key': 'MAKE_ARGS',
            'value': MAKE_ARGS
        },
        {
            'key': 'UHD_VERSION',
            'value': ''
        },
        {
            'key': 'DPDK_VERSION',
            'value': DPDK_VERSION
        },
        {
            'key': 'TESTBED',
            'value': TESTBED
        },
        {
            'key': 'MARKERS',
            'value': MARKERS
        },
        {
            'key': 'KEYWORDS',
            'value': ''
        },
        {
            'key': 'PYTEST_ARGS',
            'value': PYARGS
        },
        {
            'key': 'RETINA_ARGS',
            'value': RETINA_ARGS
        },
        {
            'key': 'E2E_LOG_LEVEL',
            'value': loglevel
        },
        {
            'key': 'GROUP',
            'value': 'viavi',
        },
        {
            'key': "PIPELINE_DESCRIPTION",
            'value': "Viavi manual test"
        }
    ]

    print(f"Creating Viavi pipeline for branch {branch}...")
    print(f"    - BUILD_ARGS {BUILD_ARGS}")
    print(f"    - DPDK_VERSION {DPDK_VERSION}")
    print(f"    - OS {OS_NAME}")

    gl = gitlab.Gitlab('https://gitlab.com', private_token=private_token)
    project = gl.projects.get('softwareradiosystems/srsgnb')
    pipeline = project.pipelines.create({'ref': branch, 'variables': variables})

    pipeline_url = pipeline.web_url

    print(f"Pipeline created: {pipeline_url}")


if __name__ == "__main__":
    main()
