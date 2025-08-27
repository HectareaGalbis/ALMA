
#include "registers.hpp"

namespace Registers {

std::map<std::string, Xbyak::Reg64> reg64 = {
    { "rax", Xbyak::Reg64(Xbyak::Operand::RAX) },
    { "rcx", Xbyak::Reg64(Xbyak::Operand::RCX) },
    { "rdx", Xbyak::Reg64(Xbyak::Operand::RDX) },
    { "rbx", Xbyak::Reg64(Xbyak::Operand::RBX) },
    { "rsp", Xbyak::Reg64(Xbyak::Operand::RSP) },
    { "rbp", Xbyak::Reg64(Xbyak::Operand::RBP) },
    { "rsi", Xbyak::Reg64(Xbyak::Operand::RSI) },
    { "rdi", Xbyak::Reg64(Xbyak::Operand::RDI) },
    { "r8", Xbyak::Reg64(Xbyak::Operand::R8) },
    { "r9", Xbyak::Reg64(Xbyak::Operand::R9) },
    { "r10", Xbyak::Reg64(Xbyak::Operand::R10) },
    { "r11", Xbyak::Reg64(Xbyak::Operand::R11) },
    { "r12", Xbyak::Reg64(Xbyak::Operand::R12) },
    { "r13", Xbyak::Reg64(Xbyak::Operand::R13) },
    { "r14", Xbyak::Reg64(Xbyak::Operand::R14) },
    { "r15", Xbyak::Reg64(Xbyak::Operand::R15) }
};

}
