srsRAN
======

[![Build Status](https://github.com/srsran/srsRAN_Project/actions/workflows/ccpp.yml/badge.svg?branch=main)](https://github.com/srsran/srsRAN_Project/actions/workflows/ccpp.yml)
[![OpenSSF Best Practices](https://www.bestpractices.dev/projects/7868/badge)](https://www.bestpractices.dev/projects/7868)

srsRAN is a complete 5G RAN solution, featuring an ORAN-native CU/DU developed by [SRS](http://www.srs.io).

The solution includes a complete L1/2/3 implementation with minimal external dependencies. Portable across processor architectures, the software has been optimized for x86 and ARM. srsRAN follows the 3GPP 5G system architecture implementing the functional splits between distributed unit (DU) and centralized unit (CU). The CU is further disaggregated into control plane (CU-CP) and user-plane (CU-UP).

srsRAN further follows O-RAN architecture principles, supports all external interfaces (F1, E1, E2, A1, O1, N2 and N3) and allows split 7.2a/b as well as split 8 deployments for the fronthaul connection.

License
-------

For license details, see [LICENSE](LICENSE) file.

Build Instructions
------------------

* Mandatory requirements:
  * cmake:               <https://cmake.org/>
  * libfftw:             <https://www.fftw.org/>
  * libsctp:             <https://github.com/sctp/lksctp-tools>
  * yaml-cpp:            <https://github.com/jbeder/yaml-cpp>
  * mbedTLS:             <https://www.trustedfirmware.org/projects/mbed-tls/>
  * git-lfs:             <https://git-lfs.github.com/>
  * googletest:          <https://github.com/google/googletest/>
    * You can skip test building by using the cmake option `-DBUILD_TESTS=False`. GoogleTest is not mandatory when building without tests.

You can install the required libraries for some example distributions with the commands below:

<details open>
<summary>Ubuntu 22.04</summary>


```bash
sudo apt-get install cmake make gcc g++ pkg-config libfftw3-dev libmbedtls-dev libsctp-dev libyaml-cpp-dev libgtest-dev git-lfs
git lfs install
```
</details>
<details>
<summary>Fedora</summary>


```bash
sudo yum install cmake make gcc gcc-c++ pkgconf fftw-devel lksctp-tools-devel yaml-cpp-devel mbedtls-devel gtest-devel git-lfs
git lfs install
```
</details>
<details>
<summary>Arch Linux</summary>


```bash
sudo pacman -S cmake make base-devel fftw mbedtls yaml-cpp lksctp-tools gtest git-lfs pkgconf
git lfs install
```
</details>

* Optional requirements:
  * UHD:                 <https://github.com/EttusResearch/uhd>
  * ZeroMQ:              <https://github.com/zeromq>
  * DPDK:                <https://www.dpdk.org/>

Download and build srsRAN:

```bash
git clone https://gitlab.com/softwareradiosystems/srsran.git
cd srsran
git lfs pull
mkdir build
cd build
cmake ../
make
make test
```

How to deploy a 5G network?
----------------------------------------------

Check our [tutorials](https://docs.srsran.com/projects/project/en/latest/tutorials/source/index.html) that describe how to set up an end-to-end 5G SA system with srsRAN Project.
