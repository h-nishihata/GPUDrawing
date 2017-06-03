#version 120

#extension GL_ARB_texture_rectangle : enable
#extension GL_ARB_draw_buffers : enable

#pragma include "noise.frag"

uniform sampler2DRect u_posTex;
uniform sampler2DRect u_velTex;
uniform sampler2DRect u_nextPosTex;
uniform float u_time;
uniform vec2  u_resolution;
uniform vec3  u_nodePos;
uniform int   u_overdose;

void main(void){
    vec2 st = gl_TexCoord[0].st;
    
    vec4 position = texture2DRect(u_posTex,st);
    vec4 velocity = texture2DRect(u_velTex,st);
    vec4 nextPos  = texture2DRect(u_nextPosTex,st);
    
    vec3 pos  = position.xyz;
    vec3 vel  = velocity.xyz;
    vec3 next = nextPos.xyz;
    
    vec2 resolution = u_resolution;
    vec3 nodePos = u_nodePos;
    float lifeTime = nextPos.w;
    int overdose = u_overdose;
    
    float posMapAlpha = position.w;
    float velMapAlpha = velocity.w;
    
    vec3 origin = vec3(0,0,0);

    
    vec2 p = (gl_FragCoord.xy * 2.0 - resolution.xy) / min(resolution.x, resolution.y);
    float t = 0.0, d;
    float time2 = u_time / 2.0;
    
    vec2 q = vec2(0.0);
    q.x = fbm(p + 0.00 * time2);
    q.y = fbm(p + vec2(1.0));
    vec2 r = vec2(0.0);
    r.x = fbm(p + 1.0 * q + vec2(1.7, 9.2) + 0.15 * time2);
    r.y = fbm(p + 1.0 * q + vec2(8.3, 2.8) + 0.126 * time2);
    float f = fbm(p + r);
    vec3 v = vec3(f);
    
    
    if(overdose == 0){
        vel.x = v.x;
        vel.y = v.y;
        vel.z = v.z;
    }else if(overdose == 1){
        if(abs(next.x - pos.x)>0.1){ vel.x = (next.x - pos.x)*0.1; }else{ vel.x=0; }
        if(abs(next.y - pos.y)>0.1){ vel.y = (next.y - pos.y)*0.1; }else{ vel.y=0; }
        if(abs(next.z - pos.z)>0.1){ vel.z = (next.z - pos.z)*0.1; }else{ vel.z=0; }
    }
    
    origin -= vec3(0.3, 0.3, 0.3);
    pos = origin + pos + vel;
    
    gl_FragData[0].rgba = vec4(pos,  posMapAlpha);
    gl_FragData[1].rgba = vec4(vel,  velMapAlpha);
    gl_FragData[2].rgba = vec4(next, lifeTime);
}
