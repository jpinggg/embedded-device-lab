# High-Level Architecture

## Status

The C++ update-session core and its automated behavior tests are implemented.
The Python host tool, public protocol, payload storage, cryptographic
verification, persistence, reboot recovery, and retry behavior are not yet
implemented.

## Components

### Python Host Tool

Status: planned, not implemented.

The future host tool will use a project-specific public protocol to start an
update, send bounded image data, query status, and confirm the installed
simulated version.

### C++ Device Simulator

Status: partially implemented.

The current simulator provides:

- an installed `DeviceVersion`
- explicit update-session states
- pending-version ownership
- expected and received byte accounting
- guarded receive, verification-result, activation, cancellation, and reset
  operations
- observable terminal failure reasons

The byte-accounting API records only byte counts. It does not currently store
or inspect image payloads.

### Automated Test Suite

Status: implemented for the current C++ behavior.

CTest currently verifies:

- installed-version behavior
- valid and invalid update-session transitions
- rejected-operation state preservation
- exact transfer-size completion
- successful and failed verification-result handling
- activation and installed-version confirmation
- cancellation and reusable session reset
- failure-reason creation, preservation, and clearing

## Implemented Update Flow

```text
Idle
  -> Receiving
  -> Verifying
     -> ReadyToActivate
        -> Active
     -> Failed (VerificationFailed)

Receiving, Verifying, or ReadyToActivate
  -> Failed (Cancelled)

Active or Failed
  -> Idle (session reset)
```

`finishVerification(bool passed)` accepts an externally supplied result. The
current implementation does not perform hashing, signature verification, or
authenticity checks.

## Operation Contract

| Operation | Valid starting state | Additional condition | Accepted outcome | Rejected behavior |
|---|---|---|---|---|
| `start(version, expectedBytes)` | `Idle` | Version is nonempty and expected size is greater than zero. | Returns `true`, stores the pending version and expected size, clears the received count, and enters `Receiving`. | Returns `false` and preserves session data. |
| `receiveChunk(chunkBytes)` | `Receiving` | Chunk size is greater than zero and does not exceed the remaining expected size. | Returns `true`, increases the received count, and remains `Receiving`. | Returns `false` and preserves state, counters, and failure reason. |
| `finishReceiving()` | `Receiving` | Received size exactly equals expected size. | Returns `true` and enters `Verifying`; counters remain observable. | Returns `false` and preserves state, counters, and failure reason. |
| `finishVerification(passed)` | `Verifying` | The caller supplies the verification result. | Returns `true`; a passing result enters `ReadyToActivate`, while a failing result enters `Failed / VerificationFailed`. | An invalid starting state returns `false` and preserves state and reason. |
| `activate(activeVersion)` | `ReadyToActivate` | The pending version is nonempty. | Returns `true`, installs the pending version, and enters `Active`. | An invalid starting state returns `false` without change. The defensive empty-version path returns `false`, enters `Failed`, preserves the installed version, and leaves the reason as `None`; this path is not reachable through the normal public flow. |
| `cancel()` | `Receiving`, `Verifying`, or `ReadyToActivate` | None. | Returns `true`, clears pending transfer data, and enters `Failed / Cancelled`. | Returns `false` and preserves state, session data, installed version, and reason. |
| `resetSession()` | `Active` or `Failed` | None. | Returns `true`, clears pending transfer data and the failure reason, and enters `Idle / None`. | Returns `false` and preserves state, session data, installed version, and reason. |

## Ownership And Success Evidence

- `DeviceVersion` owns the currently installed simulated version.
- `UpdateSession` owns update state, pending version, transfer counters, and
  the current failure reason.
- `ReadyToActivate` does not prove installation.
- Successful activation is observable only when the session is `Active` and
  `DeviceVersion` contains the expected pending version.
- Resetting an update session does not change the installed version or start a
  new update automatically.

## Current Design Boundaries

- Byte totals do not prove payload integrity or detect duplicate chunks.
- No image payload, offset, chunk identifier, or retransmission model exists.
- No host protocol or Python host tool exists yet.
- No hashing, signature verification, authenticity check, or key material is
  implemented.
- No logging, persistence, reboot, power-loss recovery, retry orchestration,
  rollback, concurrency, hardware timing, power, or performance behavior is
  claimed.
- The repository does not reproduce a company product, proprietary packet
  format, internal command value, production key, or unreleased specification.
