#ifndef _TEXT_H
#define _TEXT_H

#include <common.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace draw {

    class Text {
    public:       
        Text();
        Text(std::string font, int font_size);
        ~Text();
        void draw(Program& prog, GLFWwindow& window, std::string display_txt);
    private:
	    GLuint tex;
	    GLuint vbo;

	    FT_Library ft;
    	FT_GlyphSlot g;
		FT_Face face;

		int pix_size;

		std::string header;
    	std::string font_name;
    	std::string txt;

		void renderText(const char *text, float x, float y, float sx, float sy);
    };

}
#endif