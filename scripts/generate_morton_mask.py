#!/usr/bin/env python3


def generate(nd, integer_bits, shift):
    bits = nd * integer_bits
    output = 0
    for i in range(bits):
        if i % nd == shift:
            output |= 1 << i;
    return output


def run(nd, integer_bits):
    print(nd, integer_bits)
    for i in range(nd):
        mask = generate(nd, integer_bits, i)
        print(mask, hex(mask), bin(mask))


run(2, 16)
run(3, 16)
run(4, 8)
run(4, 16)
