// Copyright (c) 2019 Daniel Krawisz
// Distributed under the Open BSV software license, see the accompanying file LICENSE.

#ifndef BITCOIN_TXID
#define BITCOIN_TXID

#include "hash.hpp"

namespace gigamonkey::bitcoin {
    
    using txid = digest<32>;
    
    txid id(bytes_view b) {
        return hash256(b);
    }
    
}

#endif
