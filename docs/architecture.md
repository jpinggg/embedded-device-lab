# High-Level Architecture

## Status

This document describes the planned clean-room architecture. The components have not been implemented yet.

## Components

### Python Host Tool

The host tool will:

- prepare public test metadata and image data
- send update commands and bounded data chunks
- query update status and the simulated firmware version
- report deterministic success or failure results

### C++ Device Simulator

The device simulator will:

- receive update metadata and image chunks
- maintain explicit update states
- validate integrity and authenticity using public test material
- reject invalid or incomplete updates
- activate a new simulated version only after final verification

### Automated Test Suite

The test suite will verify:

- successful update behavior
- interrupted transfer handling
- corrupted metadata and image rejection
- invalid authenticity data rejection
- restart and recovery behavior
- final version confirmation

## Planned Update Flow

```text
Host starts update
-> Device validates metadata
-> Host sends bounded chunks
-> Device validates the complete image
-> Device activates the simulated version
-> Host confirms the expected version
```

## Planned Device States

```text
Idle
-> Receiving
-> Verifying
-> ReadyToActivate
-> Active
```

Any invalid input or interrupted operation shall move the simulator to a defined failure or recovery path instead of activating incomplete data.

## Design Boundaries

- The protocol will be created specifically for this public project.
- The simulator will not reproduce a company product or proprietary packet format.
- Cryptographic examples will use project-only public test material.
- Hardware timing, power, and performance claims remain out of scope until measured on suitable public hardware.
