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

For detailed design and implementation notes, see:

```text
docs/
└─ architecture.md
```