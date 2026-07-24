# Initial Requirements

## Purpose

The project will simulate a host-driven firmware update and the behavior of an embedded device receiving that update.

## Functional Requirements

1. The host tool shall start an update using public test metadata.
2. The device simulator shall expose explicit update states.
3. The host tool shall transfer an image in bounded chunks.
4. The device simulator shall validate metadata and image integrity before activation.
5. The simulator shall reject invalid or incomplete update data.
6. A successful update shall change the simulated firmware version only after final verification.
7. The host tool shall query the simulated device to confirm the expected version.

## Reliability Requirements

1. An interrupted transfer shall not activate an incomplete image.
2. Invalid metadata or corrupted image data shall produce a deterministic failure result.
3. Restarting the simulator after an interrupted update shall return it to a defined safe state.
4. Tests shall cover success, interruption, corruption, and recovery behavior.

## Security Requirements

1. The repository shall not contain private signing keys or credentials.
2. Cryptographic examples shall use public test material created only for this project.
3. Update validation shall distinguish integrity checks from authenticity checks.
4. Rejected images shall never become the active simulated firmware.

## Engineering Requirements

1. The device simulator shall be implemented in modern C++.
2. The host tool shall be implemented in Python.
3. The project shall use CMake and automated tests.
4. Continuous integration shall build and test the project.
5. Logs shall make update-state transitions and failure reasons observable.

## Out of Scope

- Real product firmware
- Proprietary protocols or packet formats
- Production keys or signing infrastructure
- Claims about hardware timing, power, or performance without measurements
