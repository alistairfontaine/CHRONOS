#!/bin/bash

# Exit script immediately if any compilation or execution task fails
set -e

echo "=========================================================="
echo "CHRONOS MULTI-TARGET PRODUCTION PIPELINE"
echo "=========================================================="

echo -e "\n[Step 1/4] Compiling native CLI build using host optimizations..."
rm -f chronos-cli
g++ -O3 -std=c++17 -march=native src/main.cpp -I include -o chronos-cli -lpthread

echo -e "\n[Step 2/4] Triggering automated internal unit verifications..."
./chronos-cli --test

echo -e "\n[Step 3/4] Compiling high-velocity WebAssembly target distribution..."
# Maps out the functions so they can be easily driven by JS modules later
emcc src/main.cpp -o chronos.js \
  -O3 \
  -s EXPORTED_FUNCTIONS='["_main"]' \
  -s ALLOW_MEMORY_GROWTH=1 \
  -s ERROR_ON_UNDEFINED_SYMBOLS=0

echo -e "\n[Step 4/4] Staging production builds and synchronizing to GitHub..."
git add src/main.cpp include/ChronosCore.hpp docs/ROADMAP.md deploy.sh chronos.js chronos.wasm

# Check if there are active changes to commit before pushing
if ! git diff-index --quiet HEAD --; then
    git commit -m "pipeline: final production multi-target deployment (Native CLI + WASM modules)"
    git push origin main
else
    echo "No modifications detected. Codebase is already fully synchronized."
fi

echo -e "\n=========================================================="
echo "SUCCESS! CHRONOS IS SIGNED, SEALED, AND DEPLOYED LIVE."
echo "=========================================================="
