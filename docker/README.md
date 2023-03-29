### Step 1. Build Docker Image ###

```bash
docker build -f docker/Dockerfile --build-arg UHD_VERSION=v4.3.0.0 --target srsran --tag srsran:latest .
```
Optinally you can use pre-built image also
```bash
docker pull rohankharade/srsran:latest
docker tag rohankharade/srsran:latest srsran:latest

```
### Step 2. Launch Tester ###
Update docker-compose file as per required configuration
* If you are using  core-network on same machine, then you can you reuse same AMF N1/N2 subnet as `external` bridge for srsran service 
* You can use `network_mode: host` if core and gnb is running on differnt machine over network

```bash
docker-compose -f docker/docker-compose.yaml up -d
```

Verify logs
```bash
$ docker logs srsran -f
Now setting these variables '@BAND@ @CH_BW@ @DL_ARFCN@ @NGAP_LOCAL_ADDR@ @NGAP_REMOTE_ADDR@ @PLMN@ @RX_GAIN@ @SCS@ @SRATE@ @TAC@ @TX_GAIN@ @UHD_TYPE@'
Done setting the configuration

Running gNB Service 

[INFO] [UHD] linux; GNU C++ version 9.4.0; Boost_107100; UHD_4.3.0.HEAD-0-g1f8fd345
[INFO] [LOGGING] Fastpath logging disabled at runtime.
[INFO] [B200] Detected Device: B210
[INFO] [B200] Operating over USB 3.
[INFO] [B200] Initialize CODEC control...
[INFO] [B200] Initialize Radio control...
[INFO] [B200] Performing register loopback test... 
[INFO] [B200] Register loopback test passed
[INFO] [B200] Performing register loopback test... 
[INFO] [B200] Register loopback test passed
[INFO] [B200] Setting master clock rate selection to 'automatic'.
[INFO] [B200] Asking for clock rate 16.000000 MHz... 
[INFO] [B200] Actually got clock rate 16.000000 MHz.
[INFO] [MULTI_USRP] Setting master clock rate selection to 'manual'.
[INFO] [B200] Asking for clock rate 23.040000 MHz... 
[INFO] [B200] Actually got clock rate 23.040000 MHz.
```

<br/>

* [Sample pcap for simcom ue + srsran docker + oai 5g cn docker](https://github.com/orion-belt/srsRAN_Project/files/11100673/srsran_simcom_oai_cn.zip)
