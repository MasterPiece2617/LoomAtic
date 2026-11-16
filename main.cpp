#include <iostream>
#include <array.hpp>

int main()
{
    // Usamos DynArray de DeSiGNAR (namespace Designar)
    Designar::DynArray<int> v;
    v.append(10);
    v.append(20);
    v.append(30);

    std::cout << "¡DeSiGNAR de los cojones funciona!" << std::endl;
    std::cout << "Elemento v[0]: " << v[0] << std::endl;
    std::cout << "Elemento v[1]: " << v[1] << std::endl;

    return 0;
}