// Copyright (c) 2019 Daniel Krawisz
// Distributed under the Open BSV software license, see the accompanying file LICENSE.

#ifndef GIGAMONKEY_WIF
#define GIGAMONKEY_WIF

#include "signature.hpp"
#include "address.hpp"
#include <gigamonkey/ecies/electrum.hpp>

namespace Gigamonkey::Bitcoin {
    
    struct secret;
        
    bool operator==(const secret&, const secret&);
    bool operator!=(const secret&, const secret&);
    
    ostream& operator<<(ostream&, const secret&);
    
    struct secret {
        
        enum type : byte {
            main = 0x80, 
            test = 0xef
        };
        
        type Prefix;
        secp256k1::secret Secret;
        bool Compressed;
        
        constexpr static byte CompressedSuffix = 0x01;
        
        size_t size() const;
        
        bool valid() const;
        
        // why is wif compressed bigger than wif uncompressed? 
        // because compressed keys were invented after uncompressed keys. 
        // A compressed wif code is denoted with an extra character to
        // distinguish it from an uncompressed wif code. 
        constexpr static size_t CompressedSize{34};
        constexpr static size_t UncompressedSize{33};
        
        secret();
        secret(type p, secp256k1::secret s, bool c = true);
        
        static secret read(string_view s);
        
        secret(string_view s);
        
        static string write(byte, const secp256k1::secret&, bool compressed = true);
        
        string write() const;
        
        pubkey to_public() const;
        
        Bitcoin::address address() const;
        
        secp256k1::signature sign(const digest256& d) const;
        
        signature sign(const signature::document& document, sighash::directive d = directive(sighash::all)) const;
        
        bytes encrypt(const bytes& message) const;
        bytes decrypt(const bytes& message) const;
        
    private:
        static Bitcoin::address::type to_address_type(type t);
    };
    
    bool inline operator==(const secret& a, const secret& b) {
        return a.Prefix == b.Prefix && a.Secret == b.Secret && a.Compressed == b.Compressed;
    }
    
    bool inline operator!=(const secret& a, const secret& b) {
        return !(a == b);
    }
    
    inline ostream& operator<<(ostream& o, const secret& s) {
        return o << s.write();
    }
        
    inline size_t secret::size() const {
        return 33 + (Compressed ? 1 : 0); 
    }
    
    inline bool secret::valid() const {
        return Secret.valid() && (Prefix == main || Prefix == test);
    }
    
    inline secret::secret() : Prefix{0}, Secret{}, Compressed{false} {}
    
    inline Bitcoin::address::type secret::to_address_type(secret::type t) {
        return t == main ? Bitcoin::address::main : Bitcoin::address::test;
    }
    
    inline secret::secret(type p, secp256k1::secret s, bool c) : Prefix{p}, Secret{s}, Compressed{c} {}
    
    inline secret::secret(string_view s) : secret{read(s)} {}
        
    inline string secret::write() const {
        return write(Prefix, Secret, Compressed);
    }
    
    inline pubkey secret::to_public() const {
        return pubkey{Compressed ? Secret.to_public().compress() : Secret.to_public().decompress()};
    }
    
    inline Bitcoin::address secret::address() const {
        return {to_address_type(Prefix), to_public()};
    }
    
    inline secp256k1::signature secret::sign(const digest256& d) const {
        return Secret.sign(d);
    }
    
    inline signature secret::sign(const signature::document& document, sighash::directive d) const {
        return signature::sign(Secret, d, document);
    }
        
    inline bytes secret::encrypt(const bytes& message) const {
        return ECIES::electrum::encrypt(message, to_public());
    }
    
    inline bytes secret::decrypt(const bytes& message) const {
        return ECIES::electrum::decrypt(message, Secret);
    }
    
}

#endif

