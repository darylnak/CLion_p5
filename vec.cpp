#ifndef _vec_CPP_
#define _vec_CPP_

#include "vec.h"

template <class Object>
const vec<Object> & vec<Object>::operator=( const vec<Object> & rhs )
{
    if( this != &rhs )
    {
        delete [ ] objects;
        currentSize = rhs.size( );
        objects = new Object*[ currentSize ];
        for( int k = 0; k < currentSize; k++ )
            objects[ k ] = rhs.objects[ k ];
    }
    return *this;
}

template <class Object>
void vec<Object>::resize( int newSize )
{
    Object* oldArray = objects;
    int numToCopy = newSize < currentSize ? newSize : currentSize;

    objects = new Object[ newSize ];
    currentSize = newSize;

    for( int k = 0; k < numToCopy; k++ )
        objects[ k ] = oldArray[ k ];

    delete [ ] oldArray;
}

#endif
