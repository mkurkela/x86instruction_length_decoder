#include "instruction.h"

Instruction::Instruction()
{
  // prefixes
  lock_prefix = false;
  repne_prefix = false;
  CS_OR = false;
  SS_OR = false;
  DS_OR = false;
  ES_OR = false;
  FS_OR = false;
  GS_OR = false;
  branchNotTaken = false;
  branchTaken = false;
  operandSize_OR = false;
  addressSize_OR = false;
  rex = 0;

  length = 0;
  modRM.encoded = 0;
  vex1.encoded = 0;
  vex2.encoded = 0;
  patch = 0;
  immediate = 0;
  prologueSize = 0;
}

void Instruction::clear()
{
  // prefixes
  lock_prefix = false;
  repne_prefix = false;
  CS_OR = false;
  SS_OR = false;
  DS_OR = false;
  ES_OR = false;
  FS_OR = false;
  GS_OR = false;
  branchNotTaken = false;
  branchTaken = false;
  operandSize_OR = false;
  addressSize_OR = false;
  rex = 0;

  length = 0;
  modRM.encoded = 0;
  vex1.encoded = 0;
  vex2.encoded = 0;
  patch = 0;
  immediate = 0;
  prologueSize = 0;  
}

// returns possible patches
unsigned Instruction::getPatch(void)
{
  return patch;
}

int Instruction::decodeInstruction(const void *sym, int n)
{
  unsigned char *insns = (unsigned char *) sym;
  prologueSize = n;
  decodePrefix(insns);
  insns += length;  
  decodeOpcode(insns);
  return length;
}

// Check possible instruction prefixes. Marks the prefix set and adds the
// instruction length by one if prefix is found and moves to next index insns
void Instruction::decodePrefix(unsigned char *insns)
{
  char done = 0;
  do
  {
    switch (*insns)
    {
      case 0xf0:
        lock_prefix = true;
        length++;
        insns++;
        break;
      case 0xf2:
        repne_prefix = true;
        length += 1;
        insns++;
        break;
      case 0xf3:
        rep_prefix = true;
        length++;
        insns++;
        break;
      case 0x2e:
        CS_OR = true;
        branchNotTaken = true;
        length++;
        insns++;
        break;
      case 0x36:
        SS_OR = true;
        length++;
        insns++;
        break;
      case 0x3e:
        DS_OR = true;
        branchTaken = true;
        length++;
        insns++;
        break;
      case 0x26:
        ES_OR = true;
        length++;
        insns++;
        break;
      case 0x64:
        FS_OR = true;
        immediate = 4;
        length += 5;
        insns += 5;
        break;
      case 0x65:
        GS_OR = true;
        immediate = 4;
        length += 5;
        insns += 5;
        break;
      case 0x66:
        operandSize_OR = true;
        length++;
        insns++;
        break;
      case 0x67:
        addressSize_OR = true;
        length++;
        insns++;
        break;
      default:
        done = 1;
    }
  } while (!done);

  // REX prefix is always the last prefix
  if ((*insns & 0xf0) == 0x40)
  {
    rex = *insns;
    length++;
  }
}

void Instruction::decodeOpcode(unsigned char *insns)
{
  // Check select correct opcode map
  int select_map;
  switch (*insns)
  {
    case 0xf8:  // Pop Ev or XOP
      modRM.encoded = insns[1];
      if (modRM.bits.reg == 0)
      {
        evalModRM(insns[1]);
        return;
      }
      else  // FIXME add support for XOP
        length = 0;
        return;
    case 0xc4:  // 3 byte vex encoded
      vex1.encoded = insns[1];
      vex2.encoded = insns[2];
      if (vex1.bits.map == 1)
        select_map = 1;
      else if (vex1.bits.map == 2)
        select_map = 2;
      else
        select_map = 3;
      length += 3;
      insns += 3;
      break;
    case 0x0f:
      if (insns[1] == 0x38) // 3 byte opcode
      {
        select_map = 2;
        length += 2;
        insns += 2;
      }
      else if (insns[1] == 0x3a) // 3 byte opcode
      {
        select_map = 3;
        length += 2;
        insns += 2;
      }
      else  // 2 byte opcode
      {
        select_map = 1;
        length++;
        insns++;
      }
      break;
    case 0xc5:  // 2 byte vex encoded. (map b00001)
      vex2.encoded = insns[1];
      select_map = 1;
      length += 2;
      insns += 2;
      break;
    default:
      select_map = 0;
  }

  int group = ins_lookup[select_map][*insns];
  switch (group)
  {
    case 0:
      length += 1;
      break;
    case 1:
      length += 2;
      break;
    case 2:
      length += 5;
      break;
    case 3:
      evalModRM(insns[1]);
      break;
    case 4:
      length += 1;
      immediate = 1;
      evalModRM(insns[1]);
      break;
    case 5:
      length += 4;
      immediate = 4;
      evalModRM(insns[1]);
      break;
    case 6: //invalid
      length = 0;
      break;
    case 7:
      groupF6F7(insns);
      break;
    case 8:
      immediate = 8;
      length += 8;
      evalModRM(insns[1]);
      break;
    case 9: // jmp 4byte off (need patch)
      length += 5;
      immediate = 4;
      makePatch();
      break;
    case 10:
      groupFF(insns);
      break;
    case 11:
      group0F00(insns);
      break;
    case 12:
      group3dNOW(insns);
      break;
    case 13:
      groupSSE5A(insns);
      break;
  }
}

void Instruction::groupF6F7(unsigned char *insns)
{
  modRM.encoded = insns[1];
  if (modRM.bits.reg == 0 || modRM.bits.reg == 1)
  {
    if (*insns == 0xf6)
    {
      immediate = 1;
      length += 1;
    }
    else
    {
      length += 4;
      immediate = 4;
    }
  }
  evalModRM(insns[1]);
}

void Instruction::groupFF(unsigned char *insns)
{
  modRM.encoded = insns[1];
  if (modRM.bits.reg == 0 || modRM.bits.reg == 1
      || modRM.bits.reg == 6)
    evalModRM(insns[1]);
  else
    length = 0;
}

void Instruction::group0F00(unsigned char *insns)
{
  modRM.encoded = insns[1];
  if (modRM.bits.reg != 6)
    evalModRM(insns[1]);
  else
    length = 0;
}

void Instruction::group3dNOW(unsigned char *insns)
{
  insns = 0;
  length = 0; //not yet supported
}

void Instruction::groupSSE5A(unsigned char *insns)
{
  insns = 0;
  length = 0; //not yet supported
}

void Instruction::evalModRM(unsigned char insns)
{
  modRM.encoded = insns;
  //mod == 00 and rm == 5 opcode, modRM, rip + 32bit
  //mod == 00 opcode, modRM,(SIB)
  //mod == 01 opcode,modRM,(SIB),1 byte immediate
  //mod == 10 opcode,modRM,(SIB),4 byte immeadiate
  //mod == 11 opcode,modRM
  if (modRM.bits.mod == 0 && modRM.bits.rm == 5)
  {
    makePatch();
  }
  else if (modRM.bits.mod == 0)
    (modRM.bits.rm) != 4 ? length += 2 : length += 3;  //check if SIB byte is needed
  else if (modRM.bits.mod == 1)
    (modRM.bits.rm) != 4 ? length += 3 : length += 4;  //check if SIB byte is needed
  else if (modRM.bits.mod == 2)
    (modRM.bits.rm) != 4 ? length += 6 : length += 7;  //check if SIB byte is needed
  else
    length += 2;
}

void Instruction::makePatch()
{
  int n = prologueSize + length;
  if (modRM.encoded == 0)
  {
    patch = (n+0x5)*0x100 + n + 1; //jump 4byte off
    length += 5;
  }
  else
  {
    patch = (n + 0x6)*0x100 + n + 2; //modRM == 5
    length += 6;
  }
}
