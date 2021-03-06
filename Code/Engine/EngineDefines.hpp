#ifndef INCLUDED_ENGINE_DEFINES_HPP
#define INCLUDED_ENGINE_DEFINES_HPP

#define ABSTRACT
#define STATIC 

//#define MATH_DEBUG
#ifdef MATH_DEBUG
#define ONLY_DURING_MATH_DEBUGGING( x ) x
#else
#define ONLY_DURING_MATH_DEBUGGING( x )
#endif

//#define DEBUG_PHYSICS
#ifdef DEBUG_PHYSICS
#define ONLY_DURING_PHYSICS_DEBUGGING( x ) x
#else
#define ONLY_DURING_PHYSICS_DEBUGGING( x )
#endif

#endif //INCLUDED_ENGINE_DEFINES_HPP
