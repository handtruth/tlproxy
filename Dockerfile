FROM alpine:latest AS base
RUN [ "apk", "--no-cache", "--no-progress", "add", "libstdc++" ]

FROM base AS build
RUN apk --no-cache --no-progress add build-base ninja coreutils cmake git python3 \
    && python3 -m pip install meson
ADD . /tlproxy
WORKDIR /tlproxy
RUN meson -Dprefix=`pwd`/out -Dbuildtype=release -Ddefault_library=static -Doptimization=3 build \
    && cd build && ninja && ninja install

FROM base
COPY --from=build /tlproxy/out/bin/tlproxy /usr/local/bin/tlproxy
LABEL maintainer="ktlo <ktlo@handtruth.com>"
ENTRYPOINT [ "tlproxy" ]
