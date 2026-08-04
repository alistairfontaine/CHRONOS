#ifndef CHRONOS_CORE_HPP
#define CHRONOS_CORE_HPP

#include <vector>
#include <string>
#include <cstdint>
#include <stdexcept>

namespace Chronos {

    inline uint8_t encode_nucleotide(char base) {
        switch (base) {
            case 'A': case 'a': return 0x00;
            case 'C': case 'c': return 0x01;
            case 'G': case 'g': return 0x02;
            case 'T': case 't': return 0x03;
            default:
                throw std::invalid_argument("Invalid nucleotide character encountered.");
        }
    }

    struct alignas(8) GenomicWord {
        uint64_t data = 0;
        uint8_t length = 0; // Max 32 bases
    };

    class SearchMatrix {
    private:
        std::vector<GenomicWord> sequenceWords;
        size_t totalBasesParsed = 0;

    public:
        SearchMatrix() = default;

        void import_raw_sequence(const std::string& sequence) {
            if (sequence.empty()) return;

            GenomicWord currentWord;
            int bitShift = 0;

            if (!sequenceWords.empty() && sequenceWords.back().length < 32) {
                currentWord = sequenceWords.back();
                sequenceWords.pop_back();
                bitShift = currentWord.length * 2;
            }

            for (char base : sequence) {
                if (base == ' ' || base == '\n' || base == '\r' || base == '\t') continue;

                uint64_t encodedBits = encode_nucleotide(base);
                currentWord.data |= (encodedBits << bitShift);
                bitShift += 2;
                currentWord.length++;
                totalBasesParsed++;

                if (currentWord.length == 32) {
                    sequenceWords.push_back(currentWord);
                    currentWord.data = 0;
                    currentWord.length = 0;
                    bitShift = 0;
                }
            }

            if (currentWord.length > 0) {
                sequenceWords.push_back(currentWord);
            }
        }

        // High-velocity bit-shifting sliding matcher
        std::vector<size_t> find_exact_matches(const std::string& query) const {
            std::vector<size_t> indicesFound;
            if (query.empty() || totalBasesParsed < query.length()) return indicesFound;

            // 1. Pack the search target query down into a temporary 64-bit comparison register
            uint64_t queryMask = 0;
            uint64_t queryData = 0;
            for (size_t i = 0; i < query.length(); ++i) {
                uint64_t encoded = encode_nucleotide(query[i]);
                queryData |= (encoded << (i * 2));
                queryMask |= (0x03ULL << (i * 2));
            }

            size_t queryLen = query.length();

            // 2. Iterate bitwise down through our packed reference word buffer array
            for (size_t i = 0; i <= totalBasesParsed - queryLen; ++i) {
                size_t wordIdx = i / 32;
                size_t subBitOffset = (i % 32) * 2;

                if (wordIdx >= sequenceWords.size()) break;

                uint64_t combinedWindow = 0;

                // If query windows overlap across two separate 64-bit memory chunks, stitch them together
                if (subBitOffset + (queryLen * 2) <= 64) {
                    combinedWindow = sequenceWords[wordIdx].data >> subBitOffset;
                } else {
                    uint64_t firstPart = sequenceWords[wordIdx].data >> subBitOffset;
                    uint64_t secondPart = 0;
                    if (wordIdx + 1 < sequenceWords.size()) {
                        secondPart = sequenceWords[wordIdx + 1].data << (64 - subBitOffset);
                    }
                    combinedWindow = firstPart | secondPart;
                }

                // Mask out unneeded remnants and do a fast hardware-level bit equality check
                if ((combinedWindow & queryMask) == queryData) {
                    indicesFound.push_back(i);
                }
            }

            return indicesFound;
        }

        void clear() {
            sequenceWords.clear();
            totalBasesParsed = 0;
        }

        size_t get_word_count() const { return sequenceWords.size(); }
        size_t get_total_bases() const { return totalBasesParsed; }
    };
}

#endif // CHRONOS_CORE_HPP
