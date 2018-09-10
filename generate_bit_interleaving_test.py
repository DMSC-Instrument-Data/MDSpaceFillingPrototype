#!/usr/bin/env python3

# Generates expected interleaved integer (split into 64 bit integers) for a given set of input integers.
# For use in generating bit strings for unit test data
# Usage: generate_bit_interleaving_test.py N1 N2 [N3...]

import sys


integer_bit_strings = sys.argv[1:]
interleaved_bit_string = ''.join([''.join(x)[::-1] for x in zip(*integer_bit_strings)])
print(interleaved_bit_string)

for idx in range(0, len(interleaved_bit_string), 64):
    upper = min(idx + 64, len(interleaved_bit_string))
    print(idx, upper)
    print(interleaved_bit_string[idx:upper])
