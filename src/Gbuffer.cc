#include <Gbuffer.hpp>
#include <iostream>
#include <log.hpp>


bool Gbuffer::init(uint width, uint height)
{
    attachments[0] = GL_COLOR_ATTACHMENT0;
    attachments[1] = GL_COLOR_ATTACHMENT1;
    attachments[2] = GL_COLOR_ATTACHMENT2;
    attachments[3] = GL_COLOR_ATTACHMENT3;

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
    // glEnable(GL_TEXTURE_2D);

    // - Position color buffer
    glGenTextures(1, &gpos);
    LOG("GBUFFER POSITION TEXTURE: " << gpos);
    glBindTexture(GL_TEXTURE_2D, gpos);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gpos, 0);

    // - Normal color buffer
    glGenTextures(1, &gnor);
    glBindTexture(GL_TEXTURE_2D, gnor);
    LOG("GBUFFER POSITION TEXTURE: " << gnor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    gluBuild2DMipmaps(GL_TEXTURE_2D, GLU_RGB, width, height, GLU_RGB, GLU_FLOAT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gnor, 0);

    // - Color buffer
    glGenTextures(1, &gcol);
    glBindTexture(GL_TEXTURE_2D, gcol);
    LOG("GBUFFER POSITION TEXTURE: " << gcol);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gcol, 0);

#if 1
    // Specular buffer
    glGenTextures(1, &gspc);
    glBindTexture(GL_TEXTURE_2D, gspc);
    LOG("GBUFFER SPECULAR TEXTURE " << gspc);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, width, height, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gspc, 0);
#endif

    // - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    glDrawBuffers(4, attachments);

    // - Create and attach depth buffer (renderbuffer)
    glGenRenderbuffers(1, &zbuf);
    glBindRenderbuffer(GL_RENDERBUFFER, zbuf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, zbuf);

    // - Finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        ERROR("FRAMEBUFFER PROBLEM!!!");
        exit(1);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void Gbuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void Gbuffer::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

extern int special_texture_handle;

void Gbuffer::bindTextures()
{
//    LOG("RAWR " << gpos << " " << gnor << " "  << gcol << " " << gspc);
//    LOG("SPECIAL TEXTURE HANDLE " << special_texture_handle);
#if 1
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gpos);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gnor);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gcol);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, gspc);
#else
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, special_texture_handle);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, special_texture_handle);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, special_texture_handle);
#endif
}

void Gbuffer::unbindTextures()
{
}

void Gbuffer::copyDepthBuffer(uint width, uint height)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,
                      GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
