#ifndef TORERO_BUFFER_H
#define TORERO_BUFFER_H

#include "glad/glad.h"

namespace torero {
  namespace gl {
    class Buffer
    {
    public:
      // construct this Buffer object and creates a new GL_VERTEX_ARRAY if create is true
      Buffer(const bool create = false);
      /*
     * Frees the memory of its GL_VERTEX_ARRAY, GL_ARRAY_BUFFER, and GL_ELEMENT_BUFFER
     */
      ~Buffer();
      /*
     * Creates a new GL_VERTEX_ARRAY if is not yet created, also, creates a new
     * GL_ARRAY_BUFFER if has not been created yet and allocates its buffered data.
     * It also binds this GL_VERTEX_ARRAY and GL_ARRAY_BUFFER
     *
     * @returns bool : false if vertex array was not generated or error while
     *                 creating the array buffer
     */
      bool allocate_array(const GLvoid *data, const GLsizei size_in_bytes,
                          const GLenum ussage = GL_STATIC_DRAW);
      /*
     * Creates a new GL_VERTEX_ARRAY if is not yet created, also, creates a new
     * GL_ELEMENT_BUFFER if has not been created yet and allocates its buffered data.
     * It also binds this GL_VERTEX_ARRAY and GL_ELEMENT_BUFFER
     *
     * @returns bool : false if vertex array was not generated or error happened while
     *                 generating the element buffer
     */
      bool allocate_element(const GLvoid *data, const GLsizei size_in_bytes,
                            const GLenum ussage = GL_STATIC_DRAW);
      /*
     * Allocates a new buffer inside an already created array buffer
     *
     * @returns bool : false if the array buffer has not been created yet using allocate_array()
     */
      bool allocate_section_array(const GLvoid *data, const GLint offset,
                                  const GLsizei size_in_bytes);
      /*
     * Allocates a new buffer inside an already created element buffer
     * Returns false if the element array buffer has not been created yet using allocate_element()
     *
     * @returns bool : false if element buffer was not generated
     */
      bool allocate_section_element(const GLvoid *data, const GLint offset,
                                    const GLsizei size_in_bytes);
      /*
     * Declares the attribute's buffer format
     */
      void attributte_buffer(const GLuint index, const GLint size, const GLint offset,
                             const GLsizei stride, const GLenum type = GL_FLOAT,
                             const GLboolean normalized = GL_FALSE);
      /*
     * Binds this GL_VERTEX_BUFFER_OBJECT
     */
      void buffer_bind();
      /*
     *  Releases this GL_VERTEX_BUFFER_OBJECT
     */
      void buffer_release();
      /*
     * Creates a new GL_VERTEX_ARRAY that could contain a GL_ARRAY_BUFFER and GL_ELEMENT_BUFFER
     *
     * @returns bool : false if vertex array was not generated
     */
      bool create();
      /*
     * Deletes the vertex array and its buffers
     *
     * @returns bool : false if array and buffers were not generated
     */
      bool delete_buffer();
      /*
     * Disables the vertex array's attribute with id = attribute_id
     */
      void disable(const GLuint attribute_id);
      /*
     * Rate for instanced rendering
     */
      void divisor(const GLuint attribute_id, const GLuint divisor);
      /*
     * Enables the vertex array's attribute with id = attribute_id
     */
      void enable(const GLuint attribute_id);
      /*
     * Returns the ID of the buffer array
     *
     * @returns GLuint : OpenGL buffer array ID
     */
      GLuint id_array();
      /*
     * Returns the ID of the elemnt buffer array
     *
     * @returns GLuint : OpenGL element buffer array ID
     */
      GLuint id_element();
      /*
     * Returns the ID of the vertex buffer object
     *
     * @returns GLuint : OpenGL vertex array objetc ID
     */
      GLuint id_vertex();
      /*
     * Returns the size in bytes of the array buffer
     *
     * @returns GLint : size in bytes of array buffer
     */
      GLint size_array();
      /*
     * Returns the size in bytes of the element array buffer
     *
     * @returns GLint : size in bytes of element buffer
     */
      GLint size_element();
      /*
     * Binds this GL_VERTEX_ARRAY_OBJECT
     *
     * @returns bool : false if vertex array was not generated
     */
      bool vertex_bind();
      /*
     * Releases this GL_VERTEX_ARRAY_OBJECT
     */
      void vertex_release();

    private:
      GLuint array_buffer_, element_buffer_, vertex_array_;
    };
  }
}

#endif // TORERO_BUFFER_H
