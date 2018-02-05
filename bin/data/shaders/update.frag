#version 120

#extension GL_ARB_texture_rectangle : enable
#extension GL_ARB_draw_buffers : enable

uniform sampler2DRect u_currPosTex;
uniform sampler2DRect u_currColTex;
uniform sampler2DRect u_velTex;
uniform sampler2DRect u_nextPosTex;
uniform sampler2DRect u_nextColTex;

uniform vec2  u_resolution;
uniform float  u_pixelR, u_pixelG, u_pixelB;
uniform float  u_time;
uniform int   u_overdose;

void main(void){
    vec2 st = gl_TexCoord[0].st;
    
    vec3 currPos  = texture2DRect(u_currPosTex,st).xyz;
    vec3 currCol = texture2DRect(u_currColTex, st).xyz;
    vec3 field  = texture2DRect(u_velTex,st).xyz;
    vec3 nextPos = texture2DRect(u_nextPosTex,st).xyz;
    vec3 nextCol = texture2DRect(u_nextColTex, st).xyz;
    
    float posMapAlpha = texture2DRect(u_currPosTex,st).w;
    float velMapAlpha = texture2DRect(u_currPosTex,st).w;
    
    vec2 resolution = u_resolution;
    float fieldRadius = 0.05 * resolution.x;
    vec3 agentPos;
    float lerpSpeed = 0.01;
    
    // stirring agent
    float centX = 540;
    float centY = 540;
    float ang1, ang2;
    float rad1 = 240;
    float rad2 = 160;
    
    
    // morphing mode
    if(u_overdose == 1){
        field = vec3(0, 0, 0);
        
        if(abs(nextPos.x - currPos.x)>0.1){ field.x = (nextPos.x - currPos.x)*0.05; }else{ field.x=0; }
        if(abs(nextPos.y - currPos.y)>0.1){ field.y = (nextPos.y - currPos.y)*0.05; }else{ field.y=0; }
        if(abs(nextPos.z - currPos.z)>0.1){ field.z = (nextPos.z - currPos.z)*0.05; }else{ field.z=0; }
        
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
    currPos += field*0.9;
    
    
    // stirring agent
    ang1 += sin(u_time * 0.1) * 30;
    ang2 += sin(u_time * 0.2) * 20;
    float rx = centX + (rad1 * cos(ang1));
    float ry = centY + (rad1 * sin(ang1));
    agentPos.x = rx + (rad2 * cos(ang2));
    agentPos.y = ry + (rad2 * sin(ang2));
    
    // get force from pos
    float xPos = gl_FragCoord.x;
    float yPos = gl_FragCoord.y;
    float zPos = gl_FragCoord.z; // clip space (0-1)
    
    // add vector circle
    float dist = sqrt((agentPos.x-xPos)*(agentPos.x-xPos) + (agentPos.y-yPos)*(agentPos.y-yPos) + (0-zPos)*(0-zPos));
    
    if (dist < 0.0001){
        dist = 0.0001;
    }
    if (dist < fieldRadius){
        float pct = 1.0f - (dist / fieldRadius);
        float strongness = 0.5 * pct;
        float unit_px = ( agentPos.x - xPos) / dist;
        float unit_py = ( agentPos.y - yPos) / dist;
        float unit_pz = ( agentPos.z - zPos) / dist;
        field.x += unit_px * strongness;
        field.y += unit_py * strongness;
        field.z += unit_pz * strongness * 5.0;
    }
    
    // add some colors
    if((currCol.r >= 0.1) && (currCol.g >= 0.1) && (currCol.b >= 0.1)){
        if(currCol.r < u_pixelR){
            if(currCol.r < 1.0){
                currCol.r += 0.1;
            }
        }else{
            if(currCol.r > 0){
                currCol.r -= 0.1;
            }
        }
        if(currCol.g < u_pixelG){
            if(currCol.g < 1.0){
                currCol.g += 0.1;
            }
        }else{
            if(currCol.g > 0){
                currCol.g -= 0.1;
            }
        }
        if(currCol.b < u_pixelB){
            if(currCol.b < 1.0){
                currCol.b += 0.1;
            }
        }else{
            if(currCol.b > 0){
                currCol.b -= 0.1;
            }
        }
    }
    
    gl_FragData[0].rgba = vec4(currPos, posMapAlpha);
    gl_FragData[1].rgba = vec4(currCol, 1.0);
    gl_FragData[2].rgba = vec4(field, velMapAlpha);
    gl_FragData[3].rgba = vec4(nextPos, 1.0);
    gl_FragData[4].rgba = vec4(nextCol, 1.0);
}
