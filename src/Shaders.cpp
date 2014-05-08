//
//  Shaders.cpp
//  depthBlur
//
//  Created by Roee Kremer on 4/18/14.
//
//

#include "Shaders.h"

#define STRINGIFY(A) #A

void createSimpleShader(ofShader &shader,string fragment) {
    shader.setupShaderFromSource(GL_VERTEX_SHADER, getSimpleVertex());
    shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragment);
    shader.bindDefaults();
    shader.linkProgram();
}

string getSimpleVertex() {
    return STRINGIFY(
                     \n#version 150\n
                     uniform mat4 modelViewProjectionMatrix;
                     in vec4 position;
                     in vec2 texcoord;
                     
                     out vec2 texCoordVarying;
                     
                     void main() {
                         texCoordVarying = texcoord;
                         gl_Position = modelViewProjectionMatrix * position;
                     }
                     );
}

void createDepthShader(ofShader &shader) {
    
    
    string fragment = STRINGIFY(
                                 \n#version 150\n
                                 uniform sampler2D tex0;
                                 uniform float minEdge;
                                 uniform float maxEdge;
                                 
                                 in vec2 texCoordVarying;
                                 out vec4 fragColor;
                                 
                                 void main(void) {
                                     float sample = texture(tex0,texCoordVarying).r;
                                     float dist = (sample-minEdge)/(maxEdge-minEdge);
                                     float color = (1-dist)*(step(minEdge,sample)-step(maxEdge,sample));
                                     fragColor = vec4(vec3(color),1.0);
                                     
                                 }
                                 
                                 );
    
    
    
    createSimpleShader(shader,fragment);
}


void createDepthMaskShader(ofShader &shader) {
    
    
    string fragment = STRINGIFY(
                                \n#version 150\n
                                uniform sampler2D tex0;
                                uniform float minEdge;
                                uniform float maxEdge;
                                
                                in vec2 texCoordVarying;
                                out vec4 fragColor;
                                
                                void main(void) {
                                    float sample = texture(tex0,texCoordVarying).r;
                                    float dist = (sample-minEdge)/(maxEdge-minEdge);
                                    float color = step(minEdge,sample)-step(maxEdge,sample);
                                    fragColor = vec4(vec3(color),1.0);
                                    
                                }
                                
                                );
    
    
    
    createSimpleShader(shader,fragment);
}


void createDepthBackgroundSubtractionShader(ofShader &shader) {
    string fragment = STRINGIFY(
                                \n#version 150\n
                                uniform sampler2D tex0;
                                uniform sampler2D bgTex;
                                uniform float tolerance;
                                
                                in vec2 texCoordVarying;
                                out vec4 fragColor;
                                
                                void main(void) {
                                    float c = texture(tex0,texCoordVarying).r;
                                    float bg = texture(bgTex,texCoordVarying).r;
                                    bool mask = abs(c-bg)>tolerance;
                                
                                    fragColor = vec4(vec3(mix(0,c,mask)),1.0);
                                    //fragColor = vec4(vec3(c-texture(bgTex,texCoordVarying).r),1.0);
                                    //fragColor = vec4(vec3(bg),1.0);
                                }
                                
                                );
    
    
    
    createSimpleShader(shader,fragment);

}

void createMaskingShader(ofShader &shader) {
    string fragment = STRINGIFY(
                                \n#version 150\n
                                uniform sampler2D tex0;
                                uniform sampler2D maskTex;
                                
                                
                                in vec2 texCoordVarying;
                                out vec4 fragColor;
                                
                                void main(void) {
                                    
                                    fragColor = vec4(texture(tex0,texCoordVarying).rgb,texture(maskTex,texCoordVarying).r);
                                    
                                }
                                
                                );
    
    
    
    createSimpleShader(shader,fragment);
}

void createColor2GrayShader(ofShader &shader) {
    
    string fragment = STRINGIFY(
                                \n#version 150\n
                                uniform sampler2D tex0;
                                
                                
                                in vec2 texCoordVarying;
                                out vec4 fragColor;
                                
                                
                                
                                void main(void) {
                                    fragColor = vec4(vec3(dot(texture(tex0,texCoordVarying).rgb,vec3(0.299, 0.587, 0.114))),1.0);
                                    
                                }
                                
                                );
    
    
    
    createSimpleShader(shader,fragment);
}

void createFastBlurShader(ofShader &shader,int radius,double variance) {
    
    vector<double> coefs;
    double sum =0;
    
    for (int i=0; i<radius*2+1; i++) {
        double x = ofMap(i, 0, radius - 1, -1, 1);
        double y = (1. / sqrt(TWO_PI * variance)) * exp(-(x * x) / (2 * variance));
        sum+=y;
        coefs.push_back(y);
    }
    
    for(vector<double>::iterator iter=coefs.begin();iter!=coefs.end();iter++) {
        *iter/=sum;
        
    }
    
    
    
    stringstream blurVert;
    blurVert <<  STRINGIFY(
                           \n#version 150\n
                           uniform mat4 modelViewProjectionMatrix;
                           uniform vec2 dir;
                           in vec4 position;
                           in vec2 texcoord;

                           );
    
    blurVert << "out vec2 texCoordVarying[" << radius*2+1 << "];";
    
    blurVert <<  STRINGIFY(
                           
                           void main()
                           
                           );
    
    blurVert << " { "  ;
    
    blurVert <<  STRINGIFY(
                           gl_Position = modelViewProjectionMatrix * position;
                           );
    
    for (int i=0;i<radius*2+1;i++) {
        blurVert << "texCoordVarying[" << i << "] = texcoord + " << i-radius << " * dir;" ;
    }
    
    blurVert << "}" ;
    
    
    
    
    stringstream blurFrag;
    blurFrag << STRINGIFY(
                          \n#version 150\n
                          uniform sampler2D tex0;
                          );
    
    blurFrag << "in vec2 texCoordVarying[" << radius*2+1 << "];";
    
    blurFrag <<  STRINGIFY(
                           
                           
                           
                           out vec4 fragColor;
                           
                           
                           void main(void)
                           );
    
    blurFrag << "{ fragColor = vec4(0.0);";
    
    vector<float> coef;
    
    for (int i=0; i<radius*2+1; i++) {
        blurFrag << "fragColor += texture(tex0, texCoordVarying[" << i << "])*" << coefs[i] << ";";
    }
    
    blurFrag << "}";
    
    shader.setupShaderFromSource(GL_VERTEX_SHADER, blurVert.str());
    shader.setupShaderFromSource(GL_FRAGMENT_SHADER, blurFrag.str());
    shader.bindDefaults();
    shader.linkProgram();
    
    
}

void createCoefficients(int radius,double variance,vector<double> &coefs) {
    
    double sum =0;
    
    for (int i=0; i<radius*2+1; i++) {
        double x = ofMap(i, 0, 2*radius , -1, 1);
        double y = (1. / sqrt(TWO_PI * variance)) * exp(-(x * x) / (2 * variance));
        sum+=y;
        coefs.push_back(y);
    }
    
    for(vector<double>::iterator iter=coefs.begin();iter!=coefs.end();iter++) {
        *iter/=sum;
        
    }
}

void createDepthBlurShader(ofShader &shader,int radius,double variance) {
    
    vector<double> coefs;
    createCoefficients(radius,variance,coefs);
    
    
    stringstream blurFrag;
    blurFrag << STRINGIFY(
                          \n#version 150\n
                          uniform sampler2D tex0;
                          in vec2 texCoordVarying;
                          uniform vec2 dir;
                          );
    
    
    blurFrag <<  STRINGIFY(
                           
                           
                           
                           out vec4 fragColor;
                           
                           
                           void main(void)
                           );
    
    blurFrag << "{ float color = 0.0;";
    
    
    for (int i=0; i<radius*2+1; i++) {
        blurFrag << "color += texture(tex0,texCoordVarying + " << i-radius << " * dir).r*" << coefs[i] << ";";
    }
    
    blurFrag << "fragColor=vec4(vec3(color),1.0);}";
    
//    cout << blurFrag.str() << endl;
    
    createSimpleShader(shader,blurFrag.str());
    
    
    
    
}

void createBlurShader(ofShader &shader,int radius,double variance) {
    
    vector<double> coefs;
    createCoefficients(radius,variance,coefs);
    
    
    stringstream blurFrag;
    blurFrag << STRINGIFY(
                          \n#version 150\n
                          uniform sampler2D tex0;
                          in vec2 texCoordVarying;
                          uniform vec2 dir;
                        
                          out vec4 fragColor;
                           
                           
                           void main(void)
                           );
    
    blurFrag << "{ fragColor = vec4(vec3(0.0),1.0);";
    
    for (int i=0; i<radius*2+1; i++) {
        blurFrag << "fragColor += vec4(texture(tex0,texCoordVarying + " << i-radius << " * dir).rgb*" << coefs[i] << ",1.0);";
    }
    blurFrag << "}";
    
    
    createSimpleShader(shader,blurFrag.str());
    
    
}


void createVarDepthBlurShader(ofShader &shader,int radius,double variance) {
    
    vector<double> coefs;
    double sum =0;
    
    for (int i=0; i<radius*2+1; i++) {
        double x = ofMap(i, 0, radius - 1, -1, 1);
        double y = (1. / sqrt(TWO_PI * variance)) * exp(-(x * x) / (2 * variance));
        sum+=y;
        coefs.push_back(y);
    }
    
    for(vector<double>::iterator iter=coefs.begin();iter!=coefs.end();iter++) {
        *iter/=sum;
        
    }
    
    
    
    stringstream blurFrag;
    blurFrag << STRINGIFY(
                          \n#version 150\n
                          uniform sampler2D tex0;
                          uniform sampler2D depthTex;
                          in vec2 texCoordVarying;
                          uniform vec2 dir;
                          uniform float scale;
                          uniform float offset;
                          
                          out vec4 fragColor;
                          
                          
                          void main(void)
                          );
    
    
    blurFrag << "{ fragColor = vec4(0.0);";
    blurFrag << "float dist = 1-texture(depthTex,texCoordVarying).r;";
    blurFrag << "float factor=min(step(offset,dist)*scale,1);"; // (dist-offset)*
    
    
    
    
    
    for (int i=0; i<radius*2+1; i++) {
        blurFrag << "fragColor += texture(tex0,texCoordVarying + " << i-radius << " * dir*factor)*" << coefs[i] << ";";
    }
    
    blurFrag << "}";
    
    createSimpleShader(shader,blurFrag.str());
    
    
}


void createThresholdShader(ofShader &shader) {
    
    string fragment = STRINGIFY(
                                \n#version 150\n
                                uniform sampler2D tex0;
                                uniform float edge0;
                                uniform float edge1;
                                
                                in vec2 texCoordVarying;
                                out vec4 fragColor;
                                
                               
                                
                                void main(void) {
                                    float c = texture(tex0,texCoordVarying).r;
                                    
                                    fragColor = vec4(vec3(smoothstep(edge0,edge1,c)*c),1.0);
//                                    fragColor = vec4(vec3(texture(tex0,texCoordVarying).r),1.0);
                                    
                                }
                                
                                );
    
    
    
    createSimpleShader(shader,fragment);

}

void createScreenShader(ofShader &shader) {
    
    
    string fragment = STRINGIFY(
                                \n#version 150\n
                                uniform sampler2D tex0;
                                uniform sampler2D tex1;
                                
                                
                                in vec2 texCoordVarying;
                                out vec4 fragColor;
                                
                                void main(void) {
                                    vec4 col0 = texture(tex0,texCoordVarying);
                                    vec4 col1 = texture(tex1,texCoordVarying);
                                    fragColor = 1-(1-col0)*(1-col1);
                                    
                                }
                                
                                );
    
    
    
    createSimpleShader(shader,fragment);
}

void createScreenMultipleShader(ofShader &shader) {
    string fragment = STRINGIFY(
                                \n#version 150\n
                                uniform sampler2D tex0;
                                uniform sampler2D tex1;
                                uniform sampler2D tex2;
                                uniform sampler2D tex3;
                                uniform sampler2D tex4;
                                //uniform sampler2D depthTex;
                                uniform int mask;
                                
                                
                                in vec2 texCoordVarying;
                                out vec4 fragColor;
                                
                                void main(void) {
                                    vec3 col0 = mix(vec3(0),texture(tex0,texCoordVarying).rgb,bvec3(mask & 1));
                                    vec3 col1 = mix(vec3(0),texture(tex1,texCoordVarying).rgb,bvec3(mask & 1<<1));
                                    vec3 col2 = mix(vec3(0),texture(tex2,texCoordVarying).rgb,bvec3(mask & 1<<2));
                                    vec3 col3 = mix(vec3(0),texture(tex3,texCoordVarying).rgb,bvec3(mask & 1<<3));
                                    vec3 col4 = mix(vec3(0),texture(tex4,texCoordVarying).rgb,bvec3(mask & 1<<4));
                                    //float gray = texture(depthTex,texCoordVarying).r;
                                    vec3 color = 1-(1-col0)*(1-col1)*(1-col2)*(1-col3)*(1-col4);
                                    //fragColor = vec4(mix(color,vec3(gray),bvec3(gray>0)),1.0);
                                    fragColor = vec4(color,1.0);
                                }
                                
                                );
    
    
    
    createSimpleShader(shader,fragment);
}

void createHSLShader(ofShader &shader) {
    
    
    string fragment = STRINGIFY(
                                \n#version 150\n
                                uniform sampler2D tex0;
                                uniform float hue;
                                uniform float sat;
                                uniform float offset;
                                
                                in vec2 texCoordVarying;
                                out vec4 fragColor;
                                
                                void main(void) {
                                    
                                    float l = texture(tex0,texCoordVarying).r+offset;
                                    
                                    float c= (1-abs(2*l-1))*sat;
                                    
                                    vec3 y;
                                    y.r = abs(hue * 6 - 3) - 1;
                                    y.g = 2 - abs(hue * 6 - 2);
                                    y.b = 2 - abs(hue * 6 - 4);
                                    fragColor = vec4((clamp(y,0,1)-0.5)*c+l,1.0);

                                    
                                }
                                
                                );
    
    
    
    createSimpleShader(shader,fragment);

}

void createEchoShader(ofShader &shader) {
    
    string fragment = STRINGIFY(
                                \n#version 150\n
                                uniform sampler2D tex0;
                                uniform sampler2D tex1;
                                uniform float alpha;
                                
                                
                                in vec2 texCoordVarying;
                                out vec4 fragColor;
                                
                                void main(void) {
                                    fragColor = vec4(mix(texture(tex0,texCoordVarying).rgb,texture(tex1,texCoordVarying).rgb,alpha),1.0);
                                    
                                }
                                
                                );
    
    
    
    createSimpleShader(shader,fragment);
}

void createStrobeShader(ofShader &shader) {
    string fragment = STRINGIFY(
                                \n#version 150\n
                                \n#extension GL_ARB_explicit_attrib_location : enable\n
                                uniform sampler2D tex0;
                                uniform sampler2D tex1;
                                uniform sampler2D tex2;
                                
                                uniform int frameNum;
                                uniform int strobeRate;
                                uniform float decay;
                                
                                in vec2 texCoordVarying;
                                layout (location = 1) out float fragColor;
                                layout (location = 0) out float fragHue;
                                
                                
                                
                                void main(void) {
                                    float c = texture(tex0,texCoordVarying).r;
                                    float s1 = texture(tex1,texCoordVarying).r;
                                    float s2 = texture(tex2,texCoordVarying).r;
                                    bool f = (frameNum % strobeRate) == 0;
                                    
                                    
                                    
                                    fragColor = mix(s1*decay,float(f)*c,f && c>0);
                                    fragHue = mix(s2,float(frameNum/10 % 256)/255.0,f && c>0);
                                    ;
                                    
                                }
                                
                                );
    
    
    
    createSimpleShader(shader,fragment);
}

void createCloudShader(ofShader &shader) {
    string vertex = STRINGIFY(
                              \n#version 150\n
                              uniform mat4 modelViewProjectionMatrix;
                              in vec4 position;
                              out float depth;
                              
                              void main() {
                                  
                                  gl_Position = modelViewProjectionMatrix * position;
                                  depth = gl_Position.z;
                              }
                              );


    string fragment = STRINGIFY(
                                \n#version 150\n
                                
                                uniform float minEdge;
                                uniform float maxEdge;
                                uniform float scale;   // should be 1/100000
                                
                                
                                in vec4 pos;
                                in float depth;
                                
                                out vec4 fragColor;
                                
                                void main(void) {
                                    
                                    float sample = depth*scale;
                                    float dist = (sample-minEdge)/(maxEdge-minEdge);
                                    float color = (1-dist)*(step(minEdge,sample)-step(maxEdge,sample));
                                    fragColor = vec4(vec3(color),1.0);
                                    
                                }
                                
                                );




    shader.setupShaderFromSource(GL_VERTEX_SHADER, vertex);
    shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragment);
    shader.bindDefaults();
    shader.linkProgram();
}