#!/bin/bash
set -e

echo "=========================================================="
echo "CHRONOS NATIVE SYSTEM PRODUCTION PIPELINE"
echo "=========================================================="

echo -e "\n[Step 1/3] Compiling native CLI build using host optimizations..."
rm -f chronos-cli
g++ -O3 -std=c++17 -march=native src/main.cpp -I include -o chronos-cli -lpthread

echo -e "\n[Step 2/3] Triggering automated internal unit verifications..."
./chronos-cli --test

echo -e "\n[Step 3/3] Synchronizing codebase and structural documentation..."
git add src/main.cpp include/ChronosCore.hpp docs/ROADMAP.md README.md deploy.sh .gitignore .gitattributes tests/

if ! git diff-index --quiet HEAD --; then
    git commit -m "pipeline: enforce clean native environment, shift test frames to tests/"
    git push origin main
else
    echo "No modifications detected. Codebase is completely aligned."
fi

echo -e "\n=========================================================="
echo "SUCCESS! CHRONOS NATIVE CODEBASE DEPLOYED."
echo "=========================================================="
