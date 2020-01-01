// Copyright (c) 2019 Daniel Krawisz
// Distributed under the Open BSV software license, see the accompanying file LICENSE.

#ifndef GIGAMONKEY_ADDRESS
#define GIGAMONKEY_ADDRESS

#include "hash.hpp"
#include "secp256k1.hpp"
#include <data/encoding/base58.hpp>

namespace gigamonkey::bitcoin {
    
    gigamonkey::checksum checksum(bytes_view);
    
    inline bytes_writer write_checksum(bytes_writer w, bytes_view b) {
        return w << b << checksum(b);
    }
    
    namespace base58 {
    
        inline string encode(bytes_view b) {
            return data::encoding::base58::write(b);
        }
        
        inline bool decode(bytes&, string_view);
        
        string check_encode(bytes_view);
        
        bool check_decode(bytes&, string_view);
        
    }
    
    struct address {
        enum type : char {
            main = '1', 
            test = 'n'
        };
        
        type Prefix;
        
        using digest = gigamonkey::digest<20, LittleEndian>;
        
        digest Digest;
        
        address() : Prefix{}, Digest{} {}
        address(const digest& d, char p) : Prefix{p}, Digest{d} {}
        
        explicit address(string_view s);
        
        explicit address(const pubkey& pub, type p = main) : address{digest(hash160(pub)), p} {}
        
        explicit address(const secret& s, type p = main) : address{s.to_public(), p} {}
    
        static string write(char prefix, bytes_view b) {
            return base58::check_encode(gigamonkey::write(b.size() + 1, prefix, b));
        }
        
        string write() const {
            return write(Prefix, Digest);
        }
        
        operator string() const {
            return write();
        }
        
        static bool valid_prefix(type p) {
            return p == main || p == test;
        }
        
        bool valid() const {
            return Digest.valid() && valid_prefix(Prefix);
        }
    };
    
    inline address read_address(string_view str) {
        return address{str};
    }
    
}

inline std::ostream& operator<<(std::ostream& o, gigamonkey::bitcoin::address& a) {
    return o << std::string(a);
}

#endif