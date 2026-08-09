# CS179M-Best-Load-Balancing-Project

A C++ simulation of a container-ship crane that automatically rearranges cargo containers to balance a vessel's weight distribution, built for UCR's CS179M. The solver plans crane moves using an A\* search over possible ship states and drives a small web UI that visualizes each move as it happens.

## Overview

Container ships load and unload boxes of varying weight in a fixed grid of bays. If the containers on the port and starboard sides (or fore/aft) aren't distributed evenly, the ship can list or become unstable. This project models a ship as a 9x12 grid of container slots plus a crane, then uses informed search to find a sequence of legal crane moves that brings the ship's weight distribution into balance.

The program:
1. Reads a manifest file describing where containers currently sit and how much they weigh.
2. Runs an A\* search over possible container arrangements, where each move is a legal crane pick-up/put-down.
3. Outputs the sequence of moves as a step-by-step log and a final "outbound" manifest.
4. Serves a local web UI that steps through the crane's moves visually.

## How It Works

- **State representation (`ShipNode`)** — each search state is a 9x12 grid (`Container` objects) representing every slot on the ship, plus the running port-side weight, starboard-side weight, and the crane's current position.
- **Search (`Problem::algo`)** — states are pushed onto a `priority_queue` ordered by `cost + heuristic` (a classic A\* formulation), where `cost` is the number of moves taken so far and the heuristic estimates how far a state is from a balanced ship. The search explores legal crane moves (an overhead crane can only pick up a container that is clear on top and obeys physical stacking/reachability rules) until it finds a balanced final state.
- **Path reconstruction** — once a balanced state is found, `traceSolutionPath()` walks back through parent pointers to recover the full sequence of moves from the initial manifest to the balanced solution.
- **Logging** — every move and program event is timestamped and written to a log file for auditing.

## Project Structure

```
.
├── main.cpp              # Entry point: starts an HTTP server, parses manifests, drives the solver
├── Problem.h / Problem.cpp   # Core solver: ShipNode/Container models, A* search, balance logic
├── httplib.h              # cpp-httplib (vendored single-header HTTP server library)
├── UI/                     # Web front end served at http://localhost:8080
│   └── json.hpp            # nlohmann/json (vendored single-header JSON library)
├── ShipCase*.txt           # Sample input manifests (starting container layouts)
├── ShipCase*OUTBOUND.txt   # Solver output: final balanced layouts for each case
├── Test*.txt / Test*OUTBOUND.txt  # Additional test manifests and their solved outputs
└── HMM_Algeciras.txt        # Additional sample manifest
```

## Getting Started

### Prerequisites
- A C++ compiler with C++11 (or later) support (e.g. `g++`, `clang++`)
- macOS/Linux (the program shells out to `open` to launch a browser tab — swap this for `xdg-open` on Linux or `start` on Windows if needed)

### Build

```bash
g++ -std=c++17 -o load_balancer main.cpp Problem.cpp
```

### Run

```bash
./load_balancer
```

This starts an HTTP server on `http://localhost:8080` and opens the bundled UI in your browser, from which you can submit a manifest and step through the crane's moves. The server also writes a timestamped log file documenting each run.

### Running a case directly

The solver reads manifests named `<name>.txt` and writes results to `<name>OUTBOUND.txt`. Several example manifests (`ShipCase1.txt` through `ShipCase6.txt`, `Test1.txt` through `Test4.txt`) are included so you can try the solver against pre-built scenarios via the UI's input field (enter the manifest name without the `.txt` extension).

## Manifest Format

Each line of an input manifest describes one container's starting position and weight, in the form:

```
[row,col], {weight}, container name
```

- `row`/`col` locate the container in the 9x12 grid (row 8 is the dock/park position for the crane).
- `weight` is the container's weight.
- `name` is a free-text label for the container.

The solver's output manifests (`*OUTBOUND.txt`) are written in the same format, reflecting the final balanced layout.

## Tech Stack

- **C++** — core solver and search algorithm
- [**cpp-httplib**](https://github.com/yhirose/cpp-httplib) — lightweight embedded HTTP server (vendored as `httplib.h`)
- [**nlohmann/json**](https://github.com/nlohmann/json) — JSON handling for UI communication (vendored as `UI/json.hpp`)
- **HTML/CSS/JS** — front-end visualization served from `UI/`

## Course Context

Built as a team project for **CS179M** at UC Riverside.

## License

No license specified — all rights reserved by the author unless otherwise noted.
