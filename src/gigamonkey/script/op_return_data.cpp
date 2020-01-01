// Copyright (c) 2019 Daniel Krawisz
// Distributed under the Open BSV software license, see the accompanying file LICENSE.

#include <gigamonkey/script/op_return_data.hpp>

namespace gigamonkey::bitcoin {
        
    bytes op_return_data::script(queue<bytes> push) {
        using namespace script;
        program p{OP_FALSE, OP_RETURN};
        while (!data::empty(push)) {
            p << instruction::push_data(push.first());
            push = push.rest();
        }
        return compile(p);
    }
    
    op_return_data::op_return_data(bytes_view b) : Push{}, Safe{false}, Valid{false} {
        using namespace script;
        if (!pattern().match(b)) return;
        program p = decompile(b);
        if (b[0] == OP_FALSE) {
            Safe = true;
            p = p.rest().rest();
        } else p = p.rest();
        while (!p.empty()) {
            Push = Push << p.first().data();
            p = p.rest();
        }
    };
    
}
