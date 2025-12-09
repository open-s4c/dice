# Simple CICD container with smolBSD

This repository is an example of how to use [smolBSD](https://smolbsd.org) as
a container in a CICD pipeline. Since smolBSD can boot very quickly, it is a
good tool to run tests cases without booting a heavy-weight VM.

The idea is simple. It consists of these phases:

## 1. creating a Docker container

The steps below are in `.github/workflows/docker.yml`. The workflow is
triggered manually.

### 1.1 Create the VM image

Starting with a Ubuntu, install the dependencies for smolBSD:

    sudo apt install -y \
       build-essential  \
       curl             \
       git              \
       libarchive-tools \
       qemu-system      \
       rsync            \
       uuid-runtime

Next, adapt the service `cicd/`. This will be the basis of the smolBSD image.
The `options.mk` file defines the required sets and packages. Check `etc/rc`,
here we call a scripts `run.sh` in `/mnt` (if it exists).

Finally, create the VM image with `scripts/create-smolbsd.sh cicd smolBSD`. This
will create an image `cicd-ARCH.img` in `smolBSD/images/` with the dependencies
listed in `options.mk`.

### 1.2 Save the VM image in a Docker container image

Build a Docker image with the `Dockerfile` in the repo. It will install smolBSD
and copy the generated `cicd-ARCH.img` into `/smolBSD/`.

After that, push the Docker image to the CICD registry.

## 2. Running the tests

Whenever code changes in our example project are pushed, we'd like to test them
running the following steps in a Docker container from the previous phase check
`.github/workflows/example.yml` for details).

1. `scripts/pack-payload.sh <PAYLOAD_IMG> <FILE> [FILE]..`: add files to a
   FAT image that will be mounted on `/mnt` of `cicd-ARCH.img`.
   Here, we tar our test is in `example/` and also copy `example/run.sh`
   to the root of the image.
2. `scripts/run-payload.sh <SMOLBSD_DIR> <SERVICE> <PAYLOAD_IMG>` runs the
   test(s) in QEMU. It ensures the test results are saved back to the
   payload image.
3. `scripts/unpack-payload.sh <PAYLOAD_IMG> <DIR>` will copy the files of
   the image to a local directory.
4. `example/check-results.h <DIR>` checks if the expected files created by
   `example/run.sh` indicate that the tests pass.

The files `run.sh` and `check-results.sh` are the configuration points to
the specific example.

