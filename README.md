# Limit Order Book

## Introduction

This project is a Limit Order Book (LOB) implementation written in C++.

The project originated as an extension of a Red-Black Tree assignment from the Data Structures and Object-Oriented Programming (DSOOP) course. Building upon that foundation, the underlying Binary Search Tree (BST) and Red-Black Tree (RBT) were redesigned as generic data structures and extended with core LOB functionalities, including order management, order matching, order cancellation, and market data queries.

The long-term goal of this project is to support real point-in-time market data and serve as a learning and research platform for quantitative trading.

---

## Project Highlights

- Generic Binary Search Tree
	 
- Generic Red-Black Tree
	  
- Price-Time Priority Matching Engine
    
- O(1) Order Lookup via Hash Table
    

---

## Features

### Query Time Complexity

| Function           | Complexity |
| ------------------ | ---------- |
| Best Bid / Ask     | O(1)       |
| Volume At Price    | O(log N)   |
| Top-K Price Levels | O(K)       |
| Order Lookup       | O(1)       |

---

## Current Features

### Order Management

- Limit Order Placement
    
- Order Cancellation
    
- Order Matching
    
- Bid/Ask Book Management
    

### Supported Time-In-Force Types

- GTC (Good Till Cancelled)
    
- IOC (Immediate Or Cancel)
    
- FOK (Fill Or Kill)
    

### Market Data Queries

- Best Bid Price
    
- Best Ask Price
    
- Best Bid Volume
    
- Best Ask Volume
    
- Spread
    
- Mid Price
    
- Top-K Price Levels
    
- Volume At Price
    

### Status

The engine above (`L3_LOB`, order-level matching) is feature-complete and fully covered by `test/test_L3_LOB_order.cpp` / `test/test_L3_LOB_query.cpp` — everything listed passes tests. It currently has **no external interface**: no CLI, no file I/O, no CSV replay. It's used directly as a C++ class, exercised only through the test suite. This is a deliberate stopping point, not an oversight — real L3-level (order-by-order) market data is rare and expensive to obtain, so there's no natural data source to drive a CLI the way `main_program` (L2) has. Known, intentionally deferred gaps (execution/trade reporting, thread safety, sentinel values as `std::optional`, tick-integer price representation) are tracked in `doc/tech_debt.md`.

---

## Build & Run

This project is primarily developed and tested under Linux.

### Development Environment

- OS: MX Linux 23.6 (based on Debian 12)
    
- Compiler: g++ 12.2.0
    
- Standard: C++17
    

### Requirements

- C++17 or later
    
- GNU Make
    

### Build Test Program

```bash
make test_program
```

### Run Tests

```bash
make test
```

### Build Benchmark

```bash
make benchmark_program
```

### Run Benchmark

```bash
make run_benchmark
```

### Clean Build Artifacts

```bash
make clean
```

### Build & Run Main Program (L2 CLI)

`main_program` reads L2 market data (e.g. real Deribit order book data, see `sample_data/`) and runs queries against it in batch or streaming mode.

```bash
make main_program
./out/main_program -i <input CSV> -o <output CSV> -m <batch|streaming> --query <query>
```

Full CLI design (query types, modes, flags, output formats) is documented in `doc/cli_design.md`.

### Visualize / Inspect Query Output

```bash
python3 script/make_chart.py --input <output CSV path>
```

Reads `main_program`'s output CSV and auto-detects whether to print it as a table (batch mode) or plot it (streaming mode). Usage and examples are in `doc/chart_cli_design.md`.

---

## Roadmap

### Short-Term Goals

- Add more boundary tests
    
- Add randomized testing
    
- Expand benchmark coverage
    
- Improve documentation and code comments
    

### Long-Term Goals

- Support point-in-time market data input
    
- Order book state visualization
    
- Additional market analysis tools
    
- Support for more financial instruments
    

---

## Documentation

Docs in `doc/` are in Traditional Chinese (working notes); this README is the English-facing entry point.

**Usage guides** — how to actually run things:

```text
doc/
├─ main_program_usage.md    # main_program CLI: flags, queries, output columns, examples
└─ chart_cli_design.md      # make_chart.py usage guide
```

**Design notes / historical records** — why things are built this way, not how to use them:

```text
doc/
├─ architecture.md          # System design: RBT/PriceLevel/LOB structure
├─ cli_design.md            # main_program CLI design rationale and constraints
├─ l2_realdata_plan.md      # L2 real market data integration plan (mostly implemented)
├─ tech_debt.md             # Known, deferred technical debt
└─ benchmark_record.md      # Benchmark results history
```