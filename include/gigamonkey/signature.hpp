// Copyright (c) 2019 Daniel Krawisz
// Distributed under the Open BSV software license, see the accompanying file LICENSE.

#ifndef GIGAMONKEY_SIGNATURE
#define GIGAMONKEY_SIGNATURE

#include "secp256k1.hpp"
#include "hash.hpp"
#include "timechain.hpp"

namespace gigamonkey::bitcoin {
    
    namespace sighash {
        
        using directive = uint32_little;
        using fork_id = byte;
    
        enum type : byte {
            unsupported = 0,
            all = 1,
            none = 2,
            single = 3
        };
        
        type base(directive);
        bool anyone_can_pay(directive);
        fork_id fork(directive);
        
        inline bool has_fork_id(directive d) {
            return fork(d) != 0;
        }
        
    };
    
    sighash::directive directive(sighash::type t, sighash::fork_id id, bool anyone_can_pay = false);
    
    struct signature {
        bytes Data;
        signature(const secp256k1::signature raw, sighash::directive);
        bool der() const;
        
        bitcoin::sighash::directive sighash() const {
            return Data[Data.size() - 1];
        }
        
        bytes_view raw() const {
            return bytes_view{Data}.substr(0, Data.size() - 1);
        }
        
        operator bytes_view() const {
            return Data;
        }
    };
    
    struct prevout {
        bytes Output;
        uint<36> Outpoint;
        
        bool valid() const {
            return gigamonkey::output::valid(Output) && gigamonkey::outpoint::valid(Outpoint);
        }
    };
    
    struct vertex {
        list<prevout> Prevout;
        int32_little Version;
        list<output> Outputs;
        uint32_little Locktime;
        
        vertex(list<prevout> p, int32_little v, list<output> o, uint32_little l) : Prevout{p}, Version{v}, Outputs{o}, Locktime{l} {}
    private:
        // Put cached data here.
    };
    
    bytes btc_spend_order(const vertex& v, index i, sighash::directive d);
    
    bytes bch_spend_order(const vertex& v, index i, sighash::directive d);
    
    inline bytes spend_order(const vertex& v, index i, sighash::directive d) {
        return sighash::has_fork_id(d) ? bch_spend_order(v, i, d) : btc_spend_order(v, i, d);
    }
    
    inline digest<32, BigEndian> signature_hash(const vertex& v, index i, sighash::directive d) {
        return signature_hash(spend_order(v, i, d));
    } 
    
    inline signature sign(const vertex& v, index i, sighash::directive x, const secp256k1::secret& s) {
        return signature{secp256k1::sign(s, signature_hash(v, i, x)), x};
    }
    
    bool verify(vector<prevout> p, bytes_view tx, satoshi a, sighash::directive x, const secp256k1::secret& s);
}

inline std::ostream& operator<<(std::ostream& o, const gigamonkey::bitcoin::signature& x) {
    return o << "signature{" << data::encoding::hex::write(x.Data) << "}";
}

#endif