//
//  Utils.cpp
//  d4
//
//  Created by Tom Andersen on 2016-11-25.
//  Copyright © 2016 Tom Andersen. All rights reserved.
//

#include "Utils.hpp"

/// simple,
double* array_create(long length)
{
    return (double*)malloc(length*sizeof(double));
}
