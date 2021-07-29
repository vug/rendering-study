// Objects have a single/solid color
// but triangles have fixed (non-interpolting) normals, for lighting

#type vertex
#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in int a_EntityID;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec4 v_WorldPosition;
out flat int v_EntityID;

void main() {
    v_WorldPosition = u_Transform * vec4(a_Position, 1.0);
    v_EntityID = a_EntityID;
    gl_Position = u_ViewProjection * v_WorldPosition;
}


#type geometry
#version 460 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec4 v_WorldPosition[];
in flat int v_EntityID[];

out vec3 g_WorldPosition;
out vec3 g_Normal;
out flat int g_EntityID;

void main()
{
    int n;
    vec3 p0 = v_WorldPosition[0].xyz;
    vec3 p1 = v_WorldPosition[1].xyz;
    vec3 p2 = v_WorldPosition[2].xyz;
    vec3 normal = normalize(cross(p1 - p0, p2 - p0));

    for (n = 0; n < gl_in.length(); n++) {
        gl_Position = gl_in[n].gl_Position;
        g_WorldPosition = v_WorldPosition[n].xyz;
        g_Normal = normal;
        g_EntityID = v_EntityID[n];
        EmitVertex();
    }
    EndPrimitive();
}


#type fragment
#version 460 core

layout(location = 0) out vec4 color;
layout(location = 1) out int color2; // -1 no entity

in vec3 g_WorldPosition;
in vec3 g_Normal;
in flat int g_EntityID;

uniform vec4 u_Color;

#define MAX_LIGHTS 10
uniform int u_LightCount;
uniform vec3 u_LightPositions[MAX_LIGHTS];
uniform float u_LightIntensities[MAX_LIGHTS];

void main() {
    vec3 lightPos, lightDir;
    float flatShade = 0.0;

    for(int i = 0; i < MAX_LIGHTS; i++) {
        lightDir = normalize(u_LightPositions[i] - g_WorldPosition);
        flatShade += clamp(dot(lightDir, g_Normal) * u_LightIntensities[i], 0.0, 1.0);

        if (i == MAX_LIGHTS) break;
    }

    color = vec4(u_Color.rgb * flatShade, u_Color.a);
    color2 = g_EntityID;
}