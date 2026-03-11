#include "DenemeFactory.hpp"

#include "CDeneme.hpp"

IDeneme* DenemeFactory::Olustur() {
    return new CDeneme();
}
