#ifndef INSTRUCTION_H
# define INSTRUCTION_H

/* Opcode lookup tables:
   0 == no modRM byte
   1 == no modRM byte, one byte immediate
   2 == no modRM byte, four byte immediate
   3 == modRM byte
   4 == modRM byte + 1 byte immediate
   5 == modRM byte + 4 byte immediate
   6 == invalid 
   7 == F6/F7 group (need to check modRM reg value)
   8 == a0 a4
   9 == jump 4byte off. Need patching
  10 == group FF
  11 == 0xf0 00 group
  12 == 3DNow! instructions
  13 == SSE5A AMD */
  
static const char ins_lookup[][256] = {  
  // One byte opcode <op>
  {3,3,3,3, 1,2,6,6, 3,3,3,3, 1,2,6,6,
   3,3,3,3, 1,2,6,6, 3,3,3,3, 1,2,6,6,
   3,3,3,3, 1,2,6,6, 3,3,3,3, 1,2,6,6,
   3,3,3,3, 1,2,6,6, 3,3,3,3, 1,2,6,6,

   6,6,6,6, 6,6,6,6, 6,6,6,6, 6,6,6,6, // REX prefixes
   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
   6,6,6,6, 6,3,6,6, 2,5,1,4, 6,6,6,6, 
   6,6,6,6, 6,6,6,6, 6,6,6,6, 6,6,6,6, // jump one byte off 

   4,5,4,4, 3,3,3,3, 3,3,3,3, 3,3,3,3, 
   0,0,0,0, 0,0,0,0, 0,0,6,0, 3,3,6,6, 
   8,8,8,8, 0,0,0,0, 1,2,3,3, 3,3,3,3, 
   1,1,1,1, 1,1,1,1, 2,2,2,2, 2,2,2,2, 

   4,5,6,6, 6,6,4,5, 4,0,2,6, 0,1,6,6, 
   3,3,3,6, 6,6,6,0, 3,3,3,3, 3,3,3,3, 
   6,6,6,6, 1,1,1,1, 9,6,6,6, 6,6,6,6, 
   6,0,6,6, 0,0,7,7, 0,0,0,0, 0,0,3,10},

  // Two byte opcode (0F <op>
  {3,6,3,3, 0,0,0,0, 0,0,6,0, 6,12,12,12,
   3,3,3,3, 6,6,6,6, 3,3,3,3, 3,3,3,3,
   6,6,6,6, 6,6,6,6, 3,3,3,3, 3,3,3,3,
   0,0,0,0, 0,0,6,6, 6,6,6,6, 6,6,6,6,

   3,3,3,3, 3,3,3,3, 3,3,3,3, 3,3,3,3,
   3,3,3,3, 3,3,3,3, 3,3,3,3, 3,3,3,3,
   3,3,3,3, 3,3,3,3, 3,3,3,3, 3,3,3,3, 
   3,4,4,4, 3,3,3,0, 3,3,13,13, 3,3,3,3, 

   6,6,6,6, 6,6,6,6, 6,6,6,6, 6,6,6,6, 
   3,3,3,3, 3,3,3,3, 3,3,3,3, 3,3,3,3, 
   0,0,0,3, 4,3,0,0, 3,3,6,3, 4,3,3,3,  // AA invalid on 64bit 
   3,3,3,3, 3,3,3,3, 3,3,4,3, 3,3,3,3, 

   3,3,4,3, 4,4,4,3, 3,3,3,3, 3,3,3,3, 
   3,3,3,3, 3,3,3,3, 3,3,3,3, 3,3,3,3, 
   3,3,3,3, 3,3,3,3, 3,3,3,3, 3,3,3,3, 
   3,3,3,3, 3,3,3,3, 3,3,3,3, 3,3,3,0},

  // Three byte opcode (0F 38 <op>
  {3,3,3,3, 3,3,3,3, 3,3,3,3, 6,6,6,6,
   3,6,6,3, 3,3,3,3, 3,3,3,3, 3,3,3,6,
   3,3,3,3, 3,3,6,3, 3,3,3,3, 3,3,3,3,
   3,3,3,3, 3,3,3,3, 3,3,3,3, 3,3,3,3,

   3,3,6,6, 3,3,3,3, 6,6,6,6, 6,6,6,6,
   3,3,3,3, 6,3,6,6, 3,3,3,3, 3,3,3,3,
   6,6,6,6, 3,3,6,6, 6,6,6,6, 3,3,3,3, 
   6,6,6,6, 3,6,6,6, 3,3,6,6, 6,6,6,6, 

   3,3,3,6, 3,6,3,3, 6,6,6,6, 3,6,3,6, 
   3,3,3,3, 6,6,3,3, 3,3,3,3, 3,3,3,3, 
   3,6,3,6, 3,6,3,3, 3,3,3,3, 3,3,3,3, 
   6,6,6,6, 3,3,3,3, 3,3,3,3, 3,3,3,3, 

   6,6,6,6, 6,6,3,6, 6,6,6,6, 6,6,6,6, 
   3,3,6,6, 3,3,6,6, 6,6,6,3, 3,3,3,3, 
   6,6,6,6, 6,6,6,6, 6,6,6,6, 6,6,6,6, 
   3,3,3,3, 6,3,3,3, 6,6,6,6, 6,6,6,6},

  // Three byte opcode (0F 3A <op>)
  {4,4,4,3, 4,4,4,3, 4,4,4,4, 4,4,4,4,
   6,6,6,6, 4,4,4,4, 4,4,6,6, 6,4,4,4,
   4,4,4,6, 6,6,4,6, 6,6,6,6, 6,6,6,6,
   6,6,6,6, 6,6,6,6, 4,4,6,6, 6,6,4,6,

   4,4,4,6, 4,6,4,6, 4,4,3,3, 3,6,6,6,
   6,6,4,6, 6,6,6,6, 6,6,6,6, 3,3,3,3,
   4,4,4,4, 6,6,6,6, 3,3,3,3, 3,3,3,3, 
   4,6,6,6, 6,6,4,6, 3,3,3,3, 3,3,3,3, 

   6,6,6,6, 6,6,6,6, 6,6,6,6, 6,6,6,6, 
   6,6,6,6, 6,6,6,6, 6,6,6,6, 6,6,6,6, 
   6,6,6,6, 6,6,6,6, 6,6,6,6, 6,6,6,6, 
   6,6,6,6, 6,6,6,6, 6,6,6,6, 6,6,6,6, 

   6,6,6,6, 6,6,6,6, 6,6,4,4, 6,6,6,6, 
   6,6,6,6, 6,6,6,6, 6,6,6,6, 6,6,6,4, 
   6,6,6,6, 6,6,4,6, 6,6,6,6, 6,6,6,6, 
   4,6,6,6, 6,6,6,6, 6,6,6,6, 6,6,6,6}};

class Instruction
{
  private:
    //struct modRM to help finding out lenght of instruction
    struct modRMBits {
      unsigned char rm:3;
      unsigned char reg:3;
      unsigned char mod:2;
    };
    union modRMByte {
      unsigned char encoded;
      modRMBits     bits;
    };

    struct vex1Bits {
      unsigned char map:5;
      unsigned char B:1;
      unsigned char X:1;
      unsigned char R:1;
    };
    union vex1Byte {
      unsigned char encoded;
      vex1Bits bits;
    };
    struct vex2Bits {
      unsigned char pp:2;
      unsigned char L:1;
      unsigned char vvvv:4;
      unsigned char WE:1;
    };
    union vex2Byte {
      unsigned char encoded;
      vex2Bits bits;
    };

    // instruction prefixes
    bool lock_prefix;
    bool repne_prefix;
    bool rep_prefix;
    bool CS_OR;
    bool SS_OR;
    bool DS_OR;
    bool ES_OR;
    bool FS_OR;
    bool GS_OR;
    bool branchNotTaken;
    bool branchTaken;
    bool operandSize_OR;
    bool addressSize_OR;
    char rex;
 
    int length;
    modRMByte modRM;
    vex1Byte vex1;
    vex2Byte vex2;
    unsigned patch;
    char immediate;
    int prologueSize;

    void decodePrefix(unsigned char *insns);
    void decodeOpcode(unsigned char *insns);
    void evalModRM(unsigned char insns);
    void makePatch();
    void groupFF(unsigned char *insns);
    void groupF6F7(unsigned char *insns);
    void group3dNOW(unsigned char *insns);
    void group0F00(unsigned char *insns);
    void groupSSE5A(unsigned char *insns);
 
  public:
    Instruction();
    void clear();
    unsigned getPatch();
    int decodeInstruction(const void *sym, int n);
};
#endif //INSTURCTION_H
