//
//  ScalarField.hpp
//  d4
//
//  Created by Tom Andersen on 2016-11-26.
//  Copyright © 2016 Tom Andersen. All rights reserved.
//

#ifndef ScalarField_hpp
#define ScalarField_hpp

#include <stdio.h>


typedef struct {
    double A; // amplitude
    double dAdx;
    double dAdy;
    double dAdz;
} ScalarField;



#endif /* ScalarField_hpp */
