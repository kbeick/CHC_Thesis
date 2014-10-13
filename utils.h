#ifndef __UTILS_H__
#define __UTILS_H__

#include <assert.h>
#include <iostream>



template<typename T>
class Vec2
{
public:
    T x, y;
    Vec2() : x(0), y(0) {}
    Vec2(T xx, T yy) : x(xx), y(yy) {}
};

template<typename T>
class Vec3
{
public:
    T x, y, z;
    Vec3() : x(0), y(0), z(0) {}
    Vec3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}
    friend std::ostream & operator << (std::ostream &os, const Vec3<T> &v)
    { os << v.x << ", " << v.y << ", " << v.z; return os; }

    float& operator[] (int i) {
        assert (i>=0 && i<3);
        if( i==0 ) return x;
        else if( i==1 ) return y;
        else if( i==2 ) return z;
    }
};

class Vec3f{
public:
    float x, y, z;
    Vec3f() : x(0), y(0), z(0) {}
    Vec3f(float xx, float yy, float zz) : x(xx), y(yy), z(zz) {}
    friend std::ostream & operator << (std::ostream &os, const Vec3f &v)
    { os << v.x << ", " << v.y << ", " << v.z; return os; }
        
    // cross product
    Vec3f crossProduct(const Vec3f&) const;
    // dot product
    float dotProduct(const Vec3f&) const;

    Vec3f operator-(const Vec3f&) const;
    void  operator-=(const Vec3f&);

    Vec3f operator+(const Vec3f&) const;
    void  operator+=(const Vec3f&);

    Vec3f operator*(double a) const;
    void  operator*=(double a);

    float& operator[] (int i) {
        assert (i>=0 && i<3);
        if     ( i==0 ) return x;
        else if( i==1 ) return y;
        else            return z;
    }
  
};

// typedef Vec3<float> Vec3f;
typedef Vec3<int> Vec3i;
typedef Vec2<float> Vec2f;

Vec3f
Vec3f::operator-(const Vec3f &r) const
{
    return Vec3f(x-r.x, y-r.y, z-r.z);
}
void
Vec3f::operator-=(const Vec3f &r)
{
    x -= r.x;
    y -= r.y;
    z -= r.z;
}
Vec3f
Vec3f::operator+(const Vec3f &r) const
{
    return Vec3f(x+r.x, y+r.y, z+r.z);
}
void
Vec3f::operator+=(const Vec3f &r)
{
    x += r.x;
    y += r.y;
    z += r.z;
}
Vec3f
Vec3f::operator*(double a) const
{
    return Vec3f(x*a, y*a, z*a);
}
void
Vec3f::operator*=(double a)
{
    x *= a;
    y *= a;
    z *= a;
}
// Vec3f
// Vec3f::crossProduct(const Vec3f &r) const
// {
//     Vec3f v;
//     v.x = y*r.z - z*r.y;
//     v.y = z*r.x - x*r.z;
//     v.z = x*r.y - y*r.x;
//     return v;
// }
// float
// Vec3f::dotProduct(const Vec3f &r) const
// {
//     return x*r.x + y*r.y + z*r.z;
// }


double ceil441(double f)
{
    return ceil(f-0.00001);
}

double floor441(double f)
{
    return floor(f+0.00001);
}

float dotProd(double A[], double B[])
{
    return A[0]*B[0] + A[1]*B[1] + A[2]*B[2];
}

float dotProduct(Vec3f A, Vec3f B)
{
    return A.x*B.x + A.y*B.y + A.z*B.z;
}

Vec3f* crossProduct(float Ax, float Ay, float Az, float Bx, float By, float Bz)
{
    Vec3f* vector = new Vec3f((Ay*Bz) - (Az*By), (Az*Bx) - (Ax*Bz), (Ax*By) - (Ay*Bx));
    return vector;
}
Vec3f* crossProduct(const Vec3f &l, const Vec3f &r)
{
    Vec3f* v = new Vec3f((l.y*r.z) - (l.z*r.y), (l.z*r.x) - (l.x*r.z), (l.x*r.y) - (l.y*r.x));
    return v;
}

Vec3f* crossProductNormalized(const Vec3f &l, const Vec3f &r)
{
    Vec3f* result = crossProduct(l,r);
    
    double norm = sqrt( pow(result->x, 2.) + pow(result->y, 2.) + pow(result->z, 2.) );

    result->x = result->x/norm;
    result->y = result->y/norm;
    result->z = result->z/norm;

    return result;
}

/* Returns  an array
 *      First element is -1 if solution is imaginary
 *      Otherwise first element is 1
 *          and second element is solution to the quadratic formula with given coefficients
 */
double* quadraticSolver(double a, double b, double c)
{
    double root = b*b-4*a*c;
    double* result = new double[2];
    
    if (root < 0.0) {
        // then solution is imaginary
        result[0] = -1.0;
    }else{
        result[0] = 1.0;
        double neg = (-b-sqrt(root))/(2*a);
        double pos = (-b+sqrt(root))/(2*a);
        // Find the least non-neg real solution
        if (neg < 0.0) {
            if (pos < 0.0)
                result[0] = -1.0;
            else
                result[1] = pos;
        }else if (pos < 0.0){
            result[1] = neg;
        }else{
            result[1] = std::min(neg, pos);
        }
    }
    return result;
}

/* Returns true if 'val' appears more than once in 'array'. dups[] is an array whose elements rep the "guilty" indices of array */
bool IsDuplicated(std::vector<float> array, int count, float val, int dups[])
{
    int c=0;
    bool result = false;
    for (int i = 0; i < count; i++) {
        if(array[i] == val){
            dups[c++] = i;
            if(c>1){ result = true; }
        }
    }
    return result;
}

/*
 * returns true if 'array' of length 'count' has any duplicate values (the first index of which is put in "guilty index")
*/
bool HasDuplicates(std::vector<float> array, int count, int *guilty_index)
{
    for (int i = 0; i < count; i++) {
        for (int j = i + 1; j < count; j++) {
            if (array[i] == array[j]) {
                *guilty_index = i;
                return true;
            }
        }
    }
    return false;
}


#endif