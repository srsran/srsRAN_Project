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

