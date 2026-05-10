# Data Link Layer Protocol Simulation

Simulation of **Go-Back-N** sliding-window transmission between two peers over a **noisy, delayed channel**, implemented as an OMNeT++ model (NED topology + C++ modules).

Byte stuffing frames upper-layer data for transmission; **even-parity over the stuffed payload** (single XOR parity byte, shipped as an 8-bit binary string) is used at the receiver to decide ACK versus negative/indicated failure behavior. A small **coordinator** module selects which node starts as sender and when.

---

## What This Model Demonstrates

- **Go-Back-N**: Sliding window (`WS`), sequence indices modulo window size, per-frame timeouts (`TO`), and retransmission from the earliest outstanding frame when the timer fires or feedback implies failure.
- **Byte stuffing**: Flag `$` and escape `/`; occurrences of `$` or `/` in the payload (after the first character in the implementation) are prefixed with `/`; the framed field is `$ … payload … $`.
- **Parity-based error detection**: Sender XORs all octets of the stuffed payload; receiver XORs payload octets with the decoded parity; zero XOR residue ⇒ treated as correct frame.
- **Controlled faults**: Per-message binary codes in the sender input file can trigger payload corruption (bit flip), dropped sends, duplication (with configurable spacing), and extra transmission delay together with probabilistic ACK loss (`LP`) at the receiver and propagation (`TD`), processing (`PT`), duplication gap (`DD`), and coordinator-scheduled extra delay (`ED`) where configured.

---

## Tech Stack

| Component | Role |
|-----------|------|
| **OMNeT++** | Discrete-event simulation kernel and IDE/build integration |
| **NED** | Network topology (`Network`: coordinator + two `Node0` instances + channels) |
| **C++** | Module logic (`node0`, `cordinator`), message class |

---

## Topology

Defined in `src/package.ned`:

- **`cordinator`**: Reads `coordinator.txt`, then notifies one node (ideal channels to each node’s `port2`) to become sender with a start time.
- **`node0` × 2**: Same compound module type; one acts as sender (`sender = 1` after coordinator signal), the other as receiver. They communicate over **`DelayChannel`** on mutual `port1`; coordinator messages arrive on `port2`.

---

## Prerequisites

- **[OMNeT++](https://omnetpp.org/)** installed with its toolchain on `PATH` (so `opp_makemake`, `opp_configfilepath`, etc. resolve).

Optional:

- **GNU Make** and a Unix-like shell if you build from the top-level `Makefile` (common under OMNeT++’s MinGW/Git Bash environment on Windows, or natively on Linux/macOS).

---

## Building

### OMNeT++ IDE

Import or open the project (`Project_v1`), ensure the OMNeT++ nature/builders are enabled, then build (typically generates `src/Makefile` via `opp_makemake` and produces `out/$(CONFIG)/src/Project_v1$(EXE_SUFFIX)`).

### Command line

From the repository root:

```bash
make makefiles    # only if src/Makefile is missing
make              # release build by default in typical setups
```

The canonical makefile expects OMNeT++’s `Makefile.inc` via `CONFIGFILE` / `OMNETPP_CONFIGFILE`.

---

## Configuration

Simulation-wide defaults live in **`src/omnetpp.ini`**. Important parameters (applied under `**` to both nodes unless overridden):

| Parameter | Meaning (typical use here) |
|-----------|---------------------------|
| `WS` | Go-Back-N window size |
| `TO` | Sender timeout per outstanding frame |
| `TD` | Channel propagation delay (`DelayChannel`) |
| `PT` | Receiver processing delay before scheduling ACK/NACK feedback |
| `ED` | Extra delay injected when an input-line fault enables delay |
| `DD` | Spacing between original and duplicate frame |
| `LP` | Probability [0,1] that an ACK feedback message is discarded |

Adjust **`seed-set`** under `[General]` for repeatable RNG sequences.

---

## Input Files

Place these next to the **working directory** of the simulation (often `src/` when launching from the IDE with default paths):

### `coordinator.txt`

Single line, format **`[<node_id>,<start_time>]`** (parsed by `Cordinator::readCoordinatorFile`). Example:

```text
[0,0]
```

Selects which node ID receives the start message (connections route that node to become sender) and the coordinator-supplied start time used when seeding the sender’s window.

### `input<id>.txt`

Lines define **one logical message** each:

```text
<flags> <payload>
```

The **flags** string is four characters (`0`/`1`), interpreted positionally by the sender when applying channel faults (see `Node0::sendWithErrors`):

| Index | When `1` |
|-------|----------|
| 0 | Flip one bit in the stuffed payload (simulate corruption) |
| 1 | Do not transmit this frame (loss at sender side) |
| 2 | Schedule a duplicate copy after `DD` |
| 3 | Add extra delay `ED` before send |

Example excerpt:

```text
0000 aaaa
0100 aa
0011 fa
```

---

## Running

### OMNeT++

Open **`src/omnetpp.ini`**, choose configuration/network **`Network`**, run with working directory set so `coordinator.txt` and `input*.txt` resolve.

### Shell helper

`simulations/run` is a thin wrapper (Unix shell):

```bash
cd simulations
chmod +x run
./run
```

It invokes the built executable with NED path `.:../src`. Adjust the executable name in that script if your build produces a different binary name or location on your platform.

---

## Project Layout

```text
├── Makefile                 # Top-level wrapper → src/
├── src/
│   ├── package.ned          # Network + submodule wiring
│   ├── node0.ned / node0.cc / node0.h
│   ├── cordinator.ned / cordinator.cc / cordinator.h   # Note: spelling in repo
│   ├── CustomMessage.msg    # Packet: header, payload, trailer, type, ack
│   ├── omnetpp.ini          # Main simulation config
│   ├── coordinator.txt      # Sender selection & start time
│   └── input0.txt           # Sample framed fault catalog & payloads
├── simulations/
│   ├── package.ned          # Simulation package imports
│   ├── omnetpp.ini          # Placeholder / alternate runs
│   └── run                  # Launch helper
└── out/                     # Build artifacts (generated)
```

---

## Message Types (`CustomMessage`)

Defined in `CustomMessage.msg`:

- **`header`**: Sequence number for Go-Back-N indexing.
- **`payload`**: Byte-stuffed frame body as string.
- **`trailer`**: 8-character `'0'/'1'` string representing parity bits.
- **`type` / `ack`**: Protocol discrimination fields for data vs acknowledgements (see comment in `.msg`).

Inspect **`CustomMessage_m.h`** after building for the generated API.

---

## License

Source headers indicate **GNU LGPL** for several generated OMNeT++ artifacts and modules; refer to the license blocks in individual `.ned`, `.msg`, and `.cc` files.

---

## Course Context

This repository implements a **data link layer protocol simulation**: reliable **windowed** delivery over an imperfect link using **stuffing**, **parity**, **timeouts**, and explicit modelling of **delay**, **duplication**, **errors**, and **loss**—with experiment control split between **`omnetpp.ini`** and per-message **`input*.txt`** fault vectors.
