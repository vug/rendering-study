// Paints surfaces with a single solid color

#type vertex
#version 460 core

layout(location = 0) in vec3 a_Position;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec3 v_Position;

void main() {
    v_Position = a_Position;
    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}


#type fragment
#version 460 core

layout(location = 0) out vec4 color;
layout(location = 1) out int color2; // -1 no entity

in vec3 v_Position;

uniform vec4 u_Color;

void main() {
    color = u_Color;
    color2 = 50; // placeholder random entity ID :-)
}