#ifndef MAT4_H
#define MAT4_H

#include "vec3.h"
#include "vec4.h"
#include "mat3.h"

#include <math.h>
#include <iostream>
#include <iomanip>

/*
 +–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––+
 | ------------------------------------ four-dimensional matrix ---------------------------------- |
 +–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––+
*/
namespace Algebraica {
  template<typename T> class mat4
  {
  public:
    mat4(T m00 = 1, T m01 = 0, T m02 = 0, T m03 = 0,
         T m10 = 0, T m11 = 1, T m12 = 0, T m13 = 0,
         T m20 = 0, T m21 = 0, T m22 = 1, T m23 = 0,
         T m30 = 0, T m31 = 0, T m32 = 0, T m33 = 1) :
      m_{vec4<T>(m00, m10, m20, m30),
         vec4<T>(m01, m11, m21, m31),
         vec4<T>(m02, m12, m22, m32),
         vec4<T>(m03, m13, m23, m33)}
    {}

    mat4(const vec4<T> column_0,
         const vec4<T> column_1,
         const vec4<T> column_2,
         const vec4<T> column_3) :
      m_{column_0, column_1, column_2, column_3}
    {}

    mat4(const mat4<T> &m) { *this = m; }
    /*
   * Converts this matrix into a identity
   */
    mat4<T>& to_identity(){
      m_[0] = vec4<T>(1, 0, 0, 0);
      m_[1] = vec4<T>(0, 1, 0, 0);
      m_[2] = vec4<T>(0, 0, 1, 0);
      m_[3] = vec4<T>(0, 0, 0, 1);
      return *this;
    }
    /*
   * Converts this matrix into a zero matrix
   */
    mat4<T>& to_zero(){
      m_[0] = vec4<T>(0, 0, 0, 0);
      m_[1] = vec4<T>(0, 0, 0, 0);
      m_[2] = vec4<T>(0, 0, 0, 0);
      m_[3] = vec4<T>(0, 0, 0, 0);
      return *this;
    }
    /*
   * Calculated the determinant
   */
    const T determinant(){
      T SubFactor00 = m_[2][2] * m_[3][3] - m_[3][2] * m_[2][3];
      T SubFactor01 = m_[2][1] * m_[3][3] - m_[3][1] * m_[2][3];
      T SubFactor02 = m_[2][1] * m_[3][2] - m_[3][1] * m_[2][2];
      T SubFactor03 = m_[2][0] * m_[3][3] - m_[3][0] * m_[2][3];
      T SubFactor04 = m_[2][0] * m_[3][2] - m_[3][0] * m_[2][2];
      T SubFactor05 = m_[2][0] * m_[3][1] - m_[3][0] * m_[2][1];

      T DetCof[4]{(m_[1][1] * SubFactor00 - m_[1][2] * SubFactor01 + m_[1][3] * SubFactor02),
            -(m_[1][0] * SubFactor00 - m_[1][2] * SubFactor03 + m_[1][3] * SubFactor04),
            +(m_[1][0] * SubFactor01 - m_[1][1] * SubFactor03 + m_[1][3] * SubFactor05),
            -(m_[1][0] * SubFactor02 - m_[1][1] * SubFactor04 + m_[1][2] * SubFactor05)};

      return m_[0][0] * DetCof[0] + m_[0][1] * DetCof[1] +
          m_[0][2] * DetCof[2] + m_[0][3] * DetCof[3];
    }
    /*
   * Rotates a mat4 by the given angle around the given axis
   *
   * @param {number} angle is the angle to rotate the matrix by
   * @param {vec3} v is a vector with the axes to rotate around
   */
    mat4<T>& rotate(const T angle, const vec3<T> &v){
      T const c{std::cos(angle)}, t{T(1) - c};
      T const s{std::sin(angle)};

      T x{v[0]}, y{v[1]}, z{v[2]}, lenght{std::sqrt(x * x + y * y + z * z)};

      if(lenght < 0.001)
        return *this;

      x /= lenght;
      y /= lenght;
      z /= lenght;

      T temp[3]{t * x, t * y, t * z};

      T Rotate[9];
      Rotate[0] = c + temp[0] * x;
      Rotate[1] = temp[0] * y + s * z;
      Rotate[2] = temp[0] * z - s * y;

      Rotate[3] = temp[1] * x - s * z;
      Rotate[4] = c + temp[1] * y;
      Rotate[5] = temp[1] * z + s * x;

      Rotate[6] = temp[2] * x + s * y;
      Rotate[7] = temp[2] * y - s * x;
      Rotate[8] = c + temp[2] * z;

      vec4<T> m0(m_[0]), m1(m_[1]), m2(m_[2]);

      m_[0] = m0 * Rotate[0] + m1 * Rotate[1] + m2 * Rotate[2];
      m_[1] = m0 * Rotate[3] + m1 * Rotate[4] + m2 * Rotate[5];
      m_[2] = m0 * Rotate[6] + m1 * Rotate[7] + m2 * Rotate[8];

      return *this;
    }
    mat4<T>& rotate(const T pitch, const T yaw, const T roll){
      const T cosA{std::cos(pitch)}, sinA{std::sin(pitch)};
      const T cosB{std::cos(yaw)},   sinB{std::sin(yaw)};
      const T cosC{std::cos(roll)},  sinC{std::sin(roll)};

      mat4<T> r_m( cosC * cosB - sinC * sinA * sinB, -sinC * cosA, cosC * sinB + sinC * sinA * cosB, 0,
                   sinC * cosB + cosC * sinA * sinB,  cosC * cosA, sinC * sinB - cosC * sinA * cosB, 0,
                   -cosA * sinB,                       sinA,        cosA * cosB,                      0,
                   0,                                 0,           0,                                1);

      return *this = *this * r_m;
    }
    /*
   * Rotates a matrix by the given angle around the X axis not using SIMD
   *
   * @param {Number} angle is the angle in radians to rotate the matrix by
   */
    mat4<T>& rotate_x(const T angle){
      T s{std::sin(angle)};
      T c{std::cos(angle)};

      vec4<T> m1(m_[1]), m2(m_[2]);

      m_[1] = m1 * c + m2 * s;
      m_[2] = m2 * c - m1 * s;

      return *this;
    }
    /*
   * Rotates a matrix by the given angle around the Y axis not using SIMD
   *
   * @param {Number} angle is the angle in radians to rotate the matrix by
   */
    mat4<T>& rotate_y(const T angle){
      T s{std::sin(angle)};
      T c{std::cos(angle)};

      vec4<T> m0(m_[0]), m2(m_[2]);

      m_[0] = m0 * c - m2 * s;
      m_[2] = m0 * s + m2 * c;

      return *this;
    }
    /*
   * Rotates a matrix by the given angle around the Z axis not using SIMD
   *
   * @param {Number} angle is the angle in radians to rotate the matrix by
   */
    mat4<T>& rotate_z(const T angle){
      T s{std::sin(angle)};
      T c{std::cos(angle)};

      vec4<T> m0(m_[0]), m1(m_[1]);

      m_[0] = m0 * c + m1 * s;
      m_[1] = m1 * c - m0 * s;

      return *this;
    }
    /*
   * Translates this matrix by the given vector
   *
   * @param {vec3} v translation vector
   */
    mat4<T>& translate(const vec3<T> &v){
      return translate(v[0], v[1], v[2]);
    }
    /*
   * Translates this matrix by given X, Y and Z axes
   *
   * @param {number} x translation in X axis
   * @param {number} y translation in Y axis
   * @param {number} z translation in Z axis
   */
    mat4<T>& translate(const T x, const T y, const T z){
      m_[3] = m_[0] * x + m_[1] * y + m_[2] * z + m_[3];
      return *this;
    }
    /*
   * Scales all axes of this matrix by the given number
   *
   * @param {muber} s scaling factory
   */
    mat4<T>& scale(const T s){
      return scale(s, s, s);
    }
    /*
   * Could scale all axes of this matrix by the given axes value inside the vector
   *
   * @param {vec3} v scaling factory for each axis
   */
    mat4<T>& scale(const vec3<T> &v){
      return scale(v[0], v[1], v[2]);
    }
    /*
   * Scale each axis by a given number
   *
   * @param {number} x scaling factory for X axis
   * @param {number} y scaling factory for Y axis
   * @param {number} z scaling factory for Z axis
   */
    mat4<T>& scale(const T x, const T y, const T z){
      m_[0] *= x;
      m_[1] *= y;
      m_[2] *= z;

      return *this;
    }
    /*
   * Calculates the normal matrix
   */
    mat3<T> normal_matrix(){
      T Determinant =
          + m_[0][0] * (m_[1][1] * m_[2][2] - m_[1][2] * m_[2][1])
          - m_[0][1] * (m_[1][0] * m_[2][2] - m_[1][2] * m_[2][0])
          + m_[0][2] * (m_[1][0] * m_[2][1] - m_[1][1] * m_[2][0]);

      mat3<T> Inverse;
      Inverse[0][0] =  (m_[1][1] * m_[2][2] - m_[2][1] * m_[1][2]);
      Inverse[0][1] = -(m_[1][0] * m_[2][2] - m_[2][0] * m_[1][2]);
      Inverse[0][2] =  (m_[1][0] * m_[2][1] - m_[2][0] * m_[1][1]);
      Inverse[1][0] = -(m_[0][1] * m_[2][2] - m_[2][1] * m_[0][2]);
      Inverse[1][1] =  (m_[0][0] * m_[2][2] - m_[2][0] * m_[0][2]);
      Inverse[1][2] = -(m_[0][0] * m_[2][1] - m_[2][0] * m_[0][1]);
      Inverse[2][0] =  (m_[0][1] * m_[1][2] - m_[1][1] * m_[0][2]);
      Inverse[2][1] = -(m_[0][0] * m_[1][2] - m_[1][0] * m_[0][2]);
      Inverse[2][2] =  (m_[0][0] * m_[1][1] - m_[1][0] * m_[0][1]);
      Inverse /= Determinant;

      return Inverse;
    }
    /*
   * Creates an orthogonal matrix
   *
   * @param {number} left is the left limit measured from center
   * @param {number} right is the right limit measured from center
   * @param {number} bottom is the bottom limit measured from center
   * @param {number} top is the left top measured from center
   * @param {number} near_plane indicates the minimum limit for the objects to appear in screen
   * @param {number} far_plane indicates the maximum limit for the objects to appear in screen
   */
    mat4<T>& ortho(const T left, const T right, const T bottom, const T top,
                   const T near_plane, const T far_plane){
      to_identity();
      m_[0][0] = static_cast<T>(2) / (right - left);
      m_[1][1] = static_cast<T>(2) / (top - bottom);
      m_[2][2] = - static_cast<T>(2) / (far_plane - near_plane);
      m_[3][0] = - (right + left) / (right - left);
      m_[3][1] = - (top + bottom) / (top - bottom);
      m_[3][2] = - (far_plane + near_plane) / (far_plane - near_plane);

      return *this;
    }
    /*
   * Creates a perspective matrix
   *
   * @param {number} fov is the field of view in radians
   * @param {number} aspect is the aspect radio
   * @param {number} near indicates the minimum limit for the objects to appear in screen
   * @param {number} far indicates the maximum limit for the objects to appear in screen
   */
    mat4<T>& perspective(const T fov, const T aspect, const T near, const T far){
      T const tanHalfFov = std::tan(fov / static_cast<T>(2));

      to_zero();

      m_[0][0] = static_cast<T>(1) / (aspect * tanHalfFov);
      m_[1][1] = static_cast<T>(1) / (tanHalfFov);
      m_[2][2] = - (far + near) / (far - near);
      m_[2][3] = - static_cast<T>(1);
      m_[3][2] = - (static_cast<T>(2) * far * near) / (far - near);

      return *this;
    }
    /*
   * Pocisionates the camera in a 3D space
   *
   * @param {vec3} eye is the 3D position of the camera's lenses
   * @param {vec3} center is the 3D position of the point in which the camera is aimed to
   * @param {vec3} up is the camera's vertical position vector
   */
    mat4<T>& look_at(vec3<T> eye, vec3<T> center, vec3<T> up){
      vec3<T> f(vec3<T>::normalize(center - eye));
      vec3<T> s(vec3<T>::normalize(vec3<T>::cross(f, up)));
      vec3<T> u(vec3<T>::cross(s, f));

      to_identity();

      m_[0][0] = s[0];
      m_[1][0] = s[1];
      m_[2][0] = s[2];
      m_[0][1] = u[0];
      m_[1][1] = u[1];
      m_[2][1] = u[2];
      m_[0][2] =-f[0];
      m_[1][2] =-f[1];
      m_[2][2] =-f[2];
      m_[3][0] =-vec3<T>::dot(s, eye);
      m_[3][1] =-vec3<T>::dot(u, eye);
      m_[3][2] = vec3<T>::dot(f, eye);

      return *this;
    }

    static mat4<T> lookAt(vec3<T> eye, vec3<T> center, vec3<T> up){
      vec3<T> f(vec3<T>::normalize(center - eye));
      vec3<T> s(vec3<T>::normalize(vec3<T>::cross(f, up)));
      vec3<T> u(vec3<T>::cross(s, f));

      mat4<T> matrix;

      matrix[0][0] = s[0];
      matrix[1][0] = s[1];
      matrix[2][0] = s[2];
      matrix[0][1] = u[0];
      matrix[1][1] = u[1];
      matrix[2][1] = u[2];
      matrix[0][2] =-f[0];
      matrix[1][2] =-f[1];
      matrix[2][2] =-f[2];
      matrix[3][0] =-vec3<T>::dot(s, eye);
      matrix[3][1] =-vec3<T>::dot(u, eye);
      matrix[3][2] = vec3<T>::dot(f, eye);

      return matrix;
    }
    /*
   * Returns the same transformation matrix without the translation component
   */
    mat4<T> stationary(){
      mat4<T> stationary_(*this);
      stationary_[3] = vec4f(0.0f, 0.0f, 0.0f, 1.0f);
      stationary_[0][3] = 0.0f;
      stationary_[1][3] = 0.0f;
      stationary_[2][3] = 0.0f;
      return stationary_;
    }
    /*
   * Returns the same transformation matrix without the rotation and scale components
   */
    mat4<T> only_translation(){
      mat4<T> translation_;
      translation_[3] = m_[3];
      return translation_;
    }
    /*
   * Access the data of this matrix
   *
   * @param {unsigned int} index is the array index of this vector data, maximum: 3
   */
    vec4<T>& operator[](const int index){
      assert(index >= 0 && index < 4);
      return m_[index];
    }
    const vec4<T>& operator[](const int index) const{
      assert(index >= 0 && index < 4);
      return m_[index];
    }
    /*
   * Scalar-matrix addition
   *
   * @param {number} s is a scalar number
   */
    mat4<T> operator+(const T s){
      return mat4<T>(m_[0] + s, m_[1] + s, m_[2] + s, m_[3] + s);
    }
    /*
   * Matrix-matrix addition
   *
   * @param {mat4} m is the added matrix
   */
    mat4<T> operator+(const mat4<T> &m){
      return mat4<T>(m_[0] + m[0], m_[1] + m[1], m_[2] + m[2], m_[3] + m[3]);
    }
    /*
   * Scalar-matrix substraction
   *
   * @param {number} s is a scalar number
   */
    mat4<T> operator-(const T s){
      return mat4<T>(m_[0] - s, m_[1] - s, m_[2] - s, m_[3] - s);
    }
    /*
   * Matrix-matrix substraction
   *
   * @param {mat4} m is the substracted matrix
   */
    mat4<T> operator-(const mat4<T> &m){
      return mat4<T>(m_[0] - m[0], m_[1] - m[1], m_[2] - m[2], m_[3] - m[3]);
    }
    /*
   * Scalar-matrix multiplication
   *
   * @param {number} s is a scalar number
   */
    mat4<T> operator*(const T s){
      return mat4<T>(m_[0] * s, m_[1] * s, m_[2] * s, m_[3] * s);
    }
    /*
   * Matrix-matrix multiplication
   *
   * @param {mat4} m is the multiplier matrix
   */
    mat4<T> operator*(const mat4<T> &m){
      mat4<T> out;
      out[0] = m_[0] * m[0][0] + m_[1] * m[0][1] + m_[2] * m[0][2] + m_[3] * m[0][3];
      out[1] = m_[0] * m[1][0] + m_[1] * m[1][1] + m_[2] * m[1][2] + m_[3] * m[1][3];
      out[2] = m_[0] * m[2][0] + m_[1] * m[2][1] + m_[2] * m[2][2] + m_[3] * m[2][3];
      out[3] = m_[0] * m[3][0] + m_[1] * m[3][1] + m_[2] * m[3][2] + m_[3] * m[3][3];
      return out;
    }
    /*
   * Vector-matrix multiplication
   *
   * @param {vec4} v is the multiplier vector
   */
    vec4<T> operator*(const vec4<T> &v){
      return vec4<T>(vec4<T>(m_[0] * v).add(), vec4<T>(m_[1] * v).add(),
          vec4<T>(m_[2] * v).add(), vec4<T>(m_[3] * v).add());
    }
    /*
   * Vector-matrix multiplication
   *
   * @param {vec3} v is the multiplier vector
   */
    vec3<T> operator*(const vec3<T> &v){
      return vec3<T>(vec4<T>(m_[0] * v).add(), vec4<T>(m_[1] * v).add(), vec4<T>(m_[2] * v).add());
    }
    /*
   * matrix-matrix assigment addition
   *
   * @param {mat4} m is the added matrix
   */
    mat4<T>& operator+=(const mat4<T> &m){
      m_[0] += m[0];
      m_[1] += m[1];
      m_[2] += m[2];
      m_[3] += m[3];

      return *this;
    }
    /*
   * scalar-matrix assigment addition
   *
   * @param {number} s is a scalar number
   */
    mat4<T>& operator+=(const T s){
      m_[0] += s;
      m_[1] += s;
      m_[2] += s;
      m_[3] += s;

      return *this;
    }
    /*
   * matrix-matrix assigment substraction
   *
   * @param {mat4} m is the substracted matrix
   */
    mat4<T>& operator-=(const mat4<T> &m){
      m_[0] -= m[0];
      m_[1] -= m[1];
      m_[2] -= m[2];
      m_[3] -= m[3];

      return *this;
    }
    /*
   * scalar-matrix assigment substraction
   *
   * @param {number} s is a scalar number
   */
    mat4<T>& operator-=(const T s){
      m_[0] -= s;
      m_[1] -= s;
      m_[2] -= s;
      m_[3] -= s;

      return *this;
    }
    /*
   * matrix-matrix assigment multiplication
   *
   * @param {mat4} m is the multiplied matrix
   */
    mat4<T>& operator*=(const mat4<T> &m){
      return *this = *this * m;
    }
    /*
   * scalar-matrix assigment multiplication
   *
   * @param {number} s is a scalar number
   */
    mat4<T>& operator*=(const T s){
      m_[0] *= s;
      m_[1] *= s;
      m_[2] *= s;
      m_[3] *= s;

      return *this;
    }
    /*
   * scalar-matrix assigment division
   *
   * @param {number} s is a scalar number
   */
    mat4<T>& operator/=(const T s){
      m_[0] /= s;
      m_[1] /= s;
      m_[2] /= s;
      m_[3] /= s;

      return *this;
    }
    /*
   * value printing
   */
    friend std::ostream& operator<<(std::ostream &os, const mat4<T> &m){
      return os << "mat4(" << std::setprecision(4) << std::fixed <<
                   m[0][0] << ", " << m[1][0] << ", " << m[2][0] << ", " << m[3][0] << ",\n     " <<
                   m[0][1] << ", " << m[1][1] << ", " << m[2][1] << ", " << m[3][1] << ",\n     " <<
                   m[0][2] << ", " << m[1][2] << ", " << m[2][2] << ", " << m[3][2] << ",\n     " <<
                   m[0][3] << ", " << m[1][3] << ", " << m[2][3] << ", " << m[3][3] << ")\n";
    }
    /*
   * pointer to raw data
   */
    T* data(){
      return &data_[0];
    }
    /*
   * constant pointer to raw data
   */
    const T* data() const{
      return &data_[0];
    }

  private:
    union{
      vec4<T> m_[4];
      T data_[16];
    };
  };

  typedef mat4<double> mat4d;
  typedef mat4<float> mat4f;
  typedef mat4<int> mat4i;
  }

#endif // MAT4_H
