# CHRONOS: ENGINE ARCHITECTURE & MEMORY TOPOLOGY

This document details the hardware alignment strategy and memory compression structures engineered inside the `CHRONOS` layout framework.

## 1. Linear Bit-Packing Model
Standard nucleotide processing tools parse text arrays mapping each element as a full 8-bit ASCII character. This introduces heavy data footprints that trigger constant hardware bus cache evictions.

`CHRONOS` consolidates sequences using a **2-Bit Compact Integer Sequence Projection Model**.

```text
  ASCII Input Sequence:   [ 'A' ]   [ 'C' ]   [ 'G' ]   [ 'T' ]  -> 32 Bits (4 Bytes)

                           |         |         |         |
  Bit-Packed Register:    [ 00 ]    [ 01 ]    [ 10 ]    [ 11 ]   -> 8 Bits  (1 Byte)
```

### CPU Word Composition
- Each base pair takes precisely \(2\text{ bits}\) of active space.
- A native \(64\text{-bit}\) hardware machine word holds exactly \(32\text{ base pairs}\) (\(32 \times 2 = 64\text{ bits}\)).
- The core tracking record utilizes `alignas(8)` instructions to align arrays along precise \(8\text{-byte}\) CPU cache line strides. This eliminates misaligned retrieval delays during iteration loops.

## 2. Low-Level Core Memory Interface Pipeline
```text
  +-----------------------------------------------------------------------+

  |                   L1/L2 Cache Aligned Memory Strip                    |
  |                                                                       |
  |  +------------------------+  +------------------------+  +---------+  |
  |  | GenomicWord (64-bit)   |  | GenomicWord (64-bit)   |  | ...     |  |
  |  | 32 Base Pairs Packed   |  | 32 Base Pairs Packed   |  |         |  |
  |  +------------------------+  +------------------------+  +---------+  |
  +-----------------------------------------------------------------------+
```
By deploying data within contiguous memory spaces inside a flat `std::vector`, we can loop through operations sequentially without navigating individual pointer paths. This technique prevents CPU cache misses during massive scans.
