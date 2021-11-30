enum AddressingMode
{
    IMMEDIATE,
    ZERO_PAGE,
    ZERO_PAGE_X,
    ZERO_PAGE_Y,
    ABSOLUTE,
    ABSOLUTE_X,
    ABSOLUTE_Y,
    INDIRECT,
    INDEXED_INDIRECT, // Indirect X
    INDIRECT_INDEXED, // Indirect Y
    ACCUMULATOR
};