#include <Gbuffer.hpp>
#include <iostream>
#include <log.hpp>


bool Gbuffer::init(uint width, uint height)
{
    attachments[0] = GL_COLOR_ATTACHMENT0;
    attachments[1] = GL_COLOR_ATTACHMENT1;
    attachments[2] = GL_COLOR_ATTACHMENT2;

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // - Position color buffer
    glGenTextures(1, &gpos);
    glBindTexture(GL_TEXTURE_2D, gpos);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gpos, 0);

    // - Normal color buffer
    glGenTextures(1, &gnor);
    glBindTexture(GL_TEXTURE_2D, gnor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gnor, 0);

    // - Color + Specular color buffer
    glGenTextures(1, &gcol);
    glBindTexture(GL_TEXTURE_2D, gcol);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gcol, 0);

    // - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    glDrawBuffers(3, attachments);

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
