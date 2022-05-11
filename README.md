srsGNB
======

srsGNB is a 5G-SA L1/L2/L3 gNodeB stack developed by [SRS](http://www.srs.io).

srsGNB follows the 3GPP 5G system architecture implementing the functional splits between distributed unit (DU)
and centralized unit (CU). The CU is further disaggregated into control plan (CU-CP) and user-plane (CU-UP).

srsGNB further follows O-RAN architecture principles, supports all external interfaces (F1, E1, E2, A1, O1, N2 and N3) and allows split 7.2a/b as well as split 8 deployments
for the fronthaul connection.


License
-------

For license details, see LICENSE file.

Build Instructions
------------------

* Mandatory requirements:
  * cmake:               https://cmake.org/
  * libfftw:             http://www.fftw.org/


For example, on Ubuntu 22.04, one can install the required libraries with:
```
sudo apt-get install cmake libfftw3-dev
```

* Optional requirements
  * UHD:                 https://github.com/EttusResearch/uhd
  * ZeroMQ:              https://github.com/zeromq


Download and build srsGNB:
```
git clone https://github.com/softwareradiosystems/srsgnb.git
cd srsGNB
mkdir build
cd build
cmake ../
make
make test
```