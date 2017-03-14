#include "FrameBuffer.h"

namespace gigapoint {

const int FrameBuffer::RT_SIZE_DEFAULT = 1024;

FrameBuffer::FrameBuffer(bool storeDepth)
{
    uid = 0;
    hasDepth = storeDepth;
    width = FrameBuffer::RT_SIZE_DEFAULT;
    height = FrameBuffer::RT_SIZE_DEFAULT;
}

FrameBuffer::FrameBuffer(vector<string> targets, unsigned int _width, unsigned int _height, bool storeDepth)
{
    uid = 0;
    hasDepth = storeDepth;
    texturesNames = targets;
    width = _width;
    height = _height;
}

FrameBuffer::~FrameBuffer()
{
    for(int i=0; i<textures.size(); i++)
        delete textures[i];
    textures.clear();

    delete drawBuffers;
}

void FrameBuffer::clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FrameBuffer::init(int textureUnitOffset, bool linearFilter)
{
    glGenFramebuffers(1, &uid);
    glBindFramebuffer(GL_FRAMEBUFFER, uid);

    int bcount = texturesNames.size() + (hasDepth ? 1 : 0);
    if(bcount <= 0)
    {
        return;
    }
    else
    {
        size = bcount;
    }

    drawBuffers = new unsigned int[bcount];
    bcount = 0;

    if(hasDepth)
    {
        glGenRenderbuffers(1, &depthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    }

    if(texturesNames.size() > 0)
    {
        ColorTexture::resetUnit(textureUnitOffset);
        for(int i = 0; i < texturesNames.size(); i++)
        {
            ColorTexture* tex = ColorTexture::newFromNextUnit(width, height, GL_RGBA32F);
            tex->bind();

            unsigned int attachment = getAttachementFromIndex(i);

            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, tex->gluid, 0);

            textures.push_back(tex);
            drawBuffers[bcount++] = attachment;
            tex->unbind();
        }
    }

    //glDrawBuffers(size, drawBuffers);
    unbind();
}

void FrameBuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, uid);
    //glDrawBuffers(size, drawBuffers);
}

void FrameBuffer::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::resize(unsigned int _width, unsigned int _height)
{
    width = _width;
    height = _height;

    if(hasDepth)
    {
        glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    }

    for(int i=0; i<textures.size(); i++)
    {
        ColorTexture* tex = textures[i];
        tex->resize(width, height);
    }
}

void FrameBuffer::resizeViewport()
{
    glViewport(0, 0, width, height);
}

void FrameBuffer::bindAndTransmitTextures(Shader *shader)
{
    for(int i=0; i<textures.size(); i++)
    {
        ColorTexture* tex = textures[i];
        tex->bind();
        shader->transmitUniform(texturesNames.at(i), (int)tex->index);
    }
}

unsigned int FrameBuffer::getAttachementFromIndex(unsigned int index)
{
    switch(index)
    {
        case 1: return GL_COLOR_ATTACHMENT1;
        case 2: return GL_COLOR_ATTACHMENT2;
        case 3: return GL_COLOR_ATTACHMENT3;
        case 4: return GL_COLOR_ATTACHMENT4;
        case 5: return GL_COLOR_ATTACHMENT5;
        case 6: return GL_COLOR_ATTACHMENT6;
        case 7: return GL_COLOR_ATTACHMENT7;
        case 8: return GL_COLOR_ATTACHMENT8;
        case 9: return GL_COLOR_ATTACHMENT9;
        default: return GL_COLOR_ATTACHMENT0;
    }
}

ColorTexture* FrameBuffer::getTexture(string name)
{
    for(int i=0; i<texturesNames.size(); i++)
    {
        if(name.compare(texturesNames[i]) == 0)
            return textures[i];
    }
}

unsigned int FrameBuffer::getWidth()
{
    return width;
}

unsigned int FrameBuffer::getHeight()
{
    return height;
}

}; // namespace gigapoint
