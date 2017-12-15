#ifndef LINEAR_H
#define LINEAR_H

#include <math.h>
#include <algorithm>
#include <ostream>

/*
 +–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––+
 | -------------------------------------- linear operations -------------------------------------- |
 +–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––+
*/
namespace Algebraica {
  /*
   * Returns min(max(x, minVal), maxVal) for each component in x
   * using the floating-point values minVal and maxVal.
   * Translates this matrix by the given vector
   *
   * @param {T} x : value to clamp
   * @param {T} min_value : minimum value
   * @param {T} max_value : maximum value
   */
  template<typename T>
  const T clamp(const T &value, const T &min_value, const T &max_value){
    return std::min(std::max(value, min_value), max_value);
  }
}

#endif // LINEAR_H
