// This code is derived work from https://github.com/kpmiller/emulator101/blob/master/8080emu-first50.c

#include <string>


using byte = std::uint8_t;
using word = std::uint16_t;


const char* hex_byte[]
{
    "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0A", "0B", "0C", "0D", "0E", "0F",
    "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1A", "1B", "1C", "1D", "1E", "1F",
    "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "2A", "2B", "2C", "2D", "2E", "2F",
    "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "3A", "3B", "3C", "3D", "3E", "3F",
    "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "4A", "4B", "4C", "4D", "4E", "4F",
    "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "5A", "5B", "5C", "5D", "5E", "5F",
    "60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "6A", "6B", "6C", "6D", "6E", "6F",
    "70", "71", "72", "73", "74", "75", "76", "77", "78", "79", "7A", "7B", "7C", "7D", "7E", "7F",
    "80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "8A", "8B", "8C", "8D", "8E", "8F",
    "90", "91", "92", "93", "94", "95", "96", "97", "98", "99", "9A", "9B", "9C", "9D", "9E", "9F",
    "A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9", "AA", "AB", "AC", "AD", "AE", "AF",
    "B0", "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8", "B9", "BA", "BB", "BC", "BD", "BE", "BF",
    "C0", "C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8", "C9", "CA", "CB", "CC", "CD", "CE", "CF",
    "D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7", "D8", "D9", "DA", "DB", "DC", "DD", "DE", "DF",
    "E0", "E1", "E2", "E3", "E4", "E5", "E6", "E7", "E8", "E9", "EA", "EB", "EC", "ED", "EE", "EF",
    "F0", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "FA", "FB", "FC", "FD", "FE", "FF",
};


const char* mnemonic[] {
    "NOP",    "LXI  B,",  "STAX B",  "INX  B",  "INR  B", "DCR  B", "MVI  B,", "RLC", "NOP_08", "DAD  B",  "LDAX B", "DCX  B",  "INR  C", "DCR  C", "MVI  C,", "RRC",
    "NOP_10", "LXI  D,", " STAX D",  "INX  D",  "INR  D", "DCR  D", "MVI  D,", "RAL", "NOP_18", "DAD  D",  "LDAX D", "DCX  D",  "INR  E", "DCR  E", "MVI  E,", "RAR",
    "NOP_20", "LXI  H,",  "SHLD ",   "INX  H",  "INR  H", "DCR  H", "MVI  H,", "DAA", "NOP_28", "DAD  H",  "LHLD ",  "DCX  H",  "INR  L", "DCR  L", "MVI  L,", "CMA",
    "NOP_30", "LXI  SP,", "STA  ",   "INX  SP", "INR  M", "DCR  M", "MVI  M,", "STC", "NOP_38", "DAD  SP", "LDA  ",  "DCX  SP", "INR  A", "DCR  A", "MVI  A,", "CMC",

    "MOV  B,B", "MOV  B,C", "MOV  B,D", "MOV  B,E", "MOV  B,H", "MOV  B,L", "MOV  B,M", "MOV  B,A", "MOV  C,B", "MOV  C,C", "MOV  C,D", "MOV  C,E", "MOV  C,H", "MOV  C,L", "MOV  C,M", "MOV  C,A",
    "MOV  D,B", "MOV  D,C", "MOV  D,D", "MOV  D,E", "MOV  D,H", "MOV  D,L", "MOV  D,M", "MOV  D,A", "MOV  E,B", "MOV  E,C", "MOV  E,D", "MOV  E,E", "MOV  E,H", "MOV  E,L", "MOV  E,M", "MOV  E,A",
    "MOV  H,B", "MOV  H,C", "MOV  H,D", "MOV  H,E", "MOV  H,H", "MOV  H,L", "MOV  H,M", "MOV  H,A", "MOV  L,B", "MOV  L,C", "MOV  L,D", "MOV  L,E", "MOV  L,H", "MOV  L,L", "MOV  L,M", "MOV  L,A",
    "MOV  M,B", "MOV  M,C", "MOV  M,D", "MOV  M,E", "MOV  M,H", "MOV  M,L", "HLT",      "MOV  M,A", "MOV  A,B", "MOV  A,C", "MOV  A,D", "MOV  A,E", "MOV  A,H", "MOV  A,L", "MOV  A,M", "MOV  A,A",
    
    "ADD  B", "ADD  C", "ADD  D", "ADD  E", "ADD  H", "ADD  L", "ADD  M", "ADD  A", "ADC  B", "ADC  C", "ADC  D", "ADC  E", "ADC  H", "ADC  L", "ADC  M", "ADC  A",
    "SUB  B", "SUB  C", "SUB  D", "SUB  E", "SUB  H", "SUB  L", "SUB  M", "SUB  A", "SBB  B", "SBB  C", "SBB  D", "SBB  E", "SBB  H", "SBB  L", "SBB  M", "SBB  A",
    "ANA  B", "ANA  C", "ANA  D", "ANA  E", "ANA  H", "ANA  L", "ANA  M", "ANA  A", "XRA  B", "XRA  C", "XRA  D", "XRA  E", "XRA  H", "XRA  L", "XRA  M", "XRA  A",
    "ORA  B", "ORA  C", "ORA  D", "ORA  E", "ORA  H", "ORA  L", "ORA  M", "ORA  A", "CMP  B", "CMP  C", "CMP  D", "CMP  E", "CMP  H", "CMP  L", "CMP  M", "CMP  A",
    
    "RNZ", "POP  B",   "JNZ  ",  "JMP  ", "CNZ  ", "PUSH B",   "ADI  ", "RST  0", "RZ",  "RET",    "JZ   ", "NOP_CB", "CZ   ", "CALL ",  "ACI  ", "RST  1",
    "RNC", "POP  D",   "JNC  ",  "OUT  ", "CNC  ", "PUSH D",   "SUI  ", "RST  2", "RC",  "NOP_D9", "JC   ", "IN   ",  "CC   ", "NOP_DD", "SBI  ", "RST  3",
    "RPO", "POP  H",   "JPO  ",  "XTHL",  "CPO  ", "PUSH H",   "ANI  ", "RST  4", "RPE", "PCHL",   "JPE  ", "XCHG",   "CPE  ", "NOP_ED", "XRI  ", "RST  5",
    "RP",  "POP  PSW", "JP   ",  "DI",    "CP   ", "PUSH PSW", "ORI  ", "RST  6", "RM",  "SPHL",   "JM   ", "EI",     "CM   ", "NOP_FD", "CPI  ", "RST  7"
};

static_assert(sizeof(mnemonic) / sizeof(mnemonic[0]) == 256, "Opcode array size mismatch");



int states[] {
    4, 10,  7,  5,   5,  5,  7,  4,     4, 10,  7,  5,   5,  5,  7,  4,
    4, 10,  7,  5,   5,  5,  7,  4,     4, 10,  7,  5,   5,  5,  7,  4,
    4, 10, 16,  5,   5,  5,  7,  4,     4, 10, 16,  5,   5,  5,  7,  4,
    4, 10, 13,  5,  10, 10, 10,  4,     4, 10, 13,  5,   5,  5,  7,  4,

    5,  5,  5,  5,   5,  5,  7,  5,     5,  5,  5,  5,   5,  5,  7,  5,
    5,  5,  5,  5,   5,  5,  7,  5,     5,  5,  5,  5,   5,  5,  7,  5,
    5,  5,  5,  5,   5,  5,  7,  5,     5,  5,  5,  5,   5,  5,  7,  5,
    7,  7,  7,  7,   7,  7,  7,  7,     5,  5,  5,  5,   5,  5,  5,  5,

    4,  4,  4,  4,   4,  4,  7,  4,     4,  4,  4,  4,   4,  4,  7,  4,
    4,  4,  4,  4,   4,  4,  7,  4,     4,  4,  4,  4,   4,  4,  7,  4,
    4,  4,  4,  4,   4,  4,  7,  4,     4,  4,  4,  4,   4,  4,  7,  4,
    4,  4,  4,  4,   4,  4,  7,  4,     4,  4,  4,  4,   4,  4,  7,  4,

    5, 10, 10, 10,  11, 11,  7, 11,     5, 10, 10,  4,  11, 17,  7, 11,
    5, 10, 10, 10,  11, 11,  7, 11,     5,  4, 10, 10,  11,  4,  7, 11,
    5, 10, 10, 18,  11, 11,  7, 11,     5,  5, 10,  4,  11,  4,  7, 11,
    5, 10, 10,  4,  11, 11,  7, 11,     5,  5, 10,  4,  11,  4,  7, 11,
};

static_assert(sizeof(states) / sizeof(states[0]) == 256, "States size array size mismatch");


int opcode_arg_size[] {
    0, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
    0, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
    0, 2, 2, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 1, 0,
    0, 2, 2, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 1, 0,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 2, 2, 2, 0, 1, 0, 0, 0, 2, 0, 2, 2, 1, 0,
    0, 0, 2, 1, 2, 0, 1, 0, 0, 0, 2, 1, 2, 0, 1, 0,
    0, 0, 2, 0, 2, 0, 1, 0, 0, 0, 2, 0, 2, 0, 1, 0,
    0, 0, 2, 0, 2, 0, 1, 0, 0, 0, 2, 0, 2, 0, 1, 0
};

static_assert(sizeof(opcode_arg_size) / sizeof(opcode_arg_size[0]) == 256, "Opcode arg size array size mismatch");


bool parity[] {
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,

    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,

    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,

    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
};


byte key_code[] {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x10, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x00, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};


class UT88
{

public:
    byte b;
    byte c;
    byte d;
    byte e;
    byte h;
    byte l;
    byte a;

    word sp;
    word pc;

    bool s;
    bool z;
    bool ac;
    bool p;
    bool cy;

    bool ei;

    absolute_time_t time_of_reset;
    int num_states = 0;
    int states_since_last_second;

    struct rom
    {
        const byte bytes[1024]
        {
#include "ut88_rom.h"
        };

        enum { start = 0, end = start + sizeof bytes };
    } rom;

    
    struct mmio_display
    {
        uint8_t bytes[3];
        enum { start = 0x9000, end = start + sizeof bytes };
    } mmio_display;

    bool redraw;

    struct ram
    {
        uint8_t bytes[1024];
        enum { start = 0xC000, end = start + sizeof bytes };
    } ram;

    byte opcode;

    byte kb_byte;
    bool reset_requested;

    byte psw()
    {
        return (s << 7) | (z << 6) | (ac << 4) | (p << 2) | 0x02 | cy;
    }

    void psw(byte psw)
    {
        s = (psw & 0x80) != 0;
        z = (psw & 0x40) != 0;
        ac = (psw & 0x10) != 0;
        p = (psw & 0x04) != 0;
        cy = (psw & 0x01) != 0;
    }

    void set_szp(byte& bb)
    {
        s = (bb & 0x80) != 0;
        z = bb == 0;
        p = parity[bb];
    }

    bool tick()
    {
        if (reset_requested)
        {
            reset();
            reset_requested = false;
        }

        word bc = (b << 8) | c;
        word de = (d << 8) | e;
        word hl = (h << 8) | l;

        opcode = read_mem(pc);
        byte arg1 = read_mem(pc + 1);
        byte arg2 = read_mem(pc + 2);
        word addr = (arg2 << 8) | arg1;

        word prev_pc = pc;
        pc += opcode_arg_size[opcode] + 1;

        switch (opcode)
        {
            case 0x00: break; // NOP
            case 0x01: c = arg1; b = arg2; break; // LXI B, word
            case 0x02: write_mem(bc, a); break; // STAX B
            case 0x03: ++bc; b = bc >> 8; c = bc & 0xFF; break; // INX B
            case 0x04: // INR B
                {
                    ++b;
                    set_szp(b);
                    ac = (b & 0x0F) == 0x00;
                }
                break;
            case 0x05: // DCR B
                {
                    --b;
                    set_szp(b);
                    ac = (b & 0x0F) == 0x0F;
                }
                break;
            case 0x06: b = arg1; break; // MVI B, byte
            case 0x07: cy = a >> 7; a = (a << 1) | (a >> 7); break; // RLC
            case 0x08: break; // NOP_08
            case 0x0A: a = read_mem(bc); break; // LDAX B
            case 0x0B: --bc; b = bc >> 8; c = bc & 0xFF; break; // DCX B
            case 0x0C: // INR C
                {
                    ++c;
                    set_szp(c);
                    ac = (c & 0x0F) == 0x00;
                }
                break;
            case 0x0D: // DCR C
                {
                    --c;
                    set_szp(c);
                    ac = (c & 0x0F) == 0x0F;
                }
                break;
            case 0x0E: c = arg1; break; // MVI C, byte
            case 0x10: break; // NOP_10
            case 0x11: e = arg1; d = arg2; break; // LXI D, word
            case 0x12: write_mem(de, a); break; // STAX D
            case 0x13: ++de; d = de >> 8; e = de & 0xFF; break; // INX D
            case 0x14: // INR D
                {
                    ++d;
                    set_szp(d);
                    ac = (d & 0x0F) == 0x00;
                }
                break;
            case 0x15: // DCR D
                {
                    --d;
                    set_szp(d);
                    ac = (d & 0x0F) == 0x0F;
                }
                break;
            case 0x16: d = arg1; break; // MVI D, byte
            case 0x18: break; // NOP_18
            case 0x1A: a = read_mem(de); break; // LDAX D
            case 0x1B: --de; d = de >> 8; e = de & 0xFF; break; // DCX D
            case 0x1C: // INR E
                {
                    ++e;
                    set_szp(e);
                    ac = (e & 0x0F) == 0x00;
                }
                break;
            case 0x1D: // DCR E
                {
                    --e;
                    set_szp(e);
                    ac = (e & 0x0F) == 0x0F;
                }
                break;
            case 0x1E: e = arg1; break; // MVI E, byte
            case 0x20: break; // NOP_20
            case 0x21: l = arg1; h = arg2; break; // LXI H, word
            case 0x22: write_mem(addr, l); write_mem(addr + 1, h); break; // SHLD addr
            case 0x23: ++hl; h = hl >> 8; l = hl & 0xFF; break; // INX H
            case 0x24: // INR H
                {
                    ++h;
                    set_szp(h);
                    ac = (h & 0x0F) == 0x00;
                }
                break;
            case 0x25: // DCR H
                {
                    --h;
                    set_szp(h);
                    ac = (h & 0x0F) == 0x0F;
                }
                break;
            case 0x26: h = arg1; break; // MVI H, byte
            // 0x27 DAA  (fully corrected)
            case 0x27: {
                byte adj = 0;
                bool carry = cy;
                bool half  = ac;

                if ((a & 0x0F) > 9 || half)       adj += 0x06;
                if (a > 0x99 || carry) { adj |= 0x60; carry = true; }

                word r = word(a) + adj;
                ac = ((a & 0x0F) + (adj & 0x0F)) > 0x0F;
                a = byte(r);
                cy = carry || (r & 0x100);

                set_szp(a);
            } break;
            case 0x28: break; // NOP_28
            case 0x2A: l = read_mem(addr); h = read_mem(addr + 1); break; // LHLD addr
            case 0x2B: --hl; h = hl >> 8; l = hl & 0xFF; break; // DCX H
            case 0x2C: // INR L
                {
                    ++l;
                    set_szp(l);
                    ac = (l & 0x0F) == 0x00;
                }
                break;
            case 0x2D: // DCR L
                {
                    --l;
                    set_szp(l);
                    ac = (l & 0x0F) == 0x0F;
                }
                break;
            case 0x2E: l = arg1; break; // MVI L, byte
            case 0x2F: a = ~a; break; // CMA
            case 0x30: break; // NOP_30
            case 0x31: sp = addr; break; // LXI SP, word
            case 0x32: write_mem(addr, a); break; // STA addr
            case 0x33: ++sp; break; // INX SP
            case 0x34: // INR M
                {
                    byte mem = read_mem(hl);
                    ++mem;
                    set_szp(mem);
                    ac = (mem & 0x0F) == 0x00;
                    write_mem(hl, mem);
                }
                break;
            case 0x35: // DCR M
                {
                    byte mem = read_mem(hl);
                    --mem;
                    set_szp(mem);
                    ac = (mem & 0x0F) == 0x0F;
                    write_mem(hl, mem);
                }
                break;
            case 0x36: write_mem(hl, arg1); break; // MVI M, byte
            case 0x37: cy = true; break; // STC
            case 0x38: break; // NOP_38
            case 0x3A: a = read_mem(addr); break; // LDA addr
            case 0x3B: --sp; break; // DCX SP
            case 0x3C: // INR A
                {
                    ++a;
                    set_szp(a);
                    ac = (a & 0x0F) == 0x00;
                }
                break;
            case 0x3D: // DCR A
                {
                    --a;
                    set_szp(a);
                    ac = (a & 0x0F) == 0x0F;
                }
                break;
            case 0x3E: a = arg1; break; // MVI A, byte
            case 0x3F: cy = !cy; break; // CMC
            case 0x40: break; // MOV B, B
            case 0x41: b = c; break; // MOV B, C
            case 0x42: b = d; break; // MOV B, D
            case 0x43: b = e; break; // MOV B, E
            case 0x44: b = h; break; // MOV B, H
            case 0x45: b = l; break; // MOV B, L
            case 0x46: b = read_mem(hl); break; // MOV B, M
            case 0x47: b = a; break; // MOV B, A
            case 0x48: c = b; break; // MOV C, B
            case 0x49: break; // MOV C, C
            case 0x4A: c = d; break; // MOV C, D
            case 0x4B: c = e; break; // MOV C, E
            case 0x4C: c = h; break; // MOV C, H
            case 0x4D: c = l; break; // MOV C, L
            case 0x4E: c = read_mem(hl); break; // MOV C, M
            case 0x4F: c = a; break; // MOV C, A
            case 0x50: d = b; break; // MOV D, B
            case 0x51: d = c; break; // MOV D, C
            case 0x52: break; // MOV D, D
            case 0x53: d = e; break; // MOV D, E
            case 0x54: d = h; break; // MOV D, H
            case 0x55: d = l; break; // MOV D, L
            case 0x56: d = read_mem(hl); break; // MOV D, M
            case 0x57: d = a; break; // MOV D, A
            case 0x58: e = b; break; // MOV E, B
            case 0x59: e = c; break; // MOV E, C
            case 0x5A: e = d; break; // MOV E, D
            case 0x5B: break; // MOV E, E
            case 0x5C: e = h; break; // MOV E, H
            case 0x5D: e = l; break; // MOV E, L
            case 0x5E: e = read_mem(hl); break; // MOV E, M
            case 0x5F: e = a; break; // MOV E, A
            case 0x60: h = b; break; // MOV H, B
            case 0x61: h = c; break; // MOV H, C
            case 0x62: h = d; break; // MOV H, D
            case 0x63: h = e; break; // MOV H, E
            case 0x64: break; // MOV H, H
            case 0x65: h = l; break; // MOV H, L
            case 0x66: h = read_mem(hl); break; // MOV H, M
            case 0x67: h = a; break; // MOV H, A
            case 0x68: l = b; break; // MOV L, B
            case 0x69: l = c; break; // MOV L, C
            case 0x6A: l = d; break; // MOV L, D
            case 0x6B: l = e; break; // MOV L, E
            case 0x6C: l = h; break; // MOV L, H
            case 0x6D: break; // MOV L, L
            case 0x6E: l = read_mem(hl); break; // MOV L, M
            case 0x6F: l = a; break; // MOV L, A
            case 0x70: write_mem(hl, b); break; // MOV M, B
            case 0x71: write_mem(hl, c); break; // MOV M, C
            case 0x72: write_mem(hl, d); break; // MOV M, D
            case 0x73: write_mem(hl, e); break; // MOV M, E
            case 0x74: write_mem(hl, h); break; // MOV M, H
            case 0x75: write_mem(hl, l); break; // MOV M, L
            case 0x76: return false; // HLT
            case 0x77: write_mem(hl, a); break; // MOV M, A
            case 0x78: a = b; break; // MOV A, B
            case 0x79: a = c; break; // MOV A, C
            case 0x7A: a = d; break; // MOV A, D
            case 0x7B: a = e; break; // MOV A, E
            case 0x7C: a = h; break; // MOV A, H
            case 0x7D: a = l; break; // MOV A, L
            case 0x7E: a = read_mem(hl); break; // MOV A, M
            case 0x7F: break; // MOV A, A

            case 0xA8: a ^= b; set_szp(a); ac = false; cy = false; break; // XRA B
            case 0xA9: a ^= c; set_szp(a); ac = false; cy = false; break; // XRA C
            case 0xAA: a ^= d; set_szp(a); ac = false; cy = false; break; // XRA D
            case 0xAB: a ^= e; set_szp(a); ac = false; cy = false; break; // XRA E
            case 0xAC: a ^= h; set_szp(a); ac = false; cy = false; break; // XRA H
            case 0xAD: a ^= l; set_szp(a); ac = false; cy = false; break; // XRA L
            case 0xAE: a ^= read_mem(hl); set_szp(a); ac = false; cy = false; break; // XRA M
            case 0xAF: a ^= a; set_szp(a); ac = false; cy = false; break; // XRA A
            case 0xB0: a |= b; set_szp(a); ac = false; cy = false; break; // ORA B
            case 0xB1: a |= c; set_szp(a); ac = false; cy = false; break; // ORA C
            case 0xB2: a |= d; set_szp(a); ac = false; cy = false; break; // ORA D
            case 0xB3: a |= e; set_szp(a); ac = false; cy = false; break; // ORA E
            case 0xB4: a |= h; set_szp(a); ac = false; cy = false; break; // ORA H
            case 0xB5: a |= l; set_szp(a); ac = false; cy = false; break; // ORA L
            case 0xB6: a |= read_mem(hl); set_szp(a); ac = false; cy = false; break; // ORA M
            case 0xB7: set_szp(a); ac = false; cy = false; break; // ORA A

            // 0xBE CMP M  (was mislabeled/implemented as CPI)
            case 0xBE: {
                byte m = read_mem(hl);
                ac = (a & 0x0F) < (m & 0x0F);
                byte res = a - m;
                set_szp(res);
                cy = a < m;
            } break;

            case 0xC0: if (!z) { pc = read_mem(sp++); pc |= read_mem(sp++) << 8; num_states += 6; } break; // RNZ addr
            case 0xC1: c = read_mem(sp++); b = read_mem(sp++); break; // POP B
            case 0xC2: if (!z) pc = addr; break; // JNZ addr
            case 0xC3: pc = addr; break; // JMP addr
            case 0xC4: if (!z) { write_mem(--sp, pc >> 8); write_mem(--sp, pc & 0xFF); pc = addr; num_states += 6; } break; // CNZ addr
            case 0xC5: write_mem(--sp, b); write_mem(--sp, c); break; // PUSH B
            case 0xC6: // ADI byte
                {
                    ac = (a & 0x0F) + (arg1 & 0x0F) >= 0x10;
                    word x = word(a) + word(arg1);
                    a = byte(x);
                    set_szp(a);
                    cy = (x & 0x100) != 0;
                }
                break;
            case 0xC7: write_mem(--sp, pc >> 8); write_mem(--sp, pc & 0xFF); pc = 0x00; break; // RST 0
            case 0xC8: if (z) { pc = read_mem(sp++); pc |= read_mem(sp++) << 8; num_states += 6; } break; // RZ addr
            case 0xC9: pc = read_mem(sp++); pc |= read_mem(sp++) << 8; break; // RET
            case 0xCA: if (z) pc = addr; break; // JZ addr
            case 0xCB: break; // NOP_CB
            case 0xCC: if (z) { write_mem(--sp, pc >> 8); write_mem(--sp, pc & 0xFF); pc = addr; num_states += 6; } break; // CZ addr
            case 0xCD: write_mem(--sp, pc >> 8); write_mem(--sp, pc & 0xFF); pc = addr; break; // CALL addr
            // 0xCE ACI  (fix AC)
            case 0xCE: {
                word cin = cy ? 1 : 0;
                ac = ((a & 0x0F) + (arg1 & 0x0F) + cin) > 0x0F;
                word x = word(a) + word(arg1) + cin;
                a = byte(x);
                set_szp(a);
                cy = (x & 0x100) != 0;
            } break;
            case 0xCF: write_mem(--sp, pc >> 8); write_mem(--sp, pc & 0xFF); pc = 0x08; break; // RST 1
            case 0xD0: if (!c) { pc = read_mem(sp++); pc |= read_mem(sp++) << 8; num_states += 6; } break; // RNC
            case 0xD1: e = read_mem(sp++); d = read_mem(sp++); break; // POP D
            case 0xD2: if (!c) pc = addr; break; // JNC addr
            case 0xD3: write_io(arg1, a); break; // OUT port
            case 0xD4: if (!c) { write_mem(--sp, pc >> 8); write_mem(--sp, pc & 0xFF); pc = addr; num_states += 6; } break; // CNC addr
            case 0xD5: write_mem(--sp, d); write_mem(--sp, e); break; // PUSH B
            case 0xD6: // SUI byte
                {
                    ac = (a & 0x0F) < (arg1 & 0x0F);
                    cy = a < arg1;
                    a -= arg1;
                    set_szp(a);
                }
                break;
            case 0xD7: write_mem(--sp, pc >> 8); write_mem(--sp, pc & 0xFF); pc = 0x10; break; // RST 2
            case 0xD8: if (c) { pc = read_mem(sp++); pc |= read_mem(sp++) << 8; num_states += 6; } break; // RC
            case 0xD9: break; // NOP_D9
            case 0xDA: if (c) pc = addr; break; // JC addr
            case 0xDB: a = read_io(arg1); break; // IN port
            case 0xDC: if (c) { write_mem(--sp, pc >> 8); write_mem(--sp, pc & 0xFF); pc = addr; num_states += 6; } break; // CC addr
            case 0xDD: break; // NOP_DD
            case 0xDE: // SBI byte
                {
                    ac = (a & 0x0F) < (arg1 & 0x0F) + cy;
                    cy = a < arg1 + cy;
                    a -= arg1 + cy;
                    set_szp(a);
                }
                break;
            case 0xDF: write_mem(--sp, pc >> 8); write_mem(--sp, pc & 0xFF); pc = 0x18; break; // RST 3
            case 0xE0: if (!p) { pc = read_mem(sp++); pc |= read_mem(sp++) << 8; num_states += 6; } break; // RPO
            case 0xE1: l = read_mem(sp++); h = read_mem(sp++); break; // POP H
            case 0xE2: if (!p) pc = addr; break; // JPO addr
            case 0xE3: // XTHL
                {
                    byte temp = read_mem(sp);
                    write_mem(sp, l);
                    l = temp;

                    temp = read_mem(sp + 1);
                    write_mem(sp + 1, h);
                    h = temp;
                }
                break;
            case 0xE4: if (!p) { write_mem(--sp, pc >> 8); write_mem(--sp, pc & 0xFF); pc = addr; num_states += 6; } break; // CPO addr
            case 0xE5: write_mem(--sp, h); write_mem(--sp, l); break; // PUSH B
            case 0xE6: a &= arg1; set_szp(a); ac = true; cy = false; break; // ANI byte
            case 0xE7: write_mem(--sp, pc >> 8); write_mem(--sp, pc & 0xFF); pc = 0x20; break; // RST 4
            case 0xE8: if (p) { pc = read_mem(sp++); pc |= read_mem(sp++) << 8; num_states += 6; } break; // RPE
            case 0xE9: pc = hl; break; // PCHL
            case 0xEA: if (p) pc = addr; break; // JPE addr
            case 0xEB: std::swap(h, d); std::swap(l, e); break; // XCHG
            case 0xEC: if (p) { write_mem(--sp, pc >> 8); write_mem(--sp, pc & 0xFF); pc = addr; num_states += 6; } break; // CPE addr
            case 0xED: break; // NOP_ED
            case 0xEE: a ^= arg1; set_szp(a); ac = false; cy = false; break; // XRI byte
            case 0xEF: write_mem(--sp, pc >> 8); write_mem(--sp, pc & 0xFF); pc = 0x28; break; // RST 5
            case 0xF0: if (!s) { pc = read_mem(sp++); pc |= read_mem(sp++) << 8; num_states += 6; } break; // RP
            case 0xF1: psw(read_mem(sp++)); a = read_mem(sp++); break; // POP PSW
            case 0xF2: if (!s) pc = addr; break; // JP addr
            case 0xF3: ei = false; break; // DI
            case 0xF4: if (!s) { write_mem(--sp, pc >> 8); write_mem(--sp, pc & 0xFF); pc = addr; num_states += 6; } break; // CP addr
            case 0xF5: write_mem(--sp, a); write_mem(--sp, psw()); break; // PUSH PSW
            case 0xF6: a |= arg1; set_szp(a); ac = false; cy = false; break; // ORI byte
            case 0xF7: write_mem(--sp, pc >> 8); write_mem(--sp, pc & 0xFF); pc = 0x30; break; // RST 6
            case 0xF8: if (s) { pc = read_mem(sp++); pc |= read_mem(sp++) << 8; num_states += 6; } break; // RM
            case 0xF9: sp = hl; break; // SPHL
            case 0xFA: if (s) pc = addr; break; // JM addr
            case 0xFB: ei = true; break; // EI
            case 0xFC: if (s) { write_mem(--sp, pc >> 8); write_mem(--sp, pc & 0xFF); pc = addr; num_states += 6; } break; // CM addr
            case 0xFD: break; // NOP_FD
            // 0xFE CPI
            case 0xFE: {
                byte res = a - arg1;
                s  = (res & 0x80) != 0;
                z  = (res == 0);
                ac = (a & 0x0F) < (arg1 & 0x0F);
                p  = parity[res];
                cy = a < arg1;
            } break;
            case 0xFF: write_mem(--sp, pc >> 8); write_mem(--sp, pc & 0xFF); pc = 0x38; break; // RST 7


            default:
                pc = prev_pc;
                return false;
        }

        num_states += states[opcode];
        states_since_last_second += states[opcode];

        // Throttle to 2 MHz (2,000 states per ms)
        absolute_time_t target_time = delayed_by_ms(time_of_reset, num_states / 2'000);
        if (absolute_time_diff_us(get_absolute_time(), target_time) > 0)
        {
            sleep_until(target_time);
        }

        // Check if one simulated second has elapsed
        if (states_since_last_second >= 2'000'000)
        {
            if (ei)
            {
                ei = false;

                // 0xFF is the only interrupt vector supported
                write_mem(--sp, pc >> 8);
                write_mem(--sp, pc & 0xFF);
                pc = 0x38;

                states_since_last_second += 11;
                num_states += 11;
            }

            // Reset the counter
            states_since_last_second -= 2'000'000;
        }

        return true;
    }

    void reset()
    {
        reset_requested = false;
        kb_byte = 0;

        pc = 0;
        ei = false;
        
        time_of_reset = get_absolute_time();
        num_states = 0;
        states_since_last_second = 0;
    }

    byte read_mem(word addr)
    {
        if (addr < rom::end)
        {
            return rom.bytes[addr];
        }

        if (ram.start <= addr && addr < ram.end)
        {
            return ram.bytes[addr - ram.start];
        }

        return 0xFF;
    }

    void write_mem(word addr, byte value)
    {
        if (mmio_display.start <= addr && addr < mmio_display.end)
        {
            mmio_display.bytes[addr - mmio_display.start] = value;
            redraw = true;
        }
        else if (ram.start <= addr && addr < ram.end)
        {
            ram.bytes[addr - ram.start] = value;
        }
    }

    byte read_io(byte port)
    {
        switch (port)
        {
            case 0xA0:
            {
                byte result = key_code[kb_byte];
                kb_byte = 0x00;
                return result;
            }

            default: return 0xFF;
        }
    }

    void write_io(byte port, byte value)
    {
    }
    
    std::string disassemble(word addr)
    {
        std::string result;

        result += hex_byte[addr >> 8];
        result += hex_byte[addr & 0xFF];
        result += ' ';

        byte opcode = read_mem(addr);
        byte arg1 = read_mem(addr + 1);
        byte arg2 = read_mem(addr + 2);

        int num_args = opcode_arg_size[opcode];

        result += hex_byte[opcode];
        result += ' ';

        if (num_args > 0)
        {
            result += hex_byte[arg1];
            result += ' ';

            if (num_args > 1)
            {
                result += hex_byte[arg2];
                result += ' ';
            }
            else
            {
                result += "   ";
            }
        }
        else
        {
            result += "      ";
        }

        result += mnemonic[opcode];
        if (num_args == 1)
        {
            result += hex_byte[arg1];
        }
        else if (num_args == 2)
        {
            result += hex_byte[arg2];
            result += hex_byte[arg1];
        }

        return result;
    }
};
