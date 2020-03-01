// Copyright (c) 2019 Daniel Krawisz
// Distributed under the Open BSV software license, see the accompanying file LICENSE.

#ifndef GIGAMONKEY_SECP256K1
#define GIGAMONKEY_SECP256K1

#include "types.hpp"
#include "hash.hpp"
#include <data/encoding/integer.hpp>
#include <data/iterable.hpp>
#include <secp256k1.h>
#include <pubkey.h>

namespace Gigamonkey::secp256k1 {
    
    const size_t SecretSize = 32;
    
    // There are two representations of public
    // keys that are allowed in Bitcoin. 
    // compressed is default. 
    const size_t CompressedPubkeySize = 33;
    const size_t UncompressedPubkeySize = 65;
    
    // Values written at the start of the standard
    // pubkey representation which tell how it is 
    // represented. 
    enum pubkey_type : byte {
        invalid = 0x00, 
        uncompressed = 0x04,
        compressed_positive = 0x03,
        compressed_negative = 0x02
    };
    
    using coordinate = uint<SecretSize>;
    
    struct point {
        coordinate X;
        coordinate Y;
    };
    
    class secret;
    class pubkey;
    
    class signature {
        friend class secret;
        secp256k1_ecdsa_signature Data;
        
    public:
        constexpr static size_t Size = 64;
        
        signature() : Data{} {}
        
        bool operator==(const signature& s) const {
            for (int i = 0; i < Size; i ++) if (Data.data[i] != s.Data.data[i]) return false;
            return true;
        }
        
        bool operator!=(const signature& s) const {
            return !operator==(s);
        }
        
        operator bytes_view() const {
            return bytes_view{Data.data, Size};
        }
        
        byte* begin() {
            return Data.data;
        }
        
        byte* end() {
            return Data.data + Size;
        }
        
        const byte* begin() const {
            return Data.data;
        }
        
        const byte* end() const {
            return Data.data + Size;
        }
        
        secp256k1::point point() const;
    };
    
    using digest = Gigamonkey::digest<SecretSize>;
    
    class secret {
        static bool valid(bytes_view);
        static bytes to_public_compressed(bytes_view);
        static bytes to_public_uncompressed(bytes_view);
        static signature sign(bytes_view, const digest&);
        static coordinate negate(const coordinate&);
        static coordinate plus(const coordinate&, bytes_view);
        static coordinate times(const coordinate&, bytes_view);
        
    public:
        constexpr static size_t Size = 32;
        
        coordinate Value;
        
        secret() : Value{0} {}
        explicit secret(const coordinate& v) : Value{v} {}
        explicit secret(string_view s); // hexidecimal and wif accepted. 
        
        bool valid() const {
            return valid(Value);
        }
        
        bool operator==(const secret& s) const {
            return Value == s.Value;
        }
        
        bool operator!=(const secret& s) const {
            return Value != s.Value;
        }
        
        signature sign(const digest& d) const {
            return sign(Value, d);
        }
        
        pubkey to_public() const;
        
        secret operator-() const {
            return secret{negate(Value)};
        }
        
        secret operator+(const secret& s) const {
            return secret{plus(Value, s.Value)};
        }
        
        secret operator*(const secret& s) const {
            return secret{times(Value, s.Value)};
        }
    };
    
    class pubkey {
        static bool valid(bytes_view);
        static bool verify(bytes_view pubkey, digest&, const signature&);
        static bytes compress(bytes_view);
        static bytes decompress(bytes_view);
        static bytes negate(const bytes&);
        static bytes plus_pubkey(const bytes&, bytes_view);
        static bytes plus_secret(const bytes&, bytes_view);
        static bytes times(const bytes&, bytes_view);
        
    public:
        bytes Value;
        
        pubkey() : Value{} {}
        explicit pubkey(bytes_view v) : Value{v} {}
        explicit pubkey(string_view s);
        //explicit pubkey(const CPubKey&);
        
        bool valid() const {
            return valid(Value);
        }
        
        bool operator==(const pubkey& p) const {
            return Value == p.Value;
        }
        
        bool operator!=(const pubkey& p) const {
            return Value != p.Value;
        }
        
        bool verify(digest& d, const signature& s) const {
            return verify(Value, d, s);
        }
        
        size_t size() const {
            return Value.size();
        }
        
        pubkey_type type() const {
            return size() == 0 ? invalid : pubkey_type{Value[0]};
        }
        
        operator bytes_view() const {
            return Value;
        }
        
        coordinate x() const;
        
        coordinate y() const;
        
        secp256k1::point point() const {
            return {x(), y()};
        }
        
        pubkey compress() const {
            return pubkey(compress(Value));
        }
        
        pubkey decompress() const {
            return pubkey(decompress(Value));
        }
        
        pubkey operator-() const {
            return pubkey(negate(Value));
        }
        
        pubkey operator+(const pubkey& p) const {
            return pubkey{plus_pubkey(Value, p.Value)};
        }
        
        pubkey operator+(const secret& s) const {
            return pubkey{plus_secret(Value, s.Value)};
        }
        
        pubkey operator*(const secret& s) const {
            return pubkey{times(Value, s.Value)};
        }
        
        bytes_writer write(bytes_writer w) const {
            return w << Value;
        }
        
        string write_string() const;/* {
            std::stringstream ss;
            ss << std::hex << Value;
            return ss.str();
        }*/
    };
    
    inline bool valid(const secret& s) {
        return s.valid();
    }
    
    inline signature sign(const secret& s, const digest& d) {
        return s.sign(d);
    }
    
    inline pubkey secret::to_public() const {
        return pubkey{to_public_compressed(bytes_view(Value))};
    }
    
    inline secret negate(const secret& s) {
        return -s;
    }
    
    inline secret plus(const secret& a, const secret& b) {
        return a + b;
    }
    
    inline pubkey to_public(const secret& s) {
        return s.to_public();
    }
    
    inline bool valid(const pubkey& p) {
        return p.valid();
    }
    
    inline bool verify(const pubkey& p, digest& d, const signature& s) {
        return p.verify(d, s);
    }
    
    inline pubkey negate(const pubkey& p) {
        return -p;
    }
    
    inline pubkey plus(const pubkey& a, const pubkey& b) {
        return a + b;
    }
    
    inline pubkey plus(const pubkey& a, const secret& b) {
        return a + b;
    }
    
    inline secret times(const secret& a, const secret& b) {
        return a * b;
    }
    
    inline pubkey times(const pubkey& a, const secret& b) {
        return a * b;
    }
    
}

namespace Gigamonkey::Bitcoin {
    using secret = secp256k1::secret;
    using pubkey = secp256k1::pubkey;
}

inline std::ostream& operator<<(std::ostream& o, const Gigamonkey::secp256k1::secret& s) {
    return o << "secret{" << s.Value << "}";
}

inline std::ostream& operator<<(std::ostream& o, const Gigamonkey::secp256k1::pubkey& p);/* {
    return o << "pubkey{" << p.Value << "}";
}*/

inline Gigamonkey::bytes_writer operator<<(Gigamonkey::bytes_writer w, const Gigamonkey::secp256k1::secret& x) {
    return w << x.Value;
}

inline Gigamonkey::bytes_reader operator>>(Gigamonkey::bytes_reader r, Gigamonkey::secp256k1::secret& x);/* {
    return r >> x.Value;
}*/

#endif
