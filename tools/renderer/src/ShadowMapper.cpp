#include "ShadowMapper.h"
#include <glad/glad.h>
#include <iostream>
static unsigned int depthFBO = 0;
static unsigned int depthTex = 0;
static int shadowSize = 1024;
namespace CoreTech {
void ShadowMapper::init(int size){
    shadowSize = size;
    if(depthFBO) return;
    glGenFramebuffers(1, &depthFBO);
    glGenTextures(1, &depthTex);
    glBindTexture(GL_TEXTURE_2D, depthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, shadowSize, shadowSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float border[4]={1,1,1,1}; glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
    glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);
    glDrawBuffer(GL_NONE); glReadBuffer(GL_NONE);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        std::cerr<<"Shadow FBO incomplete\\n";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    std::cout<<"ShadowMapper initialized with size "<<shadowSize<<"\\n";
}
void ShadowMapper::beginShadowPass(){
    glViewport(0,0,shadowSize,shadowSize);
    glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
}
void ShadowMapper::endShadowPass(){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
unsigned int getShadowDepthTexture(){ return depthTex; }
}