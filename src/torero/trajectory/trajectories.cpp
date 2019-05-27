#include "torero/trajectory/trajectories.h"

namespace torero {
  namespace trajectory {
    Trajectories::Trajectories(torero::gl::Shader *shader_program,
                               const std::vector<torero::trajectory::Trajectory> *trajectories) :
      shader_(shader_program),
      buffer_(true),
      trajectories_(trajectories),
      primary_model_(nullptr),
      secondary_model_(),
      identity_matrix_(),
      type_size_(sizeof(torero::trajectory::Shader)),
      data_size_(0)
    {
      initialize();
    }

    void Trajectories::change_input(const std::vector<torero::trajectory::Trajectory> *trajectories){
      trajectories_ = trajectories;
    }

    void Trajectories::set_transformation_matrix(const algebraica::mat4f *transformation_matrix){
      primary_model_ = transformation_matrix;
    }

    void Trajectories::translate(const float x, const float y, const float z){
      secondary_model_.translate(x, y, z);
    }

    void Trajectories::rotate(const float pitch, const float yaw, const float roll){
      secondary_model_.rotate(pitch, yaw, roll);
    }

    void Trajectories::rotate(const algebraica::vec3f rotation){
      secondary_model_.rotate(rotation.x, rotation.y, rotation.z);
    }

    void Trajectories::rotate_in_x(const float angle){
      secondary_model_.rotate_z(angle);
    }

    void Trajectories::rotate_in_y(const float angle){
      secondary_model_.rotate_x(angle);
    }

    void Trajectories::rotate_in_z(const float angle){
      secondary_model_.rotate_y(angle);
    }

    bool Trajectories::update(){
      bool no_error{shader_->use()};

      if(no_error){
        size_t size{0};
        for(const torero::trajectory::Trajectory &line : *trajectories_)
          if(line.size() > 1)
            size += line.size();

        std::vector<torero::trajectory::Shader> trajectories(0);
        trajectories.reserve(size + trajectories_->size() * 2);
        torero::trajectory::Shader first, second;
        size_t total{0}, last{0}, counter{0};

        for(const torero::trajectory::Trajectory &line : *trajectories_){
          if((total = line.size()) > 1){
            last = total - 1;
            counter = -1;

            std::vector<torero::trajectory::Shader> sub_trajectory(total + 3);

            // Creating the GL_LINE_STRIP_ADJACENCY
            // Example with a line containing 6 vertices:
            // A A–B-C-D-E–F F
            for(size_t i = 0; i < total; i++){
              second.color(line[i].color.rgba.red, line[i].color.rgba.green,
                           line[i].color.rgba.blue, line[i].color.rgba.alpha);
              second.line_width = line[i].line_width;
              second.position(-line[i].position.point.y, line[i].position.point.z,
                              -line[i].position.point.x);
              second.angle = line[i].angle;
              if(i == 0){
                second.distance = 0;
                sub_trajectory[++counter] = second;
              }else if(i == last){
                second.distance += algebraica::vec3f::distance(second.position, first.position);
                sub_trajectory[++counter] = second;
                second.position[2] += 0.1f;
                sub_trajectory[++counter] = second;
              }else{
                second.distance += algebraica::vec3f::distance(second.position, first.position);
              }
              sub_trajectory[++counter] = second;
              first = second;
            }
            trajectories.insert(trajectories.end(), sub_trajectory.begin(), sub_trajectory.end());
          }
        }

        buffer_.vertex_bind();
        data_size_ = trajectories.size();
        buffer_.allocate_array(trajectories.data(),
                               data_size_ * type_size_, GL_DYNAMIC_DRAW);

        GLint offset{0};
        buffer_.enable(i_position_);
        buffer_.attributte_buffer(i_position_, _3D, 0, type_size_);

        offset += sizeof(algebraica::vec3f);
        buffer_.enable(i_color_);
        buffer_.attributte_buffer(i_color_, _4D, offset, type_size_);

        offset += sizeof(algebraica::vec4f);
        buffer_.enable(i_line_width_);
        buffer_.attributte_buffer(i_line_width_, _1D, offset, type_size_);

        offset += sizeof(float);
        buffer_.enable(i_distance_);
        buffer_.attributte_buffer(i_distance_, _1D, offset, type_size_);

        offset += sizeof(float);
        buffer_.enable(i_angle_);
        buffer_.attributte_buffer(i_angle_, _1D, offset, type_size_);

        buffer_.vertex_release();
      }
      return no_error;
    }

    bool Trajectories::draw(){
      const bool no_error{shader_->use()};

      if(no_error){
        if(primary_model_)
          shader_->set_value(u_primary_model_, *primary_model_);
        else
          shader_->set_value(u_primary_model_, identity_matrix_);

        shader_->set_value(u_secondary_model_, secondary_model_);

        buffer_.vertex_bind();
        glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, data_size_);
        buffer_.vertex_release();
      }
      return no_error;
    }

    void Trajectories::initialize(){
      shader_->use();
      // GLSL attribute locations
      i_position_        = shader_->attribute_location("i_position");
      i_color_           = shader_->attribute_location("i_color");
      i_line_width_      = shader_->attribute_location("i_line_width");
      i_distance_        = shader_->attribute_location("i_distance");
      i_angle_          = shader_->attribute_location("i_angle");
      // GLSL uniform locations
      u_primary_model_   = shader_->uniform_location("u_primary_model");
      u_secondary_model_ = shader_->uniform_location("u_secondary_model");

      update();
    }
  }
}
