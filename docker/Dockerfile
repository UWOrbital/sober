FROM xanderhendriks/stm32cubeide:16.0

RUN apt-get update && apt-get install -y git bear clang-tidy gcc-arm-none-eabi && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace
COPY . .

RUN chmod +x scripts/*.bash