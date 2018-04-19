#version 420 core
//point cloud vertex shader

layout(location = 0) in vec3 i_position;
layout(location = 1) in float i_intensity;
layout(location = 2) in vec3 i_color;
layout(location = 3) in float i_alpha;

out vec4 o_color;

uniform mat4 u_pv;
uniform mat4 u_primary_model;
uniform mat4 u_secondary_model;

uniform vec3 u_palette[10];       //this is the color palette
uniform float u_color_size;       //this indicates the number of elements in the color palette
uniform float u_color_mode;       //this is to turn all clouds in grayscale
uniform float u_intensity_range;
uniform bool u_has_alpha;

void main()
{
  gl_Position = u_pv * u_primary_model * u_secondary_model *
      vec4(-i_position.y, i_position.z, -i_position.x, 1.0);

  if(u_color_mode == 0.0){
    o_color.xyz = vec3(1.0, 1.0, 1.0);
    o_color.a = (i_intensity / u_intensity_range) * 0.9 + 0.1;
  }else if(u_color_mode == 1.0){
    o_color.xyz = u_palette[0];
    o_color.a = 1.0;
  }else if(u_color_mode == 2.0){
    o_color.a = i_intensity / u_intensity_range;

    int idx1 = 0;// Our desired color will be between these two indexes in "color".
    int idx2 = 0;
    float fractBetween = 0.0;// Fraction between "idx1" and "idx2" where our value is.

    if(o_color.a <= 0.0){
      idx1 = idx2 = 0;
    }else if(o_color.a >= 1.0){
      idx1 = idx2 = int(u_color_size) - 1;
    }else{
      o_color.a = o_color.a * (u_color_size - 1.0);
      idx1 = int(floor(o_color.a));// Our desired color will be after this index.
      idx2 = idx1 + 1;// ... and before this index (inclusive).
      fractBetween = o_color.a - float(idx1);// Distance between the two indexes (0-1).
    }

    o_color.xyz = (u_palette[idx2] - u_palette[idx1]) * fractBetween + u_palette[idx1];
    o_color.a = 1.0;
  }else if(u_color_mode == 3.0){
    o_color.xyz = i_color;
    o_color.a = (u_has_alpha)? i_alpha : 1.0;
  }else{
    o_color = vec4(0.0);
  }
}
