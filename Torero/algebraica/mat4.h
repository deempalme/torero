#ifndef MAT4_H
#define MAT4_H

#include <math.h>
#include <ostream>

using namespace std;

/*
 +–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––+
 | ------------------------------------ four-dimensional matrix ---------------------------------- |
 +–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––+
*/
template<typename T> class mat4
{
public:
  mat4(T m00 = 1, T m01 = 0, T m02 = 0, T m03 = 0,
       T m10 = 0, T m11 = 1, T m12 = 0, T m13 = 0,
       T m20 = 0, T m21 = 0, T m22 = 1, T m23 = 0,
       T m30 = 0, T m31 = 0, T m32 = 0, T m33 = 1) :
    m_({m00, m01, m02, m03,
       m10, m11, m12, m13,
       m20, m21, m22, m23,
       m30, m31, m32, m33}),
    determinant_(0),
    size_of_(sizeof(T) * 16),
    is_normalized_(false),
    is_determined_(false) {}

  mat4(const mat4<T> &clone){ this = clone; }
  // Converts this matrix into a identity
  mat4<T>& to_identity(){
    m_[0]  = 1, m_[1]  = 0, m_[2]  = 0, m_[3]  = 0;
    m_[4]  = 0, m_[5]  = 1, m_[6]  = 0, m_[7]  = 0;
    m_[8]  = 0, m_[9]  = 0, m_[10] = 1, m_[11] = 0;
    m_[12] = 0, m_[13] = 0, m_[14] = 0, m_[15] = 1;
    is_determined_ = true;
    is_normalized_ = true;
    determinant_ = 1;
  }
  // Calculated the determinant
  const T determinant(){
    if(!is_determined_){
      determinant_ = (m_[0] * m_[5] - m_[1] * m_[4]) *
          (m_[0] * m_[6]  - m_[2]  * m_[4])  -
          (m_[0] * m_[7]  - m_[3]  * m_[4])  *
          (m_[1] * m_[6]  - m_[2]  * m_[5])  +
          (m_[1] * m_[7]  - m_[3]  * m_[5])  *
          (m_[2] * m_[7]  - m_[3]  * m_[6])  +
          (m_[8] * m_[13] - m_[9]  * m_[12]) *
          (m_[8] * m_[14] - m_[10] * m_[12]) -
          (m_[8] * m_[15] - m_[11] * m_[12]) *
          (m_[9] * m_[14] - m_[10] * m_[13]) +
          (m_[9] * m_[15] - m_[11] * m_[13]) *
          (m_[9] * m_[15] - m_[11] * m_[14]);
      is_determined_ = true;
    }
    return determinant_;
  }
  // index array access
  T& operator[](const unsigned int index){
    changed();
    if(index < 16)
      return *m_[index];
    else
      return *m_[0];
  }
  // scalar addition
  const mat4<T> operator+(const T &s){
    return mat4<T>(m_[0] + s, m_[1] + s, m_[2] + s, m_[3] + s,
        m_[4]  + s, m_[4]  + s, m_[5]  + s, m_[6]  + s,
        m_[8]  + s, m_[9]  + s, m_[10] + s, m_[11] + s,
        m_[12] + s, m_[13] + s, m_[14] + s, m_[15] + s);
  }
  // matrix addition
  const mat4<T> operator+(const mat4<T> &m){
    return mat4<T>(m_[0] + m[0], m_[1] + m[1], m_[2] + m[2], m_[3] + m[3],
        m_[4] + m[4], m_[4] + m[4], m_[5] + m[5], m_[6] + m[6],
        m_[8] + m[8], m_[9] + m[9], m_[10] + m[10], m_[11] + m[11],
        m_[12] + m[12], m_[13] + m[13], m_[14] + m[14], m_[15] + m[15]);
  }
  // scalar substraction
  const mat4<T> operator-(const T &s){
    return mat4<T>(m_[0] - s, m_[1] - s, m_[2] - s, m_[3] - s,
        m_[4]  - s, m_[4]  - s, m_[5]  - s, m_[6]  - s,
        m_[8]  - s, m_[9]  - s, m_[10] - s, m_[11] - s,
        m_[12] - s, m_[13] - s, m_[14] - s, m_[15] - s);
  }
  // matrix substraction
  const mat4<T> operator-(const mat4<T> &m){
    return mat4<T>(m_[0] - m[0], m_[1] - m[1], m_[2] - m[2], m_[3] - m[3],
        m_[4] - m[4], m_[4] - m[4], m_[5] - m[5], m_[6] - m[6],
        m_[8] - m[8], m_[9] - m[9], m_[10] - m[10], m_[11] - m[11],
        m_[12] - m[12], m_[13] - m[13], m_[14] - m[14], m_[15] - m[15]);
  }
  // scalar multiplication
  const mat4<T> operator*(const T &s){
    return mat4<T>(m_[0] * s, m_[1] * s, m_[2] * s, m_[3] * s,
        m_[4] * s, m_[4] * s, m_[5] * s, m_[6] * s,
        m_[8] * s, m_[9] * s, m_[10] * s, m_[11] * s,
        m_[12] * s, m_[13] * s, m_[14] * s, m_[15] * s);
  }
  // matrix multiplication
  const mat4<T> operator*(const mat4<T> &m){
    mat4<T> out();
    T m0 = m[0], m1 = m[1], m2 = m[2], m3 = m[3];
    out[0] = m0 * m_[0] + m1 * m_[4] + m2 * m_[8] + m3 * m_[12];
    out[1] = m0 * m_[1] + m1 * m_[5] + m2 * m_[9] + m3 * m_[13];
    out[2] = m0 * m_[2] + m1 * m_[6] + m2 * m_[10] + m3 * m_[14];
    out[3] = m0 * m_[3] + m1 * m_[7] + m2 * m_[11] + m3 * m_[15];

    m0 = m[4]; m1 = m[5]; m2 = m[6]; m3 = m[7];
    out[4] = m0 * m_[0] + m1 * m_[4] + m2 * m_[8] + m3 * m_[12];
    out[5] = m0 * m_[1] + m1 * m_[5] + m2 * m_[9] + m3 * m_[13];
    out[6] = m0 * m_[2] + m1 * m_[6] + m2 * m_[10] + m3 * m_[14];
    out[7] = m0 * m_[3] + m1 * m_[7] + m2 * m_[11] + m3 * m_[15];

    m0 = m[8]; m1 = m[9]; m2 = m[10]; m3 = m[11];
    out[8] = m0 * m_[0] + m1 * m_[4] + m2 * m_[8] + m3 * m_[12];
    out[9] = m0 * m_[1] + m1 * m_[5] + m2 * m_[9] + m3 * m_[13];
    out[10] = m0 * m_[2] + m1 * m_[6] + m2 * m_[10] + m3 * m_[14];
    out[11] = m0 * m_[3] + m1 * m_[7] + m2 * m_[11] + m3 * m_[15];

    m0 = m[12]; m1 = m[13]; m2 = m[14]; m3 = m[15];
    out[12] = m0 * m_[0] + m1 * m_[4] + m2 * m_[8] + m3 * m_[12];
    out[13] = m0 * m_[1] + m1 * m_[5] + m2 * m_[9] + m3 * m_[13];
    out[14] = m0 * m_[2] + m1 * m_[6] + m2 * m_[10] + m3 * m_[14];
    out[15] = m0 * m_[3] + m1 * m_[7] + m2 * m_[11] + m3 * m_[15];
    return out;
  }
  // matrix assigment addition
  mat4<T>& operator+=(const mat4<T> &m){
    m_[0]  += m[0],  m_[1]  += m[1],  m_[2]  += m[2],  m_[3]  += m[3];
    m_[4]  += m[4],  m_[5]  += m[5],  m_[6]  += m[6],  m_[7]  += m[7];
    m_[8]  += m[8],  m_[9]  += m[9],  m_[10] += m[10], m_[11] += m[11];
    m_[12] += m[12], m_[13] += m[13], m_[14] += m[14], m_[15] += m[15];
    changed();
    return *this;
  }
  // scalar assigment addition
  mat4<T>& operator+=(const T &s){
    m_[0]  += s, m_[1]  += s, m_[2]  += s, m_[3]  += s;
    m_[4]  += s, m_[5]  += s, m_[6]  += s, m_[7]  += s;
    m_[8]  += s, m_[9]  += s, m_[10] += s, m_[11] += s;
    m_[12] += s, m_[13] += s, m_[14] += s, m_[15] += s;
    changed();
    return *this;
  }
  // matrix assigment substraction
  mat4<T>& operator-=(const mat4<T> &m){
    m_[0]  -= m[0],  m_[1]  -= m[1],  m_[2]  -= m[2],  m_[3]  -= m[3];
    m_[4]  -= m[4],  m_[5]  -= m[5],  m_[6]  -= m[6],  m_[7]  -= m[7];
    m_[8]  -= m[8],  m_[9]  -= m[9],  m_[10] -= m[10], m_[11] -= m[11];
    m_[12] -= m[12], m_[13] -= m[13], m_[14] -= m[14], m_[15] -= m[15];
    changed();
    return *this;
  }
  // scalar assigment substraction
  mat4<T>& operator-=(const T &s){
    m_[0]  -= s, m_[1]  -= s, m_[2]  -= s, m_[3]  -= s;
    m_[4]  -= s, m_[5]  -= s, m_[6]  -= s, m_[7]  -= s;
    m_[8]  -= s, m_[9]  -= s, m_[10] -= s, m_[11] -= s;
    m_[12] -= s, m_[13] -= s, m_[14] -= s, m_[15] -= s;
    changed();
    return *this;
  }

  // vector assigment multiplication
  mat4<T>& operator*=(const mat4<T> &_b){
    values[0] *= _b.x();
    values[1] *= _b.y();
    values[2] *= _b.z();
    values[3] *= _b.w();
    changed();
    return *this;
  }
  // vector assigment division
  mat4<T>& operator/=(const mat4<T> &_b){
    values[0] /= _b.x();
    values[1] /= _b.y();
    values[2] /= _b.z();
    values[3] /= _b.w();
    changed();
    return *this;
  }
  // printing the values
  ostream& operator<<(ostream &os){
    return os << "mat4(" <<
                 m_[0]  << ", " << m_[1]  << ", " << m_[2]  << ", " << m_[3]  << ",\n" <<
                 m_[4]  << ", " << m_[5]  << ", " << m_[6]  << ", " << m_[7]  << ",\n" <<
                 m_[8]  << ", " << m_[9]  << ", " << m_[10] << ", " << m_[11] << ",\n" <<
                 m_[12] << ", " << m_[13] << ", " << m_[14] << ", " << m_[15] << ")\n";
  }
  // constant pointer to matrix data
  const T* data() const{
    return &m_;
  }
  // data size in bytes of this matrix
  const unsigned int size_of(){
    return sizeOf;
  }
  // normalize this matrix
  mat4<T>& normalize(){
    if(!is_normalized_){
      is_normalized_ = true;
      T d(determinant());
      if(d > 0) d = 1 / d;
      x_n = values[0] *= d;
      y_n = values[1] *= d;
      z_n = values[2] *= d;
      w_n = values[3] *= d;
    }
    return *this;
  }
  // normalized vector
  const mat4<T> normalized(){
    if(!isNormalized){
      isNormalized = true;
      T d(length());
      if(d > 0) d = 1 / d;
      x_n = values[0] * d;
      y_n = values[1] * d;
      z_n = values[2] * d;
      w_n = values[3] * d;
    }
    return mat4<T>(x_n, y_n, z_n, w_n);
  }

private:
  void changed(){
    is_normalized_ = false;
    is_determined_ = false;
  }

  T m_[16];
  T determinant_;
  unsigned int size_of_;
  bool is_normalized_, is_determined_;
};

typedef mat4<double> mat4d;
typedef mat4<float> mat4f;
typedef mat4<int> mat4i;

#endif // MAT4_H
