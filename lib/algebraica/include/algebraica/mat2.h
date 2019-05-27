#ifndef ALGEBRAICA_MAT2_H
#define ALGEBRAICA_MAT2_H

#include "vec2.h"

#include <cmath>
#include <iostream>
#include <iomanip>
#include <assert.h>

#ifndef ALGTEM
#define ALGTEM template<typename T>
#endif

/*
 +–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––+
 | ------------------------------------ three-dimensional matrix --------------------------------- |
 +–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––+
*/
namespace algebraica{
  ALGTEM class mat2
  {
  public:
    mat2(T m00 = 1, T m01 = 0,
         T m10 = 0, T m11 = 1 ) :
      m_{vec2<T>(m00, m10), vec2<T>(m01, m11)} {}

    mat2(const mat2<T> &m){ *this = m; }
    /*
     * Converts this matrix into a identity
     */
    mat2<T>& to_identity();
    /*
     * Calculated the determinant
     */
    const T determinant();
    /*
     * Calculates the inverted matrix
     */
    mat2<T>& invert();
    /*
     * returns the row element of this matrix
     */
    vec2<T>& operator[](const unsigned int index);
    /*
     * Sets new values for the matrix
     */
    mat2<T>& operator()(const T m00, const T m01, const T m10, const T m11);
    /*
     * returns a constant of the row element of this matrix
     */
    const vec2<T>& operator[](const unsigned int index) const;
    /*
     * scalar-matrix assigment division
     *
     * @param {number} s is a scalar number
     */
    mat2<T>& operator/=(const T s);
    /*
     * value printing
     */
    friend std::ostream& operator<<(std::ostream &os, const mat2<T> &m){
      return os << "mat2(" << std::setprecision(3) << std::fixed
                << m[0][0] << ", " << m[1][0] << ",\n"
                << "     " << m[0][1] << ", " << m[1][1] << ")\n";
    }
    /*
     * Returns the data's address
     */
    T* data();
    /*
     * Returns the constant data's address
     */
    const T* data() const;

  private:
    union{
      vec2<T> m_[2];
      T data_[4];
    };
  };

  typedef mat2<double> mat2d;
  typedef mat2<float> mat2f;
  typedef mat2<int> mat2i;
}

#endif // ALGEBRAICA_MAT2_H
