#include <Gbuffer.hpp>
#include <iostream>
#include <log.hpp>
#include <errors.hpp>


bool Gbuffer::init(uint width, uint height)
{
    attachments[0] = GL_COLOR_ATTACHMENT0;
    attachments[1] = GL_COLOR_ATTACHMENT1;
    attachments[2] = GL_COLOR_ATTACHMENT2;
    attachments[3] = GL_COLOR_ATTACHMENT3;
    // Skipping GL_COLOR_ATTACHMENT4 because that's for the final buffer
    attachments[4] = GL_COLOR_ATTACHMENT5;
    attachments[5] = GL_COLOR_ATTACHMENT6;


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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
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
    LOG("GBUFFER SPECULAR TEXTURE: " << gspc);
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
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH32F_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, zbuf);

    // Final buffer
    glGenTextures(1, &gfinal);
    glBindTexture(GL_TEXTURE_2D, gfinal);
    LOG("GBUFFER FINAL TEXTURE: " << gfinal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, gfinal, 0);

    // View space position and depth
    glGenTextures(1, &gvposd);
    glBindTexture(GL_TEXTURE_2D, gvposd);
    LOG("GBUFFER VIEW SPACE POSITION AND DEPTH TEXTURE: " << gvposd);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, gvposd, 0);

    // View space normals
    glGenTextures(1, &gvnor);
    glBindTexture(GL_TEXTURE_2D, gvnor);
    LOG("GBUFFER VIEW SPACE NORMALS TEXTURE: " << gvnor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_2D, gvnor, 0);

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
    glDrawBuffers(num_attachments, attachments);
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
/*
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, gvposd);
*/
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
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        ERROR("FRAMEBUFFER PROBLEM!!!");
        exit(1);
    }
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    CHECK_GL_ERRORS();
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        ERROR("FRAMEBUFFER PROBLEM!!!");
        exit(1);
    }
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    CHECK_GL_ERRORS();
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,
                      GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    CHECK_GL_ERRORS();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    CHECK_GL_ERRORS();
}

void Gbuffer::startFrame()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glDrawBuffer(GL_COLOR_ATTACHMENT4);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Gbuffer::bindFinalBuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glDrawBuffer(GL_COLOR_ATTACHMENT4);
    int x;
    glGetIntegerv(GL_DRAW_BUFFER, &x);
    if (x != GL_COLOR_ATTACHMENT4) {
        LOG("GL_DRAW_BUFFER not correctly set! " << x);
    }
}

void Gbuffer::unbindFinalBuffer()
{
    glDrawBuffer(GL_NONE);
}

void Gbuffer::copyFinalBuffer(uint width, uint height)
{
/*
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glReadBuffer(GL_COLOR_ATTACHMENT1);
    glDrawBuffer(GL_COLOR_ATTACHMENT4);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);
*/
    CHECK_GL_ERRORS();
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glReadBuffer(GL_COLOR_ATTACHMENT2);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);
    CHECK_GL_ERRORS();
}
