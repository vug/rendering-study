// Objects have a single/solid color
// but triangles have fixed (non-interpolting) normals, for lighting

#type vertex
#version 460 core

layout(location = 0) in vec3 a_Position;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec4 v_WorldPosition;

void main() {
    v_WorldPosition = u_Transform * vec4(a_Position, 1.0);
    gl_Position = u_ViewProjection * v_WorldPosition;
}


#type geometry
#version 460 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec4 v_WorldPosition[];
out float g_FlatShade;

void main()
{
    int n;
    vec3 p0 = v_WorldPosition[0].xyz;
    vec3 p1 = v_WorldPosition[1].xyz;
    vec3 p2 = v_WorldPosition[2].xyz;
    vec3 normal = normalize(cross(p1 - p0, p2 - p0));
    vec3 center = (p0 + p1 + p2) / 3.0;
    vec3 lightPos = vec3(2.0, 4.0, 0.0);
    vec3 lightDir = normalize(lightPos - center);
    float flatShade = dot(lightDir, normal);

    for (n = 0; n < gl_in.length(); n++) {
        gl_Position = gl_in[n].gl_Position;
        g_FlatShade = flatShade;
        EmitVertex();
    }
    EndPrimitive();
}


#type fragment
#version 460 core

layout(location = 0) out vec4 color;
layout(location = 1) out int color2; // -1 no entity

in float g_FlatShade;

uniform vec4 u_Color;

void main() {
    color = vec4(u_Color.rgb * g_FlatShade, u_Color.a);
    color2 = 50; // placeholder random entity ID :-)
}