#pragma once

#include <irrlicht/irrlicht.h>

#include <iostream>  // for ostream, <<, etc.
#include <cstdlib>   // for rand, etc.
#include <cfloat>    // for FLT_MAX, etc.
#include <cmath>     // for sqrt, etc.
#include <vector>    // for std::vector
#include <cassert>   // for assert
#include <limits>    // for numeric_limits
#include <stdlib.h>
#include <time.h>

using namespace irr;

namespace util
{

    // ----------------------------------------------------------------------------
    // Interpolation
    
    template <typename T>
    inline T interpolate (float alpha, const T& v, const T& w )
    {
        return v + ((w-v) * alpha);
    }

    // ----------------------------------------------------------------------------
    // Random number utilities

    //Returns a random between 0 and 1
    template <typename T>
    inline T random01()
    {
        return (((T)rand())/((T)RAND_MAX));
    }

    //Returns a random number between the arguments lower and upper
    template <typename T>
    inline T random(T lower, T upper)
    {
        return lower + (random01<float>() * (upper - lower));
    }

    namespace vector
    {
        template <typename T>
        struct Vector2
        {
            T x; 
            T y;
        };

        template <typename Vector3 = core::vector3df>
        float getAngleRadian(const Vector3& vector)
        {
            return atan2(vector.X, -vector.Z);
        }

        template <typename Vector3 = core::vector3df>
        float getAngleDegree(const Vector3& vector)
        {
            return getAngleRadian(vector) * 180/ M_PI;
        }

    };


};