srsGNB
======

srsGNB is a 5G-SA L1/L2/L3 gNodeB stack developed by [SRS](http://www.srs.io).

srsGNB follows the 3GPP 5G system architecture implementing the functional splits between distributed unit (DU)
and centralized unit (CU). The CU is further disaggregated into control plan (CU-CP) and user-plane (CU-UP).

srsGNB further follows O-RAN architecture principles, supports all external interfaces (F1, E1, E2, A1, O1, N2 and N3) and allows split 7.2a/b as well as split 8 deployments
for the fronthaul connection.

License
-------

For license details, see [LICENSE](LICENSE) file.

Build Instructions
------------------

* Mandatory requirements:
  * cmake:               <https://cmake.org/>
  * libfftw:             <https://www.fftw.org/>
  * PolarSSL/mbedTLS:    <https://www.trustedfirmware.org/projects/mbed-tls/>
  * git-lfs:             <https://git-lfs.github.com/>
  * googletest:          <https://github.com/google/googletest/>
    * You can skip test building by using the cmake option `-DBUILD_TESTS=False`. GoogleTest is not mandatory when building without tests.

For example, on Ubuntu 22.04, one can install the required libraries with:

```bash
sudo apt-get install cmake libfftw3-dev libmbedtls-dev git-lfs libgtest-dev
git lfs install
```

* Optional requirements
  * UHD:                 <https://github.com/EttusResearch/uhd>
  * ZeroMQ:              <https://github.com/zeromq>

Download and build srsGNB:

```bash
git clone https://gitlab.com/softwareradiosystems/srsgnb.git
cd srsgnb
git lfs pull
mkdir build
cd build
cmake ../
make
make test
```
