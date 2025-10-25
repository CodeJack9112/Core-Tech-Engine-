
#pragma once
namespace glm {
    struct vec3 { float x,y,z; vec3():x(0),y(0),z(0){} vec3(float X,float Y,float Z):x(X),y(Y),z(Z){} float length() const { return sqrtf(x*x+y*y+z*z); } };
}
