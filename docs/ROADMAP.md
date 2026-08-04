# CHRONOS DEVELOPMENT ROADMAP

## Phase 1: Foundation (Completed)
- [x] Establish directory structures and zero-dependency ecosystem.
- [x] Design 2-bit base packing primitives (`A, C, G, T -> 00, 01, 10, 11`).
- [x] Implement linear memory heap storage arrays (`std::vector<GenomicWord>`).
- [x] Complete raw FASTA sequential file stream reader module.
- [x] Complete 64-bit cross-word sliding window bitmask query matching loop.

## Phase 2: Serialization & Verification (Current)
- [ ] Implement zero-copy `.chronos` custom binary file format persistence exporter.
- [ ] Implement direct-to-memory `.chronos` index file recovery loader.
- [ ] Architect an integrated unit test execution framework (`--test`) to confirm pattern matching correctness.
- [ ] Handle sub-word boundary conditions and padding validation edge cases.

## Phase 3: Hardware Acceleration & Scale
- [ ] Integrate multi-threaded file slicing blocks (Pthreads / `std::async`).
- [ ] Explore AVX2 / AVX-512 SIMD vector instructions for parallel bit-mask comparisons.
- [ ] Develop a high-speed WebAssembly bridge compilation target (`deploy.sh`).
