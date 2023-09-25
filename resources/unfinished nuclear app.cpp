#define NK_IMPLEMENTATION
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT

#include "nuklear.h"
#include <stdlib.h>

float your_text_width_calculation(nk_handle handle, float height, const char* text, int len)
{
    float text_width = len * height;
    return text_width;
}
void query_your_font_glyph(nk_handle handle, float font_height, struct nk_user_font_glyph* glyph, nk_rune codepoint, nk_rune next_codepoint)
{
    glyph->width = 15.0f;
    glyph->height = 20.0f;
    glyph->offset.x = 0.0f;
    glyph->offset.y = 0.0f;
    glyph->xadvance = 15.0f;
    glyph->uv[0].x = 0.0f;
    glyph->uv[0].y = 0.0f;
    glyph->uv[1].x = 0.0f;
    glyph->uv[1].y = 0.0f;

}
struct your_font_class {
    // Your font-specific members
};

int main() {
    struct nk_context ctx;

    // Initialize your font

    struct nk_user_font font;
    struct your_font_class font_instance;
    font.userdata.ptr = &font_instance;
    font.height = 16;
    font.width = your_text_width_calculation;
    font.query = query_your_font_glyph;
    font.texture.id = 0;

    nk_init_fixed(&ctx, calloc(1, 20000000), 20000000, &font);

    enum { EASY, HARD };
    static int op = EASY;
    static float value = 0.6f;
    static int i = 20;

    if (nk_begin(&ctx, "Show", nk_rect(50, 50, 220, 220),
        NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_CLOSABLE)) {
        /* fixed widget pixel width */
        nk_layout_row_static(&ctx, 30, 80, 1);
        if (nk_button_label(&ctx, "button")) {
            /* event handling */
        }

        /* fixed widget window ratio width */
        nk_layout_row_dynamic(&ctx, 30, 2);
        if (nk_option_label(&ctx, "easy", op == EASY)) op = EASY;
        if (nk_option_label(&ctx, "hard", op == HARD)) op = HARD;

        /* custom widget pixel width */
        nk_layout_row_begin(&ctx, NK_STATIC, 30, 2);
        {
            nk_layout_row_push(&ctx, 50);
            nk_label(&ctx, "Volume:", NK_TEXT_LEFT);
            nk_layout_row_push(&ctx, 110);
            nk_slider_float(&ctx, 0, &value, 1.0f, 0.1f);
        }
        nk_layout_row_end(&ctx);
    }
    nk_end(&ctx);

    return 0;
}
