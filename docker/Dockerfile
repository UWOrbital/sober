FROM xanderhendriks/stm32cubeide:16.0

RUN apt-get update && apt-get install -y git bear clang-tidy && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace
COPY . .

RUN bash -lc 'for f in app/*.bash; do [ -f "${f}" ] && chmod +x "${f}"; done'