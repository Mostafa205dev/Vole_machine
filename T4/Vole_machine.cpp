#include "Vole_machine.h"

void Machine::loadProgram(const vector<string> &program)
{
    for (int i = 0; i < program.size(); i++)
    {
        for (int j = 0; j < 2; j++)
        {
            int byte = stoi(program[i].substr(j * 2, j + 2), 0, 16);
            memory.store((2 * i + j + 16), byte);
        }
    }
}

void Machine::fetchAndExecute()
{
    stringstream s;
    s << hex << memory.load(pc.getValue()) << hex << memory.load(pc.getValue() + 1);
    string instruction(s.str());
    if (instruction.length() == 3)
    {
        instruction = instruction.substr(0, 2) + "0" + instruction.substr(2, 3);
    }
    if (instruction.length() < 4)
    {
        cout << "Invalid instruction\n";
        halt();
        return;
    }

    cout << "ins: " << instruction << "\n";
    char op = instruction[0];
    string regS(1, instruction[1]);
    string desS(1, instruction[2]);
    string desS2(1, instruction[3]);
    int reg = stoi(regS);
    int msgAdrs = stoi(instruction.substr(2, 3), 0, 16);
    int three, four;
    try
    {
        three = stoi(desS);
        four = stoi(desS2);
    }
    catch (const std::exception &e)
    {
        three = stoi(desS, 0, 16);
        four = stoi(desS2, 0, 16);
    }

    if (op == '1')
    {
        LoadInstruction ins(reg, msgAdrs, false);
        Instruction *temp = new LoadInstruction(ins);
        ir.set(temp);
    }
    else if (op == '2')
    {
        LoadInstruction ins(reg, msgAdrs, true);
        Instruction *temp = new LoadInstruction(ins);
        ir.set(temp);
    }
    else if (op == '3')
    {
        StoreInstruction ins(reg, msgAdrs);
        Instruction *temp = new StoreInstruction(ins);
        ir.set(temp);
    }
    else if (op == '4')
    {
        MoveInstruction ins(three, four);
        Instruction *temp = new MoveInstruction(ins);
        ir.set(temp);
        ir.get()->execute(*this);
        delete ir.get();
        return;
    }
    else if (op == '5')
    {
        AddInstruction ins(reg, three, four, false);
        Instruction *temp = new AddInstruction(ins);
        ir.set(temp);
    }
    else if (op == '6')
    {
        AddInstruction ins(reg, three, four, true);
        Instruction *temp = new AddInstruction(ins);
        ir.set(temp);
    }
    else if (op == '7')
    {
        OrInstruction ins(reg, three, four);
        Instruction *temp = new OrInstruction(ins);
        ir.set(temp);
    }
    else if (op == '8')
    {
        AndInstruction ins(reg, three, four);
        Instruction *temp = new AndInstruction(ins);
        ir.set(temp);
    }
    else if (op == '9')
    {
        XorInstruction ins(reg, three, four);
        Instruction *temp = new XorInstruction(ins);
        ir.set(temp);
    }
    else if (op == 'A' || op == 'a')
    {
        RotateInstruction ins(reg, four);
        Instruction *temp = new RotateInstruction(ins);
        ir.set(temp);
    }
    else if (op == 'B' || op == 'b')
    {
        JumpInstruction ins(reg, msgAdrs);
        Instruction *temp = new JumpInstruction(ins);
        ir.set(temp);
    }
    else if (op == 'C' || op == 'c')
    {
        halt();
        return;
    }
    else if (op == 'D' || op == 'd')
    {
        DJumpInstruction ins(reg, msgAdrs);
        Instruction *temp = new DJumpInstruction(ins);
        ir.set(temp);
    }
    else
    {
        halt();
        return;
    }

    ir.get()->execute(*this);
    delete ir.get();
    pc.increment();
    pc.increment();
}

void Machine::displayStatus() const
{
    cout << "Output Screen: " << hex << memory.load(0x00) << "\n";
    for (int i = 0; i < registers.size(); i++)
    {
        cout << "R" << i << " : " << hex << registers[i].getValue() << "\n";
    }
    memory.displayMemory();
}

int Memory::load(int address) const
{
    if (address >= 0 && address < memoryCells.size())
    {
        return memoryCells[address];
    }
    else
    {
        cerr << "Error: Invalid memory access at address " << address << "\n";
        return 0;
    }
}

void Memory::store(int address, int value)
{
    if (address >= 0 && address < memoryCells.size())
    {
        memoryCells[address] = value;
    }
    else
    {
        cout << "Error: Invalid memory access at address " << address << "\n";
    }
}

void Memory::displayMemory() const
{
    cout << "\n--- Memory Contents (16x16 Display) ---\n";
    cout << "       ";

    // Display column headers
    for (int col = 0; col < 16; ++col)
    {
        cout << " " << hex << uppercase << setw(2) << setfill('0') << col << " ";
    }
    cout << "\n";

    cout << "    ";
    cout << string(65, '-') << "\n";

    // Display each row with row headers
    for (int row = 0; row < 16; ++row)
    {
        // Display row header
        cout << "0x" << hex << uppercase << setw(2) << setfill('0') << row * 16 << " | ";

        // Display memory contents in 16 columns
        for (int col = 0; col < 16; ++col)
        {
            int address = row * 16 + col;
            cout << " " << setw(2) << setfill('0') << memoryCells[address] << " ";
        }
        cout << "\n";
    }
}

void LoadInstruction::execute(Machine &machine)
{
    if (value)
    {
        machine.getRegister(registerIndex).setValue(machine.getMemory().load(address));
    }
    else
    {
        machine.getRegister(registerIndex).setValue(address);
    }
}

void StoreInstruction::execute(Machine &machine)
{
    machine.getMemory().store(address, machine.getRegister(registerIndex).getValue());

    if (address == 0)
    {
        cout << "Output to screen: " << machine.getRegister(registerIndex).getValue() << "\n";
    }
}

void decomposeFloat(int value, int &sign, int &exponent, int &mantissa)
{
    sign = (value >> 7) & 1;
    exponent = (value >> 4) & 0b111;
    mantissa = value & 0b1111;
}

// Helper function to compose floating-point value from components
int composeFloat(int sign, int exponent, int mantissa)
{
    return (sign << 7) | (exponent << 4) | (mantissa & 0b1111);
}

// Align the mantissas of two floating-point values to the same exponent
void alignMantissas(int &mantissa1, int &exponent1, int &mantissa2, int &exponent2)
{
    while (exponent1 < exponent2)
    {
        mantissa1 >>= 1;
        exponent1++;
    }
    while (exponent2 < exponent1)
    {
        mantissa2 >>= 1;
        exponent2++;
    }
}

void AddInstruction::execute(Machine &machine)
{
    if (isFloat)
    {
        Register &destReg = machine.getRegister(regDst);
        Register &srcReg1 = machine.getRegister(regSrc1);
        Register &srcReg2 = machine.getRegister(regSrc2);

        int src1Value = srcReg1.getValue();
        int src2Value = srcReg2.getValue();
        int sign1, exponent1, mantissa1;
        decomposeFloat(src1Value, sign1, exponent1, mantissa1);

        int sign2, exponent2, mantissa2;
        decomposeFloat(src2Value, sign2, exponent2, mantissa2);

        alignMantissas(mantissa1, exponent1, mantissa2, exponent2);

        int resultSign = sign1, resultExponent = exponent1, resultMantissa;
        if (sign1 == sign2)
        {
            resultMantissa = mantissa1 + mantissa2;
        }
        else
        {
            if (mantissa1 >= mantissa2)
            {
                resultMantissa = mantissa1 - mantissa2;
            }
            else
            {
                resultMantissa = mantissa2 - mantissa1;
                resultSign = sign2;
            }
        }

        while (resultMantissa > 0b1111)
        {
            resultMantissa >>= 1;
            resultExponent++;
        }

        // Handle underflow or overflow
        if (resultExponent >= 0b111)
        {
            resultMantissa = 0b1111;
            resultExponent = 0b111;
        }
        else if (resultExponent < 0)
        {
            resultMantissa = 0;
            resultExponent = 0;
            resultSign = 0;
        }

        // Compose final result and store in destination register
        int resultValue = composeFloat(resultSign, resultExponent, resultMantissa);
        destReg.setValue(resultValue);
    }
    else
    {
        machine.getRegister(regDst).setValue(machine.getRegister(regSrc1).getValue() + machine.getRegister(regSrc2).getValue());
    }
}

void JumpInstruction::execute(Machine &machine)
{
    if (machine.getRegister(registerIndex).getValue() == machine.getRegister(0).getValue())
    {
        machine.getPC().setValue(address);
    }
}

void HaltInstruction::execute(Machine &machine)
{
    machine.halt();
    cout << "Halt instruction success \n";
}

void MoveInstruction::execute(Machine &machine)
{
    machine.getRegister(regSrc2).setValue(machine.getRegister(regSrc1).getValue());
}

void OrInstruction::execute(Machine &machine)
{
    int result = machine.getRegister(regSrc1).getValue() | machine.getRegister(regSrc2).getValue();
    machine.getRegister(regDst).setValue(result);
}

void AndInstruction::execute(Machine &machine)
{
    int result = machine.getRegister(regSrc1).getValue() & machine.getRegister(regSrc2).getValue();
    machine.getRegister(regDst).setValue(result);
}

void XorInstruction::execute(Machine &machine)
{
    int result = machine.getRegister(regSrc1).getValue() ^ machine.getRegister(regSrc2).getValue();
    machine.getRegister(regDst).setValue(result);
}

void RotateInstruction::execute(Machine &machine)
{
    machine.getRegister(reg).setValue(machine.getRegister(reg).getValue() >> steps);
}

void DJumpInstruction::execute(Machine &machine)
{
    if (machine.getRegister(registerIndex).getValue() > machine.getRegister(0).getValue())
    {
        machine.getPC().setValue(address);
    }
}
