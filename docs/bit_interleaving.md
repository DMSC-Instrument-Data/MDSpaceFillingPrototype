# Bit interleaving implementation

Two variants of bit interleaving were implemented; a for loop approach that
takes input as an Eigen array and a fixed bit masking approach which uses a
series of bit shift and mask operations to pad integer bits.

## For loop approach

This method iterates over each bit in the input/output (depending on if the
operation is interleave of deinterleave) and shifts bits to the correct position
based on iteration counters and number of input integers.

This method is easier to understand from the source code alone and does not
require prior computation of bit masks or writing of overly specific functions
as with the bit masking approach.

This method uses a calculation of where a bit should end up in the interleaved
integer based on the position of the bit in the input integer and the index of
the integer in the coordinate.

Take the example of four 4 bit integers interleaved into a 16 bit integer, where
`B` are the bit indices of the interleaved integer, `b` are the bit indices of
the input integers and `i` is the index of the coordinate integer (e.g.
for coordinate x `i = 0`, for y `i = 1`, and so on):

```
B  15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
i   3  2  1  0  3  2  1  0  3  2  1  0  3  2  1  0
b   3  3  3  3  2  2  2  2  1  1  1  1  0  0  0  0
```

The position `B` of a bit `b` in input integer at index `i` where the point has
`N` coordinates can be determined by `B = iN + b`.

This method is easiest explained alongside the code, and is done so in comments
in the file `BitInterleavingEigen.h`.

## Bit masking approach

The general overview of this approach is as follows:
  - Pad each input integer such that each bit of the input integer has N - 1
    padded bits between each bt of the intege (i.e. for a 8 bit integer holding
    `00000011`, in 2 dimensions (N), we have a 16 bit integer,
    `0000000000000101`)
  - Shift each padded integer by the index of the integer in the coordinate
    (i.e. the above becomes `0000000000001010` if it refers to the y dimension)
  - Bitwise and the shifted, padded integers

This method is notably faster than the for loop approach.

(For me at least) this is most clear with a worked example. Note that a `.`
appearing in the binary representation of an integer indicates we do not care
about the bit in this position.

### Interleaving

Take the two inputs `a` and `b`, both 8 bit integers to be interleaved into a
single 16 bit integer.

```
a = 0b10110010
b = 0b00010100
```

As the input integers will be shifted beyond their input width, they are passed
to the interleaving functions as the interleaved type. This makes it possible to
pass an integer of greater bit width than the input type, for this reason it is
desirable to mask the inputs to their designed input width.

(this can be removed if correct integer width can be guaranteed)

```
Pad a - mask to input width
0b........10110010    a
0b........10110010    a & 0b0000000011111111
```

The integer is then shifted and masked several times (depending on the input
width and number of padding bits required).

Each shift operation redistributes some bits of the input integer and the mask
is used to remove the bits (both residual from the input of the previous
iteration and resulting from the shift operation) that are in no longer needed
positions.

```
Pad a - iteration 1
0b........10110010    a
0b....10110010....    a << 4
0b....101110110010    a | a << 4
0b....1011....0010   (a | a << 4) & 0b0000111100001111

Pad a - iteration 2
0b....1011....0010    a
0b..1011....0010..    a << 2
0b..101111..001010    a | a << 2
0b..10..11..00..10   (a | a << 2) & 0b0011001100110011

Pad a - iteration 3
0b..10..11..00..10    a
0b.10..11..00..10.    a << 1
0b.110.111.000.100    a | a << 1
0b.1.0.1.1.0.0.1.0   (a | a << 1) & 0b0101010101010101
```

This is performed for all input integers.

```
Pad b - mask to input width
0b........00010100    b
0b........00010100    b & 0b0000000011111111

Pad b - iteration 1
0b........00010100    b
0b....00010100....    b << 4
0b....000101010100    b | b << 4
0b....0001....0100   (b | b << 4) & 0b0000111100001111

Pad b - iteration 2
0b....0001....0100    b
0b..0001....0100..    b << 2
0b..000101..010100    b | b << 2
0b..00..01..01..00   (b | b << 2) & 0b0011001100110011

Pad b - iteration 3
0b..00..01..01..00    b
0b.00..01..01..00.    b << 1
0b.000.011.011.000    b | b << 1
0b.0.0.0.1.0.1.0.0   (b | b << 1) & 0b0101010101010101
```

The two integers are then shifted by a fixed offset depending on the axis they
represent (e.g. shift for x is 0, shift for y is 1, shift for z is 2, etc.). At
this point there should be no integer bits overlapping.

```
0b.1.0.1.1.0.0.1.0    a << 0
0b0.0.0.1.0.1.0.0.    b << 1
```

Finally the integers are ORd to give the interleaved integer.

```
0b.1.0.1.1.0.0.1.0    a
0b0.0.0.1.0.1.0.0.    b
0b0100011100100100    a | b = z
```

This implementation is specific to the input integer width, interleaved (output)
integer width and number of input integers.

Bit masks can be generated manually by working through the desired case by hand
(as above), however for convenience a script (`bit_padding_generator.py`) has
been included that generates the padding code from the input bit length and
number of padding bits required.

### Deinterleaving

This is essentially the same as interleaving but in reverse.

Padding functions can be converted into compacting (i.e. removal of padding
bits) functions by reversing the bit masks, bit shift amounts and bit shift
directions.

Take the interleaved 16 bit integer `z` that was interleaved from two 8 bit
integers `a` and `b` (as per the example of interleaving above):

```
z = 0b0100011100100100
```

The first step is to undo the final bit shift performed by the interleaving
process:

```
0b0100011100100100    z
0b0100011100100100    z >> 0 = a
0b0010001110010010    z >> 1 = b
```

Take integer `a`.

We must mask the integer such the padding bits are ignored, the mask used here
is the same as the final mask used in interleaving.

```
Select integer bits
0b0100011100100100    a
0b.1.0.1.1.0.0.1.0   (a | a << 1) & 0b0101010101010101
```

We can then start the reverse of the interleaving shift and mask operations.

```
Compact a - iteration 1
0b.1.0.1.1.0.0.1.0    a
0b..1.0.1.1.0.0.1.    a >> 1
0b.110011110000110    a | a >> 1
0b..10..11..00..10   (a | a >> 1) & 0b0011001100110011

Compact a - iteration 2
0b..10..11..00..10    a
0b....10..11..00..    a >> 2
0b..10101111000010    a | a >> 2
0b....1011....0010   (a | a >> 2) & 0b0000111100001111

Compact a - iteration 3
0b....1011....0010    a
0b........1011....    a >> 4
0b....101110110010    a | a >> 4
0b........10110010   (a | a >> 4) & 0b0000000011111111
```

The original value of `a` has now been retrieved.

```
a = 0b10110010
```

For completeness here is the same for `b`:

```
Select integer bits
0b0010001110010010    b
0b.0.0.0.1.0.1.0.0   (b | b << 1) & 0b0101010101010101

Compact b - iteration 1
0b.0.0.0.1.0.1.0.0    b
0b..0.0.0.1.0.1.0.    b >> 1
0b.000000110011000    b | b >> 1
0b..00..01..01..00   (b | b >> 1) & 0b0011001100110011

Compact b - iteration 2
0b..00..01..01..00    b
0b....00..01..01..    b >> 2
0b..00000101010100    b | b >> 2
0b....0001....0100   (b | b >> 2) & 0b0000111100001111

Compact b - iteration 3
0b....0001....0100    b
0b........0001....    b >> 4
0b....000100010100    b | b >> 4
0b........00010100   (b | b >> 4) & 0b0000000011111111

b = 0b00010100
```

## 128 bit versions

There are two 128 bit versions included; one with calculated bit masks that uses
a `boost::multiprecision::uint128_t` and one that uses two `uint64_t` to
represent the MSB and LSB of the 128 bit interleaved integer.

These implementations are mostly identical to the methods already described
above.
