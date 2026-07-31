# Secure Embedded Device Update Lab

## Overview

This clean-room project demonstrates a secure and reliable firmware-update workflow for a simulated embedded device.

The project will separate the host update tool from the device simulator so that update behavior, failure handling, and security checks can be tested without proprietary hardware or company source code.

## Planned Scope

- Define a simple public update protocol.
- Build a C++ device simulator with explicit update states.
- Build a Python host tool that sends update commands and data.
- Validate update metadata before accepting an image.
- Simulate interrupted transfers, invalid metadata, and recovery behavior.
- Add automated tests and continuous integration.

## Current Status

Initial C++ implementation is in progress. The repository currently provides a reusable `DeviceVersion` library, a demo executable, and an automated behavior test registered with CTest. The secure update workflow and Python host tool have not been implemented yet.

## Build and Test

Configure the build directory with testing enabled:

```bash
cmake -S . -B build/cmake -DBUILD_TESTING=ON
```

Build the library, demo, and test executable:

```bash
cmake --build build/cmake
```

Run the demo:

```bash
./build/cmake/device_version_demo
```

Run the automated tests:

```bash
ctest --test-dir build/cmake --output-on-failure
```

Generated build files are stored under `build/` and excluded from version control by `.gitignore`.

## Public-Safety Boundary

This repository is a clean-room learning project. It must not contain:

- company or customer source code
- product names or internal identifiers
- proprietary command values or packet formats
- private keys, credentials, or signing secrets
- unreleased specifications or internal documents
