#ifndef __UTILS_H__
#define __UTILS_H__


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
        if( i==0 ) return x;
        else if( i==1 ) return y;
        else if( i==2 ) return z;
        else return NULL;
    }
    
};

typedef Vec3<float> Vec3f;
typedef Vec3<int> Vec3i;
typedef Vec2<float> Vec2f;

Vec3f getVec3(std::ifstream &ifs) { float x, y, z; ifs >> x >> y >> z; return Vec3f(x, y, z); }


double ceil441(double f)
{
    return ceil(f-0.00001);
}

double floor441(double f)
{
    return floor(f+0.00001);
}

double dotProd(double A[], double B[])
{
    return A[0]*B[0] + A[1]*B[1] + A[2]*B[2];
}

double dotProd(vec3f A, vec3f B)
{
    return A.x*B.x + A.y*B.y + A.z*B.z;
}

vec3f* crossproduct(float Ax, float Ay, float Az, float Bx, float By, float Bz)
{
    vec3f* vector;
    vector->x = (Ay*Bz)-(By*Az);
    vector->y = -(Ax*Bz)+(Bx*Az);
    vector->z = (Ax*By)-(Ay*Bx);
    return vector;
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



#endif