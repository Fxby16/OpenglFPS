#include <raylib.h>
#include <raymath.h>

Vector3 MatVecMul(const Matrix& matrix, const Vector3& vector){
    Vector3 result;
    result.x = matrix.m0*vector.x + matrix.m4*vector.y + matrix.m8*vector.z + matrix.m12;
    result.y = matrix.m1*vector.x + matrix.m5*vector.y + matrix.m9*vector.z + matrix.m13;
    result.z = matrix.m2*vector.x + matrix.m6*vector.y + matrix.m10*vector.z + matrix.m14;
    return result;
}