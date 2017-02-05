#version 120
uniform float time;

void main() {
    gl_TexCoord[0] = gl_MultiTexCoord0;
    vec4 pos = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;    
//    pos.x += (pos.x-256)*time;
//    pos.y += (pos.y-0.5)*time;
    float displacement = sin(time + (pos.x/100.0)) * 100.0;
    pos += displacement;
    
    gl_PointSize = 2.0;
    gl_Position = pos;
    gl_FrontColor = gl_Color;
}
