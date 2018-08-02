#!/usr/bin/env python3

# Script to generate bit padding code for integer bit interleaving.
# Tidied up and stripped down version of the script listed here: https://stackoverflow.com/a/18528775

def generate_bit_spacing_code(bit_count, empty_bit_count):
    print('Pad {} bits with {} padding bits'.format(bit_count, empty_bit_count))

    bit_distances = [bin(i * empty_bit_count)[2:] for i in range(bit_count)]
    move_bits = []

    max_length = len(max(bit_distances, key=len))
    for i in range(max_length):
        move_bits.append([])
        for idx, bits in enumerate(bit_distances):
            if not len(bits) - 1 < i:
                if bits[len(bits) - i - 1] == '1':
                    move_bits[i].append(idx)

    bit_positions = list(range(bit_count))
    mask_old = (1 << bit_count) -1

    code_str = 'x &= {}\n'.format(hex(mask_old))
    for idx in range(len(move_bits) - 1, -1, -1):
        if move_bits[idx]:
            shifted = 0
            for bit_idx_to_move in move_bits[idx]:
                shifted |= 1 << bit_positions[bit_idx_to_move]
                bit_positions[bit_idx_to_move] += 2 ** idx

            non_shifted = ~shifted & mask_old
            shifted = shifted << 2 ** idx

            mask_new = shifted | non_shifted

            code_str += 'x = (x | x << {}) & {}\n'.format(2 ** idx, hex(mask_new))
            mask_old = mask_new

    return code_str


print(generate_bit_spacing_code(8, 1))

print(generate_bit_spacing_code(16, 1))
print(generate_bit_spacing_code(16, 2))
print(generate_bit_spacing_code(16, 3))

print(generate_bit_spacing_code(32, 1))
print(generate_bit_spacing_code(32, 2))
print(generate_bit_spacing_code(32, 3))

print(generate_bit_spacing_code(64, 3))
