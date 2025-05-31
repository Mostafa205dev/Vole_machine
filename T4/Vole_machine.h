#ifndef VOLE_MACHINE_H
#define VOLE_MACHINE_H
#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <bitset>
#include <cctype>
#include <iomanip>
using namespace std;

class Machine;

// Base class for Instructions (abstract)
class Instruction
{
public:
    virtual ~Instruction() {}
    virtual void execute(Machine &machine) = 0; // Pure virtual function to execute the instruction
};

// Register class representing a CPU register
class Register
{
private:
    int value; // Store the value in the register
public:
    Register() : value(0) {}

    int getValue() const { return value; }
    void setValue(int val) { value = val; }
};

// Memory class representing the memory of the machine
class Memory
{
private:
    vector<int> memoryCells;
    const int memorySize = 256; // Example size (can be adjusted)

public:
    Memory() : memoryCells(memorySize, 0)
    {
        fill(memoryCells.begin(), memoryCells.end(), 0);
    } // Initialize memory with zeros

    int load(int address) const;
    void store(int address, int value);
    void displayMemory() const;
};

// Program Counter (PC)
class ProgramCounter
{
private:
    int value;

public:
    ProgramCounter() : value(0) {}

    int getValue() const { return value; }
    void setValue(int val) { value = val; }
    void increment() { value++; }
};

// Instruction Register (IR)
class InstructionRegister
{
private:
    Instruction *currentInstruction;

public:
    InstructionRegister() : currentInstruction(nullptr) {}

    void set(Instruction *instr) { currentInstruction = instr; }
    Instruction *get() const { return currentInstruction; }
};

// Machine class that represents the entire Vole machine
class Machine
{
private:
    vector<Register> registers; // 16 registers (0-15)
    Memory memory;
    ProgramCounter pc;
    InstructionRegister ir;
    bool isRunning;

public:
    Machine() : registers(16), isRunning(true) {};

    void loadProgram(const vector<string> &program); // Load program into memory
    void fetchAndExecute();                          // Fetch instruction and execute it
    void displayStatus() const;                      // Display the current state of the machine
    bool running() const { return isRunning; }
    void halt() { isRunning = false; }
    bool getRunning() { return isRunning; }
    void start() { isRunning = true; }

    // Access to internal components
    Register &getRegister(int index) { return registers[index]; }
    Memory &getMemory() { return memory; }
    ProgramCounter &getPC() { return pc; }
    InstructionRegister &getIR() { return ir; }
};

// Derived instruction classes
class LoadInstruction : public Instruction
{
private:
    int registerIndex;
    int address;
    bool value;

public:
    LoadInstruction(int reg, int addr, bool vlu) : registerIndex(reg), address(addr), value(vlu) {}

    void execute(Machine &machine) override;
};

class AddInstruction : public Instruction
{
private:
    int regDst;
    int regSrc1;
    int regSrc2;
    bool isFloat;

public:
    AddInstruction(int dst, int src1, int src2, bool flt) : regDst(dst), regSrc1(src1), regSrc2(src2), isFloat(flt) {}

    void execute(Machine &machine) override;
};

class StoreInstruction : public Instruction
{
private:
    int registerIndex;
    int address;

public:
    StoreInstruction(int reg, int addr) : registerIndex(reg), address(addr) {}

    void execute(Machine &machine) override;
};

class JumpInstruction : public Instruction
{
private:
    int registerIndex;
    int address;

public:
    JumpInstruction(int reg, int addr) : registerIndex(reg), address(addr) {}

    void execute(Machine &machine) override;
};

class DJumpInstruction : public Instruction
{
private:
    int registerIndex;
    int address;

public:
    DJumpInstruction(int reg, int addr) : registerIndex(reg), address(addr) {}

    void execute(Machine &machine) override;
};

class HaltInstruction : public Instruction
{
public:
    void execute(Machine &machine) override;
};

class MoveInstruction : public Instruction
{
private:
    int regSrc1;
    int regSrc2;

public:
    MoveInstruction(int reg, int reg2) : regSrc1(reg), regSrc2(reg2) {}

    void execute(Machine &machine) override;
};

// New Instructions

class OrInstruction : public Instruction
{
private:
    int regDst;
    int regSrc1;
    int regSrc2;

public:
    OrInstruction(int dst, int src1, int src2) : regDst(dst), regSrc1(src1), regSrc2(src2) {}

    void execute(Machine &machine) override;
};

class AndInstruction : public Instruction
{
private:
    int regDst;
    int regSrc1;
    int regSrc2;

public:
    AndInstruction(int dst, int src1, int src2) : regDst(dst), regSrc1(src1), regSrc2(src2) {}

    void execute(Machine &machine) override;
};

class XorInstruction : public Instruction
{
private:
    int regDst;
    int regSrc1;
    int regSrc2;

public:
    XorInstruction(int dst, int src1, int src2) : regDst(dst), regSrc1(src1), regSrc2(src2) {}

    void execute(Machine &machine) override;
};

class RotateInstruction : public Instruction
{
private:
    int reg;
    int steps;

public:
    RotateInstruction(int reg, int rs) : reg(reg), steps(rs) {}

    void execute(Machine &machine) override;
};

#endif // VOLE_MACHINE_H
