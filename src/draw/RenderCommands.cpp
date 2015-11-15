#include <draw/RenderCommands.hpp>
#include <log.hpp>

/* Many thanks to learnopengl.com */

GLfloat quadVertices[] = {
    // Positions        // Texture Coords
    -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
};

namespace draw {

  

    void Quad::GenerateData(int h_vert, int h_uv)
    {
        LOG("h_vert " << h_vert << " h_uv " << h_uv);
        // Setup plane VAO
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(h_vert);
        glVertexAttribPointer(h_vert, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(h_uv);
        glVertexAttribPointer(h_uv, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 
                              (GLvoid*)(3 * sizeof(GLfloat)));
        LOG("end of GenerateData");
    }

    void Quad::Render()
    {
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
    }

    void Sphere::Render()
    {
    }

};
