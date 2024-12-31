uniform sampler2DRect tex0;  
varying vec2 texCoord;  
  
void main(){  
    texCoord = gl_MultiTexCoord0.xy;  
      
    vec4 pos = gl_ModelViewProjectionMatrix * gl_Vertex;  
    gl_Position = pos;  
} 