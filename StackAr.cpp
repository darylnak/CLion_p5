/**
 * MODIFIED:
 *      Author: Daryl Nakamoto
 *      Date: 12/04/17
 */

#include "StackAr.h"

/**
 * Construct the stack.
 */
template <class Object>
StackAr<Object>::StackAr(int capacity) : size(capacity)
{
  topOfStack = -1;
  theArray = new Object*[capacity];
}

/**
 * Test if the stack is logically empty.
 * Return true if empty, false otherwise.
 */
template <class Object>
bool StackAr<Object>::isEmpty( )
{
  return topOfStack == -1;
}

/**
 * Test if the stack is logically full.
 * Return true if full, false otherwise.
 */
template <class Object>
bool StackAr<Object>::isFull( )
{
  return topOfStack == size - 1;
}

        /**
         * Make the stack logically empty.
         */
        template <class Object>
        void StackAr<Object>::makeEmpty( )
        {
            topOfStack = -1;
        }

/**
 * Get the most recently inserted item in the stack.
 * Does not alter the stack.
 * Return the most recently inserted item in the stack.
 * Exception Underflow if stack is already empty.
 */
template <class Object>
Object* StackAr<Object>::top( )
{
  return theArray[ topOfStack ];
}

/**
 * Remove the most recently inserted item from the stack.
 * Exception Underflow if stack is already empty.
 */
template <class Object>
void StackAr<Object>::pop( )
{
  topOfStack--;
}

/**
 * Insert x into the stack, if not already full.
 * Exception Overflow if stack is already full.
 */
template <class Object>
void StackAr<Object>::push(Object* x )
{
  theArray[ ++topOfStack ] = x;
}

/**
 * Return and remove most recently inserted item from the stack.
 * Return most recently inserted item.
 * Exception Underflow if stack is already empty.
 */
template <class Object>
Object* StackAr<Object>::topAndPop( )
{
  return theArray[ topOfStack-- ];
}
