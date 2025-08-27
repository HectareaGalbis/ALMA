
#include "compiler.hpp"
#include <elfio/elfio.hpp>
#include <iomanip>
#include <iostream>

std::string Compiler::get_byte_code() const
{
    const uint8_t* bytecode = this->getCode();
    size_t codesize = this->getSize();
    return std::string(bytecode, bytecode + codesize);
}

static void print_byte_code(const std::string& byte_code)
{
    std::cout << std::endl;
    for (unsigned char byte : byte_code)
        std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << (int)byte << " ";
    std::cout << std::endl;
}

void Compiler::compile(const std::filesystem::path& path)
{
    std::string byte_code = this->get_byte_code();

    std::cout << this->code << std::endl;
    print_byte_code(byte_code);

    ELFIO::elfio writer;

    // Create a new ELF file (32-bit, little endian, executable)
    writer.create(ELFIO::ELFCLASS64, ELFIO::ELFDATA2LSB);
    writer.set_type(ELFIO::ET_EXEC); // Executable
    writer.set_machine(ELFIO::EM_X86_64); // x86
    writer.set_os_abi(ELFIO::ELFOSABI_LINUX);
    writer.set_entry(0x08048000); // Set entry point to start of .text section

    // Add a .text section
    ELFIO::section* text_sec = writer.sections.add(".text");
    text_sec->set_type(ELFIO::SHT_PROGBITS);
    text_sec->set_flags(ELFIO::SHF_ALLOC | ELFIO::SHF_EXECINSTR);
    text_sec->set_addr_align(0x10);

    // Add some "machine code" (just a simple x86 'ret' instruction 0xC3)
    text_sec->set_data(byte_code);

    // Add a program segment for .text
    ELFIO::segment* text_seg = writer.segments.add();
    text_seg->set_type(ELFIO::PT_LOAD);
    text_seg->set_virtual_address(0x08048000);
    text_seg->set_physical_address(0x08048000);
    text_seg->set_flags(ELFIO::PF_X | ELFIO::PF_R);
    text_seg->set_align(0x1000);
    text_seg->add_section_index(text_sec->get_index(), text_sec->get_addr_align());

    // Save ELF file
    if (!writer.save(path)) {
        throw std::runtime_error("Failed to save ELF file\n");
    }

    std::cout << std::endl;
    std::cout << "ELF file '" << path << "' created successfully!" << std::endl;
    std::cout << std::endl;
}
