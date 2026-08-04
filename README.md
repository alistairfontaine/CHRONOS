# CHRONOS

<p align="center">
  <img src="assets/banner.png" alt="CHRONOS Banner" width="100%">
</p>

A zero-dependency, cache-aligned Genomic Sequence Search Matrix engineered in native C++17 to bypass cloud-computing overheads during viral mutation analysis.

## The Problem
Modern genomic search systems parse nucleotide sequences (`A, C, G, T`) using bloated text frameworks or cloud-dependent relational databases. This introduces major processing latencies when analyzing multi-gigabyte sequence files.

## The Architectural Solution
`CHRONOS` bypasses this complexity by packing nucleotides down to raw 2-bit binary representations, fitting massive genome tracking steps inside local CPU L1/L2 cache lines for immediate bitwise comparison loops.

## System Map
- **Core Engine:** Header-only genomic indexing layout (`include/ChronosCore.hpp`).
- **Control Interface:** High-velocity Command Line Interface (`src/main.cpp`).
- **Storage Subsystem:** High-density, serialized bit arrays (`.chronos`).
- **Verification Environment:** Dedicated local system tests (`tests/`).

## Local System Operations

### 1. Ingest Raw FASTA and Serialise to Binary Database
```bash
./chronos-cli --save tests/test_sequence.fasta tests/target_genome.chronos
```

### 2. High-Velocity Match Operations Off the Compressed File
```bash
./chronos-cli --load tests/target_genome.chronos ATCG
```
