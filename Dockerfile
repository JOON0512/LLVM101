FROM ubuntu:22.04
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    clang-14 llvm-14-dev make g++ python3 ca-certificates && rm -rf /var/lib/apt/lists/*
ENV PATH="/usr/lib/llvm-14/bin:${PATH}"
WORKDIR /work
CMD ["python3", "tests/run.py"]
