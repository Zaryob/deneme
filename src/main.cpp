#include "CDeneme.hpp"
#include "DenemeFactory.hpp"

#include <cerrno>
#include <cstddef>
#include <cstring>
#include <cstdint>
#include <iostream>
#include <sys/mman.h>
#include <unistd.h>

namespace {
void CorruptFunctionBodyViaPointer(IDeneme* nesne) {
    auto** vptr = reinterpret_cast<std::uintptr_t**>(nesne);
    std::uintptr_t* vtable = *vptr;

    // Clang/GCC Itanium ABI'de bu sinif icin [2] genelde Yazdir slot'udur.
    constexpr std::size_t kYazdirSlot = 2;
    auto* hedefFonksiyon = reinterpret_cast<std::uint8_t*>(vtable[kYazdirSlot]);

    std::cout << "[+] Orijinal vtable adresi: 0x" << std::hex
              << reinterpret_cast<std::uintptr_t>(vtable) << std::dec << '\n';
    std::cout << "[+] Yazdir fonksiyon adresi: 0x" << std::hex
              << reinterpret_cast<std::uintptr_t>(hedefFonksiyon) << std::dec << '\n';

    long pageSizeRaw = ::sysconf(_SC_PAGESIZE);
    if (pageSizeRaw <= 0) {
        std::cout << "[-] Page size okunamadi." << '\n';
        return;
    }

    auto pageSize = static_cast<std::uintptr_t>(pageSizeRaw);
    auto pageStartAddr = reinterpret_cast<std::uintptr_t>(hedefFonksiyon) & ~(pageSize - 1U);
    void* pageStart = reinterpret_cast<void*>(pageStartAddr);

    if (::mprotect(pageStart, static_cast<std::size_t>(pageSize),
                   PROT_READ | PROT_WRITE | PROT_EXEC) != 0) {
        std::cout << "[-] mprotect(PROT_READ|PROT_WRITE|PROT_EXEC) basarisiz: "
                  << std::strerror(errno) << '\n';
        std::cout << "[!] Yine de fonksiyon kodunu bozmak icin yazma denenecek." << '\n';
    }

    auto* ilkInstr = reinterpret_cast<std::uint32_t*>(hedefFonksiyon);
    std::uint32_t eskiDeger = *ilkInstr;

    // vtable'a dokunmadan, fonksiyon kodunun ilk instruction'ini bozuyoruz.
    *ilkInstr=*ilkInstr+32;
    __builtin___clear_cache(reinterpret_cast<char*>(hedefFonksiyon),
                            reinterpret_cast<char*>(hedefFonksiyon + sizeof(std::uint32_t)));

    std::cout << "[+] Ilk instruction eski deger: 0x" << std::hex << eskiDeger << std::dec
              << '\n';
    std::cout << "[+] Ilk instruction yeni deger: 0x" << std::hex << *ilkInstr << std::dec
              << '\n';

    if (::mprotect(pageStart, static_cast<std::size_t>(pageSize), PROT_READ | PROT_EXEC) != 0) {
        std::cout << "[-] mprotect(PROT_READ|PROT_EXEC) basarisiz: "
                  << std::strerror(errno) << '\n';
    }
    std::cout << "[+] Yazdir fonksiyon adresi: 0x" << std::hex
              << reinterpret_cast<std::uintptr_t>(hedefFonksiyon) << std::dec << '\n';

}
} // namespace

int main() {
    std::cout.setf(std::ios::unitbuf);
    std::cout << "Bu program bilerek tanimsiz davranis uretiyor." << '\n';

    IDeneme* factoryNesnesi = DenemeFactory::Olustur();
    std::cout << "Factory ile nesne olustu." << '\n';
    factoryNesnesi->Yazdir();
    delete factoryNesnesi;

    IDeneme* nesne = new CDeneme();
    std::cout << "new ile IDeneme* nesnesi olusturuldu." << '\n';
    nesne->Yazdir();
    std::cout << "Topla(2, 3): " << nesne->Topla(2, 3) << '\n';

    CorruptFunctionBodyViaPointer(nesne);

    std::cout << "[!] Simdi vtable ayni, ama Yazdir kodu bozuk halde cagrilacak..." << '\n';
    nesne->Yazdir(); // Muhtemelen bu satirda crash olur.

    delete nesne;
    return 0;
}