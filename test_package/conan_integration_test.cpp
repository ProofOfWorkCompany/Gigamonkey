#include <gigamonkey/schema/hd.hpp>

using namespace Gigamonkey::Bitcoin::hd;

int main() {
    bip32::secret secret = 
        bip32::secret::read("xprv9s21ZrQH143K3QTDL4LXw2F7HEK3wJUD2nW2nRk4stbPy6cq3jPPqjiChkVvvNKmPGJxWUtg6LnF5kejMRNNU3TGtRBeJgk33yuGBxrMPHi");
}
