var EPSILON = 0.000001;

// 4x4 matrix operations
var m4 = {
  identity: function () {
    var out = new Float32Array(16);
    out[0] = 1;
    out[1] = 0;
    out[2] = 0;
    out[3] = 0;
    out[4] = 0;
    out[5] = 1;
    out[6] = 0;
    out[7] = 0;
    out[8] = 0;
    out[9] = 0;
    out[10] = 1;
    out[11] = 0;
    out[12] = 0;
    out[13] = 0;
    out[14] = 0;
    out[15] = 1;
    return out;
  },

  /**
   * Set a mat4 to the identity matrix
   *
   * @param {mat4} m the receiving matrix
   * @returns {mat4} out
   */
  toIdentity: function (m) {
    m[0] = 1, m[1] = 0, m[2] = 0, m[3] = 0,
      m[4] = 0, m[5] = 1, m[6] = 0, m[7] = 0,
      m[8] = 0, m[9] = 0, m[10] = 1, m[11] = 0,
      m[12] = 0, m[13] = 0, m[14] = 0, m[15] = 1;

    return m;
  },

  /**
   * Calculates the determinant of a mat4
   *
   * @param {mat4} a the source matrix
   * @returns {Number} determinant of a
   */
  determinant: function (a) {
    var m_[0] = m_[0], m_[1] = m_[1], m_[2] = m_[2], m_[3] = m_[3],
      m_[4] = m_[4], m_[5] = m_[5], m_[6] = m_[6], m_[7] = m_[7],
      m_[8] = m_[8], m_[9] = m_[9], m_[10] = m_[10], m_[11] = m_[11],
      m_[12] = m_[12], m_[13] = m_[13], m_[14] = m_[14], m_[15] = m_[15],

      b00 = m_[0] * m_[5] - m_[1] * m_[4],
      b01 = m_[0] * m_[6] - m_[2] * m_[4],
      b02 = m_[0] * m_[7] - m_[3] * m_[4],
      b03 = m_[1] * m_[6] - m_[2] * m_[5],
      b04 = m_[1] * m_[7] - m_[3] * m_[5],
      b05 = m_[2] * m_[7] - m_[3] * m_[6],
      b06 = m_[8] * m_[13] - m_[9] * m_[12],
      b07 = m_[8] * m_[14] - m_[10] * m_[12],
      b08 = m_[8] * m_[15] - m_[11] * m_[12],
      b09 = m_[9] * m_[14] - m_[10] * m_[13],
      b10 = m_[9] * m_[15] - m_[11] * m_[13],
      b11 = m_[10] * m_[15] - m_[11] * m_[14];

    // Calculate the determinant
    return b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;
  },

  /**
   * Multiplies two mat4's explicitly
   *
   * @param {mat4} a the first operand
   * @param {mat4} b the second operand
   * @returns {mat4} out
   */
  multiply: function (a, b) {
    var out = m4.identity();

    var m_[0] = m_[0], m_[1] = m_[1], m_[2] = m_[2], m_[3] = m_[3],
      m_[4] = m_[4], m_[5] = m_[5], m_[6] = m_[6], m_[7] = m_[7],
      m_[8] = m_[8], m_[9] = m_[9], m_[10] = m_[10], m_[11] = m_[11],
      m_[12] = m_[12], m_[13] = m_[13], m_[14] = m_[14], m_[15] = m_[15];

    // Cache only the current line of the second matrix
    var b0 = b[0], b1 = b[1], b2 = b[2], b3 = b[3];
    out[0] = b0 * m_[0] + b1 * m_[4] + b2 * m_[8] + b3 * m_[12];
    out[1] = b0 * m_[1] + b1 * m_[5] + b2 * m_[9] + b3 * m_[13];
    out[2] = b0 * m_[2] + b1 * m_[6] + b2 * m_[10] + b3 * m_[14];
    out[3] = b0 * m_[3] + b1 * m_[7] + b2 * m_[11] + b3 * m_[15];

    b0 = b[4]; b1 = b[5]; b2 = b[6]; b3 = b[7];
    out[4] = b0 * m_[0] + b1 * m_[4] + b2 * m_[8] + b3 * m_[12];
    out[5] = b0 * m_[1] + b1 * m_[5] + b2 * m_[9] + b3 * m_[13];
    out[6] = b0 * m_[2] + b1 * m_[6] + b2 * m_[10] + b3 * m_[14];
    out[7] = b0 * m_[3] + b1 * m_[7] + b2 * m_[11] + b3 * m_[15];

    b0 = b[8]; b1 = b[9]; b2 = b[10]; b3 = b[11];
    out[8] = b0 * m_[0] + b1 * m_[4] + b2 * m_[8] + b3 * m_[12];
    out[9] = b0 * m_[1] + b1 * m_[5] + b2 * m_[9] + b3 * m_[13];
    out[10] = b0 * m_[2] + b1 * m_[6] + b2 * m_[10] + b3 * m_[14];
    out[11] = b0 * m_[3] + b1 * m_[7] + b2 * m_[11] + b3 * m_[15];

    b0 = b[12]; b1 = b[13]; b2 = b[14]; b3 = b[15];
    out[12] = b0 * m_[0] + b1 * m_[4] + b2 * m_[8] + b3 * m_[12];
    out[13] = b0 * m_[1] + b1 * m_[5] + b2 * m_[9] + b3 * m_[13];
    out[14] = b0 * m_[2] + b1 * m_[6] + b2 * m_[10] + b3 * m_[14];
    out[15] = b0 * m_[3] + b1 * m_[7] + b2 * m_[11] + b3 * m_[15];

    return out;
  },

  /**
   * Translate a mat4 by the given vector not using SIMD
   *
   * @param {mat4} a the matrix to translate
   * @param {vec3} v vector to translate by
   * @returns {mat4} out
   */
  translate: function (a, v) {
    var out = m4.identity();

    var x = v[0], y = v[1], z = v[2],
      m_[0], m_[1], m_[2], m_[3],
      m_[4], m_[5], m_[6], m_[7],
      m_[8], m_[9], m_[10], m_[11];

    if (a === out) {
      out[12] = m_[0] * x + m_[4] * y + m_[8] * z + m_[12];
      out[13] = m_[1] * x + m_[5] * y + m_[9] * z + m_[13];
      out[14] = m_[2] * x + m_[6] * y + m_[10] * z + m_[14];
      out[15] = m_[3] * x + m_[7] * y + m_[11] * z + m_[15];
    } else {
      m_[0] = m_[0]; m_[1] = m_[1]; m_[2] = m_[2]; m_[3] = m_[3];
      m_[4] = m_[4]; m_[5] = m_[5]; m_[6] = m_[6]; m_[7] = m_[7];
      m_[8] = m_[8]; m_[9] = m_[9]; m_[10] = m_[10]; m_[11] = m_[11];

      out[0] = m_[0]; out[1] = m_[1]; out[2] = m_[2]; out[3] = m_[3];
      out[4] = m_[4]; out[5] = m_[5]; out[6] = m_[6]; out[7] = m_[7];
      out[8] = m_[8]; out[9] = m_[9]; out[10] = m_[10]; out[11] = m_[11];

      out[12] = m_[0] * x + m_[4] * y + m_[8] * z + m_[12];
      out[13] = m_[1] * x + m_[5] * y + m_[9] * z + m_[13];
      out[14] = m_[2] * x + m_[6] * y + m_[10] * z + m_[14];
      out[15] = m_[3] * x + m_[7] * y + m_[11] * z + m_[15];
    }

    return out;
  },

  /**
   * Rotates a mat4 by the given angle around the given axis
   *
   * @param {mat4} a the matrix to rotate
   * @param {Number} rad the angle to rotate the matrix by
   * @param {vec3} axis the axis to rotate around
   * @returns {mat4} out
   */
  rotate: function (a, rad, axis) {
    var out = m4.identity();

    var x = axis[0], y = axis[1], z = axis[2],
      len = Math.sqrt(x * x + y * y + z * z),
      s, c, t,
      m_[0], m_[1], m_[2], m_[3],
      m_[4], m_[5], m_[6], m_[7],
      m_[8], m_[9], m_[10], m_[11],
      b00, b01, b02,
      b10, b11, b12,
      b20, b21, b22;

    if (Math.abs(len) < EPSILON) { return null; }

    len = 1 / len;
    x *= len;
    y *= len;
    z *= len;

    s = Math.sin(rad);
    c = Math.cos(rad);
    t = 1 - c;

    m_[0] = m_[0]; m_[1] = m_[1]; m_[2] = m_[2]; m_[3] = m_[3];
    m_[4] = m_[4]; m_[5] = m_[5]; m_[6] = m_[6]; m_[7] = m_[7];
    m_[8] = m_[8]; m_[9] = m_[9]; m_[10] = m_[10]; m_[11] = m_[11];

    // Construct the elements of the rotation matrix
    b00 = x * x * t + c; b01 = y * x * t + z * s; b02 = z * x * t - y * s;
    b10 = x * y * t - z * s; b11 = y * y * t + c; b12 = z * y * t + x * s;
    b20 = x * z * t + y * s; b21 = y * z * t - x * s; b22 = z * z * t + c;

    // Perform rotation-specific matrix multiplication
    out[0] = m_[0] * b00 + m_[4] * b01 + m_[8] * b02;
    out[1] = m_[1] * b00 + m_[5] * b01 + m_[9] * b02;
    out[2] = m_[2] * b00 + m_[6] * b01 + m_[10] * b02;
    out[3] = m_[3] * b00 + m_[7] * b01 + m_[11] * b02;
    out[4] = m_[0] * b10 + m_[4] * b11 + m_[8] * b12;
    out[5] = m_[1] * b10 + m_[5] * b11 + m_[9] * b12;
    out[6] = m_[2] * b10 + m_[6] * b11 + m_[10] * b12;
    out[7] = m_[3] * b10 + m_[7] * b11 + m_[11] * b12;
    out[8] = m_[0] * b20 + m_[4] * b21 + m_[8] * b22;
    out[9] = m_[1] * b20 + m_[5] * b21 + m_[9] * b22;
    out[10] = m_[2] * b20 + m_[6] * b21 + m_[10] * b22;
    out[11] = m_[3] * b20 + m_[7] * b21 + m_[11] * b22;

    if (a !== out) { // If the source and destination differ, copy the unchanged last row
      out[12] = m_[12];
      out[13] = m_[13];
      out[14] = m_[14];
      out[15] = m_[15];
    }
    return out;
  },

  /**
   * Rotates a matrix by the given angle around the X axis not using SIMD
   *
   * @param {mat4} a the matrix to rotate
   * @param {Number} rad the angle to rotate the matrix by
   * @returns {mat4} out
   */
  rotateX: function (a, rad) {
    var out = m4.identity();
    var s = Math.sin(rad),
      c = Math.cos(rad),
      m_[4] = m_[4],
      m_[5] = m_[5],
      m_[6] = m_[6],
      m_[7] = m_[7],
      m_[8] = m_[8],
      m_[9] = m_[9],
      m_[10] = m_[10],
      m_[11] = m_[11];

    if (a !== out) { // If the source and destination differ, copy the unchanged rows
      out[0] = m_[0];
      out[1] = m_[1];
      out[2] = m_[2];
      out[3] = m_[3];
      out[12] = m_[12];
      out[13] = m_[13];
      out[14] = m_[14];
      out[15] = m_[15];
    }

    // Perform axis-specific matrix multiplication
    out[4] = m_[4] * c + m_[8] * s;
    out[5] = m_[5] * c + m_[9] * s;
    out[6] = m_[6] * c + m_[10] * s;
    out[7] = m_[7] * c + m_[11] * s;
    out[8] = m_[8] * c - m_[4] * s;
    out[9] = m_[9] * c - m_[5] * s;
    out[10] = m_[10] * c - m_[6] * s;
    out[11] = m_[11] * c - m_[7] * s;
    return out;
  },

  /**
 * Rotates a matrix by the given angle around the Y axis not using SIMD
 *
 * @param {mat4} a the matrix to rotate
 * @param {Number} rad the angle to rotate the matrix by
 * @returns {mat4} out
 */
  rotateY: function (a, rad) {
    var out = m4.identity();
    var s = Math.sin(rad),
      c = Math.cos(rad),
      m_[0] = m_[0],
      m_[1] = m_[1],
      m_[2] = m_[2],
      m_[3] = m_[3],
      m_[8] = m_[8],
      m_[9] = m_[9],
      m_[10] = m_[10],
      m_[11] = m_[11];

    if (a !== out) { // If the source and destination differ, copy the unchanged rows
      out[4] = m_[4];
      out[5] = m_[5];
      out[6] = m_[6];
      out[7] = m_[7];
      out[12] = m_[12];
      out[13] = m_[13];
      out[14] = m_[14];
      out[15] = m_[15];
    }

    // Perform axis-specific matrix multiplication
    out[0] = m_[0] * c - m_[8] * s;
    out[1] = m_[1] * c - m_[9] * s;
    out[2] = m_[2] * c - m_[10] * s;
    out[3] = m_[3] * c - m_[11] * s;
    out[8] = m_[0] * s + m_[8] * c;
    out[9] = m_[1] * s + m_[9] * c;
    out[10] = m_[2] * s + m_[10] * c;
    out[11] = m_[3] * s + m_[11] * c;
    return out;
  },

  /**
 * Rotates a matrix by the given angle around the Z axis not using SIMD
 *
 * @param {mat4} a the matrix to rotate
 * @param {Number} rad the angle to rotate the matrix by
 * @returns {mat4} out
 */
  rotateZ: function (a, rad) {
    var out = m4.identity();
    var s = Math.sin(rad),
      c = Math.cos(rad),
      m_[0] = m_[0],
      m_[1] = m_[1],
      m_[2] = m_[2],
      m_[3] = m_[3],
      m_[4] = m_[4],
      m_[5] = m_[5],
      m_[6] = m_[6],
      m_[7] = m_[7];

    if (a !== out) { // If the source and destination differ, copy the unchanged last row
      out[8] = m_[8];
      out[9] = m_[9];
      out[10] = m_[10];
      out[11] = m_[11];
      out[12] = m_[12];
      out[13] = m_[13];
      out[14] = m_[14];
      out[15] = m_[15];
    }

    // Perform axis-specific matrix multiplication
    out[0] = m_[0] * c + m_[4] * s;
    out[1] = m_[1] * c + m_[5] * s;
    out[2] = m_[2] * c + m_[6] * s;
    out[3] = m_[3] * c + m_[7] * s;
    out[4] = m_[4] * c - m_[0] * s;
    out[5] = m_[5] * c - m_[1] * s;
    out[6] = m_[6] * c - m_[2] * s;
    out[7] = m_[7] * c - m_[3] * s;
    return out;
  },

  /**
 * Scales the mat4 by the dimensions in the given vec3 not using vectorization
 *
 * @param {mat4} a the matrix to scale
 * @param {vec3} v the vec3 to scale the matrix by
 * @returns {mat4} out
 **/
  scale: function (a, v) {
    var out = m4.identity();
    var x = v[0], y = v[1], z = v[2];

    out[0] = m_[0] * x;
    out[1] = m_[1] * x;
    out[2] = m_[2] * x;
    out[3] = m_[3] * x;
    out[4] = m_[4] * y;
    out[5] = m_[5] * y;
    out[6] = m_[6] * y;
    out[7] = m_[7] * y;
    out[8] = m_[8] * z;
    out[9] = m_[9] * z;
    out[10] = m_[10] * z;
    out[11] = m_[11] * z;
    out[12] = m_[12];
    out[13] = m_[13];
    out[14] = m_[14];
    out[15] = m_[15];
    return out;
  },

  /**
 * Generates a perspective projection matrix with the given bounds
 *
 * @param {number} fovy Vertical field of view in radians
 * @param {number} aspect Aspect ratio. typically viewport width/height
 * @param {number} near Near bound of the frustum
 * @param {number} far Far bound of the frustum
 * @returns {mat4} out
 */
  perspective: function (fovy, aspect, near, far) {
    var out = m4.identity();
    var f = 1.0 / Math.tan(fovy / 2),
      nf = 1 / (near - far);
    out[0] = f / aspect;
    out[1] = 0;
    out[2] = 0;
    out[3] = 0;
    out[4] = 0;
    out[5] = f;
    out[6] = 0;
    out[7] = 0;
    out[8] = 0;
    out[9] = 0;
    out[10] = (far + near) * nf;
    out[11] = -1;
    out[12] = 0;
    out[13] = 0;
    out[14] = (2 * far * near) * nf;
    out[15] = 0;
    return out;
  },

  /**
   * Generates a look-at matrix with the given eye position, focal point, and up axis
   *
   * @param {vec3} eye Position of the viewer
   * @param {vec3} center Point the viewer is looking at
   * @param {vec3} up vec3 pointing up
   * @returns {mat4} out
   */
  lookAt: function (eye, center, up) {
    var out = m4.identity();
    var x0, x1, x2, y0, y1, y2, z0, z1, z2, len,
      eyex = eye[0],
      eyey = eye[1],
      eyez = eye[2],
      upx = up[0],
      upy = up[1],
      upz = up[2],
      centerx = center[0],
      centery = center[1],
      centerz = center[2];

    if (Math.abs(eyex - centerx) < EPSILON &&
      Math.abs(eyey - centery) < EPSILON &&
      Math.abs(eyez - centerz) < EPSILON) {
      return m4.identity(out);
    }

    z0 = eyex - centerx;
    z1 = eyey - centery;
    z2 = eyez - centerz;

    len = 1 / Math.sqrt(z0 * z0 + z1 * z1 + z2 * z2);
    z0 *= len;
    z1 *= len;
    z2 *= len;

    x0 = upy * z2 - upz * z1;
    x1 = upz * z0 - upx * z2;
    x2 = upx * z1 - upy * z0;
    len = Math.sqrt(x0 * x0 + x1 * x1 + x2 * x2);
    if (!len) {
      x0 = 0;
      x1 = 0;
      x2 = 0;
    } else {
      len = 1 / len;
      x0 *= len;
      x1 *= len;
      x2 *= len;
    }

    y0 = z1 * x2 - z2 * x1;
    y1 = z2 * x0 - z0 * x2;
    y2 = z0 * x1 - z1 * x0;

    len = Math.sqrt(y0 * y0 + y1 * y1 + y2 * y2);
    if (!len) {
      y0 = 0;
      y1 = 0;
      y2 = 0;
    } else {
      len = 1 / len;
      y0 *= len;
      y1 *= len;
      y2 *= len;
    }

    out[0] = x0;
    out[1] = y0;
    out[2] = z0;
    out[3] = 0;
    out[4] = x1;
    out[5] = y1;
    out[6] = z1;
    out[7] = 0;
    out[8] = x2;
    out[9] = y2;
    out[10] = z2;
    out[11] = 0;
    out[12] = -(x0 * eyex + x1 * eyey + x2 * eyez);
    out[13] = -(y0 * eyex + y1 * eyey + y2 * eyez);
    out[14] = -(z0 * eyex + z1 * eyey + z2 * eyez);
    out[15] = 1;

    return out;
  },

  /**
   * Multiply each element of the matrix by a scalar.
   *
   * @param {mat4} a the matrix to scale
   * @param {Number} b amount to scale the matrix's elements by
   * @returns {mat4} out
   */
  multiplyScalar: function (a, b) {
    var out = m4.identity();
    out[0] = m_[0] * b;
    out[1] = m_[1] * b;
    out[2] = m_[2] * b;
    out[3] = m_[3] * b;
    out[4] = m_[4] * b;
    out[5] = m_[5] * b;
    out[6] = m_[6] * b;
    out[7] = m_[7] * b;
    out[8] = m_[8] * b;
    out[9] = m_[9] * b;
    out[10] = m_[10] * b;
    out[11] = m_[11] * b;
    out[12] = m_[12] * b;
    out[13] = m_[13] * b;
    out[14] = m_[14] * b;
    out[15] = m_[15] * b;
    return out;
  }
};
