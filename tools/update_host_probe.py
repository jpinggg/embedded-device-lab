#!/usr/bin/env python3

import subprocess
import sys

REQUIRED_FIELDS = {
    "command",
    "accepted",
    "state",
    "active_version",
    "expected_bytes",
    "received_bytes",
    "failure",
}


def parse_status(line: str) -> dict[str, str]:
    fields: dict[str, str] = {}

    for token in line.strip().split():
        key, separator, value = token.partition("=")

        if not separator or not key or not value:
            raise ValueError(f"Malformed status field: {token!r}")

        if key in fields:
            raise ValueError(f"Duplicate status field: {key}")

        fields[key] = value

    missing = REQUIRED_FIELDS - fields.keys()

    if missing:
        names = ", ".join(sorted(missing))
        raise ValueError(f"Missing status fields: {names}")

    return fields


def require_values(
    fields: dict[str, str],
    expected: dict[str, str],
) -> None:
    for key, expected_value in expected.items():
        actual_value = fields[key]

        if actual_value != expected_value:
            raise ValueError(
                f"Unexpected {key}: "
                f"expected {expected_value!r}, got {actual_value!r}"
            )


def send_command(
    process: subprocess.Popen,
    command: str,
) -> dict[str, str]:
    if process.stdin is None or process.stdout is None:
        raise RuntimeError("Child process pipes are unavailable")

    process.stdin.write(f"{command}\n")
    process.stdin.flush()

    line = process.stdout.readline()

    if not line:
        raise RuntimeError(
            f"CLI exited without responding to {command!r}"
        )

    return parse_status(line)


def main(arguments: list[str]) -> int:
    if len(arguments) != 2:
        print(
            "Usage: update_host_probe.py <update_simulator_cli>",
            file=sys.stderr,
        )
        return 2

    try:
        process = subprocess.Popen(
            [arguments[1]],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            text=True,
        )
    except OSError as error:
        print(f"host_probe=failed error={error}", file=sys.stderr)
        return 1

    try:
        status = send_command(process, "status")
        require_values(
            status,
            {
                "command": "status",
                "accepted": "true",
                "state": "Idle",
                "active_version": "v1.0",
                "expected_bytes": "0",
                "received_bytes": "0",
                "failure": "None",
            },
        )

        quit_status = send_command(process, "quit")
        require_values(
            quit_status,
            {
                "command": "quit",
                "accepted": "true",
            },
        )

        return_code = process.wait(timeout=5)

        if return_code != 0:
            raise RuntimeError(
                f"CLI exited with status {return_code}"
            )
    except (
        OSError,
        RuntimeError,
        ValueError,
        subprocess.TimeoutExpired,
    ) as error:
        print(f"host_probe=failed error={error}", file=sys.stderr)
        return 1
    finally:
        if process.poll() is None:
            process.kill()
            process.wait()

    print(
        "host_probe=passed "
        f"state={status['state']} "
        f"active_version={status['active_version']}"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
