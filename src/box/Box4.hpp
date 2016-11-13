//
//  Box4.hpp
//  d4
//
//  Created by Tom Andersen on 2016-10-12.
//  Copyright © 2016 Tom Andersen. All rights reserved.
//

#ifndef Box4_hpp
#define Box4_hpp

#include <stdio.h>

// Box4 - 4D box of space.
// inSize - 4, 8, 16, 32 or 64 or even 128 if you have a lot of memory
// A box of size 8 has 8^4 points, each of which has six scalars. (the h)
// This data is stored in xyz planes - six for each t, (since six components of the linearized metric in a minkowski space)
// So for Box4 of size 8 we will need 8*6 = 48 zfp compressed arrays, (we work in uncompressed, store as compressed).

// There are always 6 h (h00, h01, etc). So hard code that in.
// array size  - one to hold each array.
// routines to load/store a box.
// routines to load intial conditions (usually all 0s, except for some central masses)
// 

class Box4
{
    public:
        Box4(long inSize);
        ~Box4();
        
    private:
        long size;
};

#endif /* Box4_hpp */
