uniform sampler2DRect tex0;  
uniform sampler2DRect noiseTexture;  
  
uniform vec2 textureSize;  
uniform vec2 noiseTextureSize;  
uniform int frameNum;  
uniform float vignetteSize;  
uniform float colorFilterAmount;  
uniform float noiseAmount;  
  
varying vec2 texCoord;  
  
const vec4 redfilter        = vec4(1.0, 0.0, 0.0, 1.0);  
const vec4 bluegreenfilter  = vec4(0.0, 1.0, 0.7, 1.0);  
  
void main(){      
    vec2 texC = texCoord;  
    float vignetteSizeSquared = vignetteSize * vignetteSize;  
    vec4 col = texture2DRect(tex0, texC);  
      
    // vignette  
    vec2 center = textureSize / 2.0;  
    vec2 d = center - vec2(texCoord);  
    float distSQ = d.x*d.x + d.y*d.y;  
    float invDistSQ = 1.0 / distSQ;  
      
    col.rgb -= distSQ / vignetteSizeSquared;  
    col = clamp(col, 0.0, 1.0);  
      
    // noise   
    vec2 noiseCoords = float(frameNum * 50) + texCoord;  
    noiseCoords = mod(noiseCoords, noiseTextureSize);  
    vec4 noiseColor = texture2DRect(noiseTexture, noiseCoords);  
    noiseColor = mix(noiseColor, col, noiseAmount);  
  
    // color filter  
    vec4 redrecord = noiseColor * redfilter;  
    vec4 bluegreenrecord = noiseColor * bluegreenfilter;  
      
    vec4 rednegative = vec4(redrecord.r);  
    vec4 bluegreennegative = vec4((bluegreenrecord.g + bluegreenrecord.b)/2.0);  
      
    vec4 redoutput = rednegative * redfilter;  
    vec4 bluegreenoutput = bluegreennegative * bluegreenfilter;  
    vec4 result = redoutput + bluegreenoutput;  
      
    result = mix(noiseColor, result, colorFilterAmount);  
    result.a = noiseColor.a;  
      
    gl_FragColor = result;  
} 