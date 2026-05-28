FROM xanderhendriks/stm32cubeide:16.0

RUN apt-get update && apt-get install -y git && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace
COPY . .