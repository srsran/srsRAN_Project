
Artifact Transfer
------------------

There is an script that automates the compression and secure transfer of test artifacts generated during the testing process.

### Steps:

1. **Verify SSH Connection:** This checks if the SSH connection to the remote server is active before proceeding. If the connection fails, it exits to prevent errors.
2. **Prepare Artifacts:** Artifacts are gathered from the directory `${SRSRANDIR}/tests/e2e/log` and compressed into a single zip file. The file is named dynamically based on the job ID and context to ensure uniqueness.
3. **Transfer Artifacts:** The compressed file is transferred to the remote server using `scp`. The destination path is already configured.

You can find the code in Artifact transfer section
