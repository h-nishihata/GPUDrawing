#version 120

#extension GL_ARB_texture_rectangle : enable
#extension GL_ARB_draw_buffers : enable

#pragma include "noise.frag"

uniform sampler2DRect u_posAndAgeTex;
uniform sampler2DRect u_velAndMaxAgeTex;
uniform float u_time;
uniform vec2  u_resolution;
uniform vec3  u_nodePos;

void main(void){
    vec2 st = gl_TexCoord[0].st;
    // 前の位置情報とパーティクル初期化からの経過時間を取得
    vec4 posAndAge = texture2DRect(u_posAndAgeTex,st);
    // 前の速度と生存期間を取得
    vec4 velAndMaxAge = texture2DRect(u_velAndMaxAgeTex,st);
    vec3 pos = posAndAge.xyz; // 前の位置
    vec3 vel = velAndMaxAge.xyz; // 前の速度
    vec2 resolution = u_resolution;
    vec3 nodePos = u_nodePos;
    float age = posAndAge.w; // 経過時間
    float maxAge = velAndMaxAge.w; // 生存期間
    vec3 init = vec3(0,0,0);
    
    
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

    pos.x += v.x * nodePos.x;
    pos.y += v.y * nodePos.y;
    pos.z += v.z * nodePos.z;
        
    vel = v;
    //　ひとまずBuffer 1（vel & maxAge）は使わないのでマップを表示
    
    gl_FragData[0].rgba = vec4(pos, age); // 位置と経過時間を出力
    gl_FragData[1].rgba = vec4(vel, maxAge); //速度と生存期間を出力
}
