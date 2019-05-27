#ifndef TORERO_CAMERA_VIEW_H
#define TORERO_CAMERA_VIEW_H

#include "torero/gl/uniform_buffer.h"
#include "torero/definition/types.h"
#include "algebraica/algebraica.h"

#include <boost/function.hpp>

namespace torero {
  namespace camera {
    class View
    {
    public:
      View(const algebraica::vec3f position = algebraica::vec3f(-12.0f, 0.0f, 3.5f),
           const algebraica::vec3f target = algebraica::vec3f(0.0f, 0.0f, 0.0f),
           const algebraica::vec3f up = algebraica::vec3f(0.0f, 0.0f, 1.0f),
           const torero::Pose3D *vehicle_frame = nullptr);

      bool blocked() const;
      void blocked(const bool block);
      /*
     * ### Fixing the camera to the current view
     *
     * Setting `fix = true` will fix the camera view (position, target, and up), if `false`
     * then the camera should follow the vehicle frame (default)
     *
     * @parameters
     * {const bool} fix = `true` to fiw camera view, `false` to follow **vehicle frame**.
     *
     */
      void fix_view(const bool fix = true);
      /*
     * ### Obtaining the inversed perspective transformation matrix
     *
     * Returns the _inversed_ **perspective** transformation matrix, this matrix is calculated
     * using **perspective** matrix transformation with window's width and height as parameters
     * to get the aspect radio. A near plane with value `k_near_plane`, far plane with value
     * `k_far_plane` and field of view with value `k_field_of_view`.
     *
     * @returns
     * {const algebraica::mat4f} Returns the 4x4 inversed projection matrix.
     *
     */
      const algebraica::mat4f& inversed_perspective_matrix() const;
      /*
     * ### Obtaining the inversed multiplied matrix of perspective and view matrices
     *
     * Returns the _inversed_ multiplied matrix of **perspective and view**
     * transformation matrices.
     *
     * @returns
     * {const algebraica::mat4f&} Returns the resulting 4x4 matrix of the projection and
     * view matrices multiplication.
     *
     */
      const algebraica::mat4f& inversed_perspective_view_matrix() const;
      /*
     * ### Obtaining the inversed non-translated multiplied matrix of perspective and view matrices
     *
     * Returns the _inversed _**non-translated** multiplied matrix of **perspective and view**
     * transformation matrices. **Note:* this matrix only contains the **rotations** of
     * perspective_view_matrix and **not translations**.
     *
     * @returns
     * {const algebraica::mat4f&} Returns the resulting 4x4 matrix of the projection and
     * view matrices multiplication WITHOUT translation (only rotation).
     *
     */
      const algebraica::mat4f& inversed_static_perspective_view_matrix() const;
      /*
     * ### Obtaining the view inversed transformation matrix
     *
     * Returns the **view** _inversed_ transformation matrix, this matrix is calculated using
     * **look at** matrix transformation with the camera as parameter.
     *
     * @parameters
     * {const algebraica::mat4f} Returns the 4x4 transformation matrix of the camera.
     *
     */
      const algebraica::mat4f& inversed_view_matrix() const;
      /*
     * ### Change view to isometric view
     *
     * Changes the view to proximate **Isometric view**.
     *
     */
      void isometric_view();
      void key_to_camera(const int x = 0, const int y = 0, const int z = 0);
      /*
     * ### Locking the camera to the vehicle frame
     *
     * This functions will lock the camera's orientation to the vehicle's orientation,
     * in other words, the camera will have the same **yaw** than the car.
     *
     * @parameters
     * {const bool} lock = `true` to follow vehicle's orientation
     *
     */
      void lock_camera(const bool lock = true);
      void mouse_to_camera(const int dx, const int dy, const bool rotate = true);
      /*
     * ### Obtaining the perspective transformation matrix
     *
     * Returns the **perspective** transformation matrix, this matrix is calculated using
     * **perspective** matrix transformation with window's width and height as parameters
     * to get the aspect radio. A near plane with value `k_near_plane`, far plane with value
     * `k_far_plane` and field of view with value `k_field_of_view`.
     *
     * @returns
     * {const algebraica::mat4f} Returns the 4x4 perspective projection matrix.
     *
     */
      const algebraica::mat4f& perspective_matrix() const;
      /*
     * ### Obtaining the multiplied matrix of perspective and view matrices
     *
     * Returns the multiplied matrix of **perspective and view** transformation matrices.
     *
     * @returns
     * {const algebraica::mat4f&} Returns the resulting 4x4 matrix of the projection and
     * view matrices multiplication.
     *
     */
      const algebraica::mat4f& perspective_view_matrix() const;
      /*
     * ### Obtaining the camera position
     *
     * This function returns a 3D vector with the camera position in X, Y, and Z. The
     * center is located in the target position.
     *
     * @returns
     * {const algebraica::vec3f&} 3D vector width camera position.
     *
     */
      const algebraica::vec3f& position() const;
      /*
     * ### Setting camera position
     *
     * Changes the position of the camera, note that is only the location of the camera,
     * the point that the camera is aiming will not be modified, the center of is located
     * at the **camera's target** position, the +X axis is oriented towards east.
     *
     * @parameters
     * {const float} x = Position in the X axis [meters].
     * {const float} y = Position in the Y axis [meters].
     * {const float} z = Position in the Z axis [meters].
     *
     */
      void position(const float x, const float y, const float z);
      /*
     * ### Obtaining the window and framebuffer resolution
     *
     * This function returns the window and framebuffer resolution in pixels, notice that, they
     * may be different, if so then, OpenGL will automatically scale the framebuffer content
     * to fit the window size
     *
     */
      const camera::Resolution& resolution() const;
      /*
     * ### Setting the window and framebuffer resolution
     *
     * This function sets the window and framebuffer resolution in pixels, notice that, you
     * could define them with different sizes, if so then, OpenGL will automatically scale
     * the framebuffer content to fit the window size.
     *
     * @parameters
     * {const int} window_width  = Window width [screen coordinates].
     * {const int} window_height = Window width [screen coordinates].
     * {const int} frame_width   = Framebuffer width [pixels].
     * {const int} frame_height  = Framebuffer width [pixels].
     *
     */
      void resolution(const int window_width, const int window_height,
                      const int frame_width, const int frame_height);
      /*
     * ### Rotating the camera
     *
     * Multiplies the Camera position by a orientation quaternion (the center is the
     * camera's target position), the angles are in **RADIANS**. See the
     * [[coordinate systems|Coordinate-systems#orientation-angles]] to view more
     * details about the angles.
     *
     * @parameters
     * {const algebraica::quaternionF&} rotation = Orientation quaternion.
     *
     */
      void rotate(const algebraica::quaternionF &rotation);
      void set_function_callback(boost::function<void()> callback_function);
      /*
     * ### Obtaining the non-translated multiplied matrix of perspective and view matrices
     *
     * Returns the **non-translated** multiplied matrix of **perspective and view**
     * transformation matrices. **Note:* this matrix only contains the **rotations** of
     * perspective_view_matrix and **not translations**.
     *
     * @returns
     * {const algebraica::mat4f&} Returns the resulting 4x4 matrix of the projection and
     * camera matrices multiplication WITHOUT translation (only rotation).
     *
     */
      const algebraica::mat4f& static_perspective_view_matrix() const;
      /*
     * ### Obtaining the camera's target position
     *
     * This function returns a 3D vector with the camera's target position in X, Y and Z. The
     * +X axis is oriented towards east and center is located at the GPS coordiantes (0°, 0°).
     *
     * @returns
     * {const algebraica::vec3f&} 3D vector width camera's target position.
     *
     */
      const algebraica::vec3f& target() const;
      /*
     * ### Setting camera's target position
     *
     * This specifies the point where the camera is looking. The +X axis is oriented
     * towards east and center is located at the GPS coordiantes (0°, 0°).
     *
     * @parameters
     * {const float} x = Position in the X axis [meters].
     * {const float} y = Position in the Y axis [meters].
     * {const float} z = Position in the Z axis [meters].
     *
     */
      void target(const float x, const float y, const float z);
      /*
     * ### Change the camera view matriy into a identity matrix
     *
     * Transform the camera position to the origin, the target now points to the X axis,
     * and the camera up vector is parallel to the Z axis.
     *
     */
      void to_identity();
      /*
     * ### Change view to top view
     *
     * Changes the view to **Top view**.
     *
     */
      void top_view();
      /*
     * ### Translating the camera
     *
     * Translates the target and camera position, if `parallel = true` then the translation
     * will follow the same orientation than the camera
     *
     * @parameters
     * {const float} x = Distance in X axis to translate [meters].
     * {const float} y = Distance in Y axis to translate [meters].
     * {const float} z = Distance in Z axis to translate [meters].
     * {const bool} parallel = If `true`then translation will follow the camera orientation.
     *
     */
      void translate(const float x = 0.0f, const float y = 0.0f, const float z = 0.0f,
                     const bool parallel = true);
      /*
     * ### Obtaining the camera's upwards direction
     *
     * This returns a 3D normalized vector that depics the camera orientation. Same
     * orientation than global frame.
     *
     * @returns
     * {const algebraica::vec3f&} 3D vector width camera's orientation vector.
     *
     */
      const algebraica::vec3f& up() const;
      /*
     * ### Setting camera's upwards direction
     *
     * Changes the rotation of the camera, specifically the upward side of the camera.
     * You must introduce the coordinates of a **normalized** vector, or set `normalize = true`
     * to normalize the arguments, this vector represents the camera orientation,
     * by default is pointing parallel to +Y axis.
     *
     * @parameters
     * {const float} x = Position in the X axis [meters], must be normalized.
     * {const float} y = Position in the Y axis [meters], must be normalized.
     * {const float} z = Position in the Z axis [meters], must be normalized.
     * {const bool} normalize = Indicates if the input arguments should be normalized.
     *
     */
      void up(const float x, const float y, const float z, const bool normalize = false);
      /*
     * ### Updates the view matrix
     *
     * Recalculates the view matrix (commonly called camera _look at_ matrix), is value will
     * change only when the camera's position, target or up vector is modified. Only use this
     * when manually adjusting those values.
     *
     */
      void update(const bool event = false);
      /*
     * ### Obtaining the view transformation matrix
     *
     * Returns the **view** transformation matrix, this matrix is calculated using
     * **look at** matrix transformation with the camera as parameter.
     *
     * @parameters
     * {const algebraica::mat4f} Returns the 4x4 transformation matrix of the camera.
     *
     */
      const algebraica::mat4f& view_matrix() const;
      /*
     * ### Obtaining the camera's "zoom"
     *
     * This function returns the distance between the camera position and its target
     *
     * @returns
     * {const float} Distance from position to target.
     *
     */
      float zoom() const;
      /*
     * ### Setting the camera's "zoom"
     *
     * This changes the distance of the camera to its target, closest = 0.1 meters
     * and furthest = 150 meters.
     *
     * @parameters
     * {const float} zoom = Distance from the Camera's position to its target
     *
     * @returns
     * {const bool} Returns TRUE if the distance was changed, FALSE if no change occurred or
     *              limits were reached
     *
     */
      bool zoom(const float distance);
      /*
     * ### Simple zoom in/out
     *
     * This changes the scenary zoom, if zooming in, the camera moves 10% closer to the target
     * position, if zooming out then, the camera moves 10% away from the target position
     *
     * @parameters
     * {const torero::Zoom} zooming = `torero::Zoom::In` indicates that camera should be closer
     *                                to the target position.
     * @returns
     * {const bool} Returns TRUE if the distance was changed, FALSE if limits were reached
     *
     */
      bool zoom(const torero::camera::Zoom zooming);

    private:
      void calculate_factors();
      void multiply_matrices();
      void update_perspective(const bool multiply = true);

      const torero::Pose3D *vehicle_;
      algebraica::mat4f view_matrix_, perspective_matrix_, pv_matrix_, spv_matrix_;

      algebraica::vec3f camera_position_, camera_target_, camera_up_, axis_z_;
      algebraica::quaternionF orientation_, rotation_;

      torero::camera::Resolution properties_;
      float rotational_factor_, translational_factor_;

      torero::gl::UniformBuffer uniform_buffer_;
      const GLsizei matrix_size_, vector_size_, buffer_size_;
      const GLint offset_projection_, offset_view_, offset_pv_, offset_spv_, offset_position_;

      bool fixed_view_, locked_camera_, blocked_camera_;
      boost::function<void()> callback_function_;
      bool is_callback_set_;
    };
  }
}
#endif // TORERO_CAMERA_VIEW_H
