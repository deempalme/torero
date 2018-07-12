#ifndef TORERO_GROUND_H
#define TORERO_GROUND_H

#include "glad/glad.h"

#include "torero/buffer.h"
#include "torero/shader.h"
#include "torero/type_definitions.h"

#include "algebraica/algebraica.h"

#include <vector>

namespace torero {
  class Ground
  {
  public:
    Ground(Shader *ground_shader);

    void change_input(const std::vector<torero::Ground2D> *ground);
    void change_input(const std::vector<torero::Ground3D> *ground);
    void change_input(const std::vector<torero::GroundGrid> *ground);
    void change_input(const std::vector<torero::FreeGround2D> *ground);
    void change_input(const std::vector<torero::FreeGround3D> *ground);
    void change_input(const std::vector<torero::FreePolarGround2D> *ground);
    void change_input(const std::vector<torero::FreePolarGround3D> *ground);

    void set_transformation_matrix(const algebraica::mat4f *transformation_matrix);

    void calculate_height(const bool calculate = false, const float maximum_height = 1.0f);

    void set_ground_size(const float width = 100.0f,
                         const float length = 100.0f,
                         const unsigned int number_of_elements_through_width = 100u,
                         const unsigned int number_of_elements_through_length = 100u);

    void fog_visibility(const bool visible = true);

    void translate(const float x = 0.0f, const float y = 0.0f, const float z = 0.0f);
    void translate(const algebraica::vec3f translation);
    void rotate(const float pitch = 0.0f, const float yaw = 0.0f, const float roll = 0.0f);
    void rotate(const algebraica::vec3f rotation);
    void rotate_in_x(const float angle);
    void rotate_in_y(const float angle);
    void rotate_in_z(const float angle);

    bool update();
    bool draw();

  private:
    void initialize();
    void restart();

    Shader *shader_;
    Buffer buffer_;

    float width_, length_, element_width_, element_length_, maximum_height_;
    unsigned int quantity_width_, quantity_length_;
    bool calculate_height_;
    int fog_visibility_, is_free_, is_polar_, is_grid_, has_height_, is_2D_;
    algebraica::vec3f ground_position_;

    const std::vector<torero::Ground2D> *ground_2D_;
    const std::vector<torero::Ground3D> *ground_3D_;
    const std::vector<torero::GroundGrid> *ground_grid_;
    const std::vector<torero::FreeGround2D> *free_ground_2D_;
    const std::vector<torero::FreeGround3D> *free_ground_3D_;
    const std::vector<torero::FreePolarGround2D> *polar_ground_2D_;
    const std::vector<torero::FreePolarGround3D> *polar_ground_3D_;

    const algebraica::mat4f *primary_model_;
    algebraica::mat4f secondary_model_, identity_matrix_;

    GLsizei type_size_, data_size_;

    GLint i_position_, i_color_, i_dimension_, i_height_, i_probability_;
    GLint u_primary_model_, u_secondary_model_, u_fog_, u_height_, u_max_height_;
    GLint u_width_, u_length_, u_2D_, u_position_, u_free_, u_polar_, u_grid_;

    GLint size_1_, size_2_, size_3_, size_4_;
  };
}

#endif // TORERO_GROUND_H
