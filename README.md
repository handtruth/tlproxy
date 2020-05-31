TL Proxy
======================

This is a small single thread multiclient TCP and UNIX proxy.

How to use it?
----------------------

### 1. As Independent Program

```sh
tlproxy <local> <remote>
```

`local` and `remote` are server addresses in form: `<protocol>[:address[:port]]`.

Example:

```sh
tlproxy tcp::1337 unix:/var/run/docker.sock
```

Command above will create a TCP server at 127.0.0.1:1337 that redirects any connection to `/var/run/docker.sock` socket.

### 2. As Docker Container

```sh
docker run -p 25564:25564 handtruth/tlproxy tcp:0.0.0.0:25564 tcp:example.com:25565
```

After that you can connect to container throw 25564 TCP port. It will redirect you to example.com:25565.

How to build it?
----------------------

### 1. From Source

```sh
apt install -y cmake g++ python3 python3-pip \
  python3-setuptools python3-wheel ninja-build \
  && python3 -m pip install --user meson

meson build && cd build \
  && ninja -Dbuildtype=release -Doptimization=3 -Dsystemd=true
```

### 2. Docker Image

```
docker build -t tlproxy .
```
