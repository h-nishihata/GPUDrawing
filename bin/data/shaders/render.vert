#version 120
#extension GL_EXT_gpu_shader4 : enable
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect u_currPosTex;
uniform sampler2DRect u_currColTex;

void main() {
    vec2 st = gl_MultiTexCoord0.xy;
    vec3 pos = texture2DRect( u_currPosTex, st ).xyz;
    
    gl_PointSize = 1.0;
    gl_Position = gl_ModelViewProjectionMatrix * vec4(pos, 1.0);
    
    vec3 col = texture2DRect(u_currColTex, st).xyz;
    gl_FrontColor = vec4(col, 1.0);
}
