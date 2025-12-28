// Integer literal test file
// Comprehensive tests for all integer formats

// =============================================================================
// DECIMAL INTEGERS
// =============================================================================

// Single digits
0
1
2
3
4
5
6
7
8
9

// Two digits
10
42
99

// Multiple digits
100
999
1234
12345
123456
1234567
12345678
123456789
1234567890

// Large numbers
2147483647
9999999999

// =============================================================================
// INTEGERS WITH UNDERSCORES
// =============================================================================

// Standard separators (thousands)
1_000
10_000
100_000
1_000_000
10_000_000
100_000_000
1_000_000_000

// Binary-style grouping
1111_0000
0000_1111

// Arbitrary grouping
1_2_3_4
12_34_56
123_456_789

// Single underscore positions
1_0
10_0
100_0
1000_0

// Multiple underscores (adjacent - may be error)
1__0

// Leading underscore after digit
1_

// =============================================================================
// HEXADECIMAL INTEGERS
// =============================================================================

// Basic hex
0x0
0x1
0xA
0xF
0xa
0xf

// Two digit hex
0x00
0xFF
0xff
0xAB
0xab

// Four digit hex
0x0000
0xFFFF
0xDEAD
0xBEEF
0xdead
0xbeef

// Eight digit hex
0x00000000
0xFFFFFFFF
0xDEADBEEF
0xdeadbeef
0xDeAdBeEf

// Mixed case
0xAbCdEf
0xaBcDeF

// Hex with underscores
0xFF_FF
0xDEAD_BEEF
0x0000_0000
0xFF_FF_FF_FF
0x0_0_0_0

// Common hex values
0x10
0x20
0x40
0x80
0x100
0x200
0x400
0x800
0x1000

// =============================================================================
// BINARY INTEGERS
// =============================================================================

// Single bit
0b0
0b1

// Multiple bits
0b00
0b01
0b10
0b11

// Nibbles
0b0000
0b0001
0b0010
0b0011
0b0100
0b0101
0b0110
0b0111
0b1000
0b1001
0b1010
0b1011
0b1100
0b1101
0b1110
0b1111

// Bytes
0b00000000
0b11111111
0b10101010
0b01010101

// With underscores
0b1111_0000
0b0000_1111
0b1010_1010
0b0101_0101
0b1111_1111_0000_0000

// 16-bit values
0b0000000000000000
0b1111111111111111
0b1010101010101010

// =============================================================================
// OCTAL INTEGERS
// =============================================================================

// Basic octal
0o0
0o1
0o7
0o10
0o77
0o100
0o777

// Common permission values
0o644
0o755
0o777
0o400
0o600

// With underscores
0o755_644
0o777_000

// =============================================================================
// INTEGER EDGE CASES
// =============================================================================

// Zero in all bases
0
0x0
0b0
0o0

// Maximum values
2147483647
0x7FFFFFFF
0b1111111111111111111111111111111

// Leading zeros (decimal)
00
01
007
0123

// Integers in expressions
1 + 2
10 - 5
100 * 2
1000 / 10
42 % 7

// Integers as array indices
arr[0]
arr[1]
arr[42]
arr[0xFF]

// Integers in ranges
0..10
1..100
0..0xFF

// =============================================================================
// NEGATIVE INTEGERS (unary minus + literal)
// =============================================================================
-0
-1
-42
-1000
-2147483647
-0xFF
-0b1010
-0o755
