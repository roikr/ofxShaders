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
void createDepthShader(ofShader &shader);
void createDepthMaskShader(ofShader &shader);
void createColor2GrayShader(ofShader &shader);
void createFastBlurShader(ofShader &shader,int radius,double variance);
void createBlurShader(ofShader &shader,int radius,double variance);
void createDepthBlurShader(ofShader &shader,int radius,double variance);
void createVarDepthBlurShader(ofShader &shader,int radius,double variance);
void createThresholdShader(ofShader &shader);
void createScreenShader(ofShader &shader);
void createScreenMultipleShader(ofShader &shader);
void createHSLShader(ofShader &shader);
void createEchoShader(ofShader &shader);
void createStrobeShader(ofShader &shader);
void createDepthBackgroundSubtractionShader(ofShader &shader);
void createMaskingShader(ofShader &shader);
void createCloudShader(ofShader &shader);
