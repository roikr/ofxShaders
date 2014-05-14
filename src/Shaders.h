//
//  Shaders.h
//  depthBlur
//
//  Created by Roee Kremer on 4/18/14.
//
//

#pragma once

#include "ofMain.h"

string getSimpleVertex();
void createSimpleShader(ofShader &shader,string fragment);
void createDepthShader(ofShader &shader);
void createDepthMaskShader(ofShader &shader);
void createColor2GrayShader(ofShader &shader);
void createFastBlurShader(ofShader &shader,int radius,double variance);
void createBlurShader(ofShader &shader,int radius,double variance);
void createDepthBlurShader(ofShader &shader,int radius,double variance);
void createVarDepthBlurShader(ofShader &shader,int radius,double variance);
void createThresholdShader(ofShader &shader);
void createScreenShader(ofShader &shader);
void createBlendShader(ofShader &shader);
void createScreenMultipleShader(ofShader &shader);
void createHSLShader(ofShader &shader);
void createEchoShader(ofShader &shader);
void createStrobeShader(ofShader &shader);
void createDepthBackgroundSubtractionShader(ofShader &shader);
void createMaskingShader(ofShader &shader);
void createInverseMaskingShader(ofShader &shader);
void createCloudShader(ofShader &shader);

void createBorderShader(ofShader &shader);
void createDilationShader(ofShader &shader);
void createHalftoneShader(ofShader &shader);
void createKuwaharaShader(ofShader &shader);
void createKuwahara3Shader(ofShader &shader);

