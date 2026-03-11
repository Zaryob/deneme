#include "CDeneme.hpp"
#include "DenemeFactory.hpp"

#include <cstddef>
#include <cstdint>
#include <iostream>

namespace {
void CorruptVirtualCall(IDeneme* nesne) {
    auto** vptr = reinterpret_cast<std::uintptr_t**>(nesne);
    std::uintptr_t* originalVTable = *vptr;

    constexpr std::size_t kSlotCount = 4;
    auto* fakeVTable = new std::uintptr_t[kSlotCount];
    for (std::size_t i = 0; i < kSlotCount; ++i) {
        fakeVTable[i] = originalVTable[i];
    }

    // Clang/GCC Itanium ABI'de bu sinif icin [2] genelde Yazdir slot'udur.
    constexpr std::size_t kYazdirSlot = 2;
    std::cout << "[+] Orijinal vtable adresi: 0x" << std::hex
              << reinterpret_cast<std::uintptr_t>(originalVTable) << std::dec << '\n';
    std::cout << "[+] Kopya vtable adresi: 0x" << std::hex
              << reinterpret_cast<std::uintptr_t>(fakeVTable) << std::dec << '\n';
    std::cout << "[+] Yazdir slot eski adresi: 0x" << std::hex
              << fakeVTable[kYazdirSlot] << std::dec << '\n';

    // Bilerek tek bir virtual fonksiyon hedefini bozuyoruz.
    fakeVTable[kYazdirSlot] = static_cast<std::uintptr_t>(0x1);

    std::cout << "[+] Yazdir slot yeni adresi: 0x" << std::hex
              << fakeVTable[kYazdirSlot] << std::dec << '\n';

    // Nesneyi bozuk slot iceren kopya vtable'a yonlendir.
    *vptr = fakeVTable;
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

    CorruptVirtualCall(nesne);

    std::cout << "[!] Simdi tek bir bozuk virtual slot ile fonksiyon cagrilacak..." << '\n';
    nesne->Yazdir(); // Muhtemelen bu satirda crash olur.

    delete nesne;
    return 0;
}
