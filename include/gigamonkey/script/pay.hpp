// Copyright (c) 2019 Daniel Krawisz
// Distributed under the Open BSV software license, see the accompanying file LICENSE.

#ifndef GIGAMONKEY_SCRIPT_PAY
#define GIGAMONKEY_SCRIPT_PAY

#include <gigamonkey/script.hpp>
#include <gigamonkey/address.hpp>
#include <gigamonkey/signature.hpp>
#include "pattern.hpp"

namespace gigamonkey::bitcoin {
    
    struct pay_to_pubkey {
        static script::pattern pattern(bytes& pubkey) {
            using namespace script;
            return {alternatives{
                push_size{secp256k1::CompressedPubkeySize, pubkey}, 
                push_size{secp256k1::UncompressedPubkeySize, pubkey}}, OP_CHECKSIG};
        }
        
        static bytes script(bytes_view pubkey) {
            using namespace script;
            return compile(program{pubkey, OP_CHECKSIG});
        }
        
        pubkey Pubkey;
        
        bool valid() const {
            return Pubkey.valid();
        }
        
        bytes script() const {
            return script(Pubkey);
        }
        
        pay_to_pubkey(bytes_view script) : Pubkey{} {
            pattern(Pubkey.Value.Value).match(script);
        }
        
        static bytes redeem(const signature& s) {
            using namespace script;
            return compile(instruction::push_data(s));
        }
    };
    
    struct pay_to_address {
        static script::pattern pattern(bytes& address) {
            using namespace script;
            return {OP_DUP, OP_HASH160, push_size{20, address}, OP_EQUALVERIFY, OP_CHECKSIG};
        }
        
        static bytes script(bytes_view a) {
            using namespace script;
            return compile(program{OP_DUP, OP_HASH160, a, OP_EQUALVERIFY, OP_CHECKSIG});
        }
        
        digest<20, LittleEndian> Address;
        
        bool valid() const {
            return Address.valid();
        }
        
        bytes script() const {
            return script(Address);
        }
        
        pay_to_address(bytes_view script) : Address{} {
            pattern(Address.Digest.Array).match(script);
        }
        
        static bytes redeem(const signature& s, const pubkey& p) {
            using namespace script;
            return compile(program{} << instruction::push_data(s) << instruction::push_data(p));
        }
    };
    
}

#endif

