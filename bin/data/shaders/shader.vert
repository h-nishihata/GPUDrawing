#version 120
#extension GL_EXT_gpu_shader4 : enable
#extension GL_ARB_texture_rectangle : enable

#pragma include "util.frag"

uniform sampler2DRect u_posAndAgeTex;
//uniform float time;

void main() {
    vec2 st = gl_MultiTexCoord0.xy;
    vec3 pos = texture2DRect( u_posAndAgeTex, st ).xyz;
    gl_PointSize = 2.0;
    gl_Position = gl_ModelViewProjectionMatrix * vec4(pos, 1.0);
    /*
    gl_TexCoord[0] = gl_MultiTexCoord0;
    vec4 pos = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
    float displacement = sin(time + (pos.x/100.0)) * 100.0;
    pos += displacement;
    
    gl_PointSize = 2.0;
    gl_Position = pos;
    */
    gl_FrontColor = gl_Color;
}
