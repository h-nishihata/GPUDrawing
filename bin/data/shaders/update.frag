#version 120
#extension GL_ARB_texture_rectangle : enable
#extension GL_ARB_draw_buffers : enable

#pragma include "noise.frag"

uniform sampler2DRect u_currPosTex;
uniform sampler2DRect u_currColTex;
uniform sampler2DRect u_velTex;
uniform sampler2DRect u_nextPosTex;
uniform sampler2DRect u_nextColTex;
uniform float u_time;
uniform vec2  u_resolution;
uniform vec3  u_nodePos;
uniform int   u_overdose;


void main(void){
    vec2 st = gl_TexCoord[0].st;
    
    vec4 position = texture2DRect(u_currPosTex,st);
    vec4 velocity = texture2DRect(u_velTex,st);
    vec4 nextPosition  = texture2DRect(u_nextPosTex,st);
    
    vec3 currPos  = position.xyz;
    vec3 vel  = velocity.xyz;
    vec3 nextPos = nextPosition.xyz;
    vec3 origin = vec3(0,0,0);
    
    vec3 currCol = texture2DRect(u_currColTex, st).xyz;
    vec3 nextCol = texture2DRect(u_nextColTex, st).xyz;
    float lerpSpeed = 0.01;
    
    float posMapAlpha = position.w;
    float velMapAlpha = velocity.w;
    
    vec2 resolution = u_resolution;
    vec3 nodePos = u_nodePos;
    int overdose = u_overdose;
    
    
    // generate noise map
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
        // normal mode
        vel.x = v.x;
        vel.y = v.y;
        vel.z = v.z;
    }else if(overdose == 1){
        // morphing mode
        if(abs(nextPos.x - currPos.x)>0.1){ vel.x = (nextPos.x - currPos.x)*0.05; }else{ vel.x=0; }
        if(abs(nextPos.y - currPos.y)>0.1){ vel.y = (nextPos.y - currPos.y)*0.05; }else{ vel.y=0; }
        if(abs(nextPos.z - currPos.z)>0.1){ vel.z = (nextPos.z - currPos.z)*0.05; }else{ vel.z=0; }
        // lerp colors
        if(currCol.r < nextCol.r){
            currCol.r += lerpSpeed;
        }else{
            currCol.r -= lerpSpeed;
        }
        if(currCol.g < nextCol.g){
            currCol.g += lerpSpeed;
        }else{
            currCol.g -= lerpSpeed;
        }
        if(currCol.b < nextCol.b){
            currCol.b += lerpSpeed;
        }else{
            currCol.b -= lerpSpeed;
        }
    }
    
    // stay in the original point
    origin -= vec3(0.3, 0.3, 0.3);
    currPos = origin + currPos + vel;
    
    gl_FragData[0].rgba = vec4(currPos, posMapAlpha);
    gl_FragData[1].rgba = vec4(currCol, 1.0);
    gl_FragData[2].rgba = vec4(vel,     velMapAlpha);
    gl_FragData[3].rgba = vec4(nextPos, 1.0);
    gl_FragData[4].rgba = vec4(nextCol, 1.0);
}
