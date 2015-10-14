#include "display.hpp"

#include "util.hpp"
#include "resources.hpp"
#include "load_image.hpp"
#include "string.hpp"
#include "input.hpp"
#include "physics.hpp"
#include "game.hpp"

#include <rucksack/rucksack.h>
#include <SDL.h>
#include <SDL_ttf.h>

static const int window_width = 800;
static const int window_height = 600;
static const SDL_Color black      = {0x00, 0x00, 0x00, 0xff};
static const SDL_Color dark_green = {0x00, 0x88, 0x00, 0xff};
static const SDL_Color white      = {0xff, 0xff, 0xff, 0xff};

static SDL_Window * window;
static SDL_Texture * sprite_sheet_texture;
static SDL_Renderer * renderer;

static RuckSackBundle * bundle;
static RuckSackTexture * rs_texture;
static RuckSackImage ** spritesheet_images;

static RuckSackImage * man_stand_image;

static TTF_Font * font;
static unsigned char * font_buffer;
static SDL_RWops * font_rw_ops;
static String version_string = new_string();


static inline SDL_Point scale_to_screen(const Coord & coord) {
    return SDL_Point{(int)(coord.x / 1000), (int)(coord.y / 1000)};
}

static inline SDL_Point to_screen(const Coord & coord) {
    SDL_Point scaled = scale_to_screen(coord);
    SDL_Point camera_position = scale_to_screen(get_you()->bounds.position);
    return SDL_Point{scaled.x - camera_position.x + window_width/2, scaled.y - camera_position.y + window_height/2};
}
static inline SDL_Rect to_screen(const Rect & rect) {
    SDL_Point position = to_screen(rect.position);
    SDL_Point size = scale_to_screen(rect.size);
    SDL_Rect result;
    result.x = position.x;
    result.y = position.y;
    result.w = size.x;
    result.h = size.y;
    return result;
}

static RuckSackImage * find_image(RuckSackImage ** spritesheet_images, long image_count, const char * name) {
    for (int i = 0; i < image_count; i++)
        if (strcmp(spritesheet_images[i]->key, name) == 0)
            return spritesheet_images[i];
    panic("sprite not found");
}

static void load_images(RuckSackImage ** spritesheet_images, long image_count) {
    man_stand_image = find_image(spritesheet_images, image_count, "img/man_stand.png");
}

void display_init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        panic("unable to init SDL");

    window = SDL_CreateWindow("Prismoid", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, 0);
    if (window == nullptr)
        panic("window create failed");
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
        panic("renderer create failed");

    if (rucksack_bundle_open_read_mem(get_binary_resources_start(), get_binary_resources_size(), &bundle) != RuckSackErrorNone)
        panic("error opening resource bundle");
    RuckSackFileEntry * entry = rucksack_bundle_find_file(bundle, "spritesheet", -1);
    if (entry == nullptr)
        panic("spritesheet not found in bundle");

    if (rucksack_file_open_texture(entry, &rs_texture) != RuckSackErrorNone)
        panic("open texture failed");

    sprite_sheet_texture = load_texture(renderer, rs_texture);

    long image_count = rucksack_texture_image_count(rs_texture);
    spritesheet_images = allocate<RuckSackImage*>(image_count);
    rucksack_texture_get_images(rs_texture, spritesheet_images);
    load_images(spritesheet_images, image_count);

    TTF_Init();

    RuckSackFileEntry * font_entry = rucksack_bundle_find_file(bundle, "font/DejaVuSansMono.ttf", -1);
    if (font_entry == nullptr)
        panic("font not found in bundle");
    long font_file_size = rucksack_file_size(font_entry);
    font_buffer = allocate<unsigned char>(font_file_size);
    rucksack_file_read(font_entry, font_buffer);
    font_rw_ops = SDL_RWFromMem(font_buffer, font_file_size);
    if (font_rw_ops == nullptr)
        panic("sdl rwops fail");
    font = TTF_OpenFontRW(font_rw_ops, 0, 13);
    TTF_SetFontHinting(font, TTF_HINTING_LIGHT);

    RuckSackFileEntry * version_entry = rucksack_bundle_find_file(bundle, "version", -1);
    if (version_entry == nullptr)
        panic("version not found in bundle");
    ByteBuffer buffer;
    buffer.resize(rucksack_file_size(version_entry));
    rucksack_file_read(version_entry, (unsigned char *)buffer.raw());
    version_string->decode(buffer);
}

void display_finish() {
    TTF_Quit();

    SDL_RWclose(font_rw_ops);
    destroy(font_buffer, 0);

    destroy(spritesheet_images, 0);
    rucksack_texture_close(rs_texture);

    SDL_DestroyTexture(sprite_sheet_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    if (rucksack_bundle_close(bundle) != RuckSackErrorNone) {
        panic("error closing resource bundle");
    }
    SDL_Quit();
}

static int64_t last_display_time; // milliseconds
static int elapsed_time = 0; // will be 0 on first frame
void delay_until_next_frame() {
    int64_t now = SDL_GetTicks();
    int64_t sleep_time = (last_display_time + 17) - now; // 60Hz or whatever

    if (sleep_time > 0)
        SDL_Delay((Uint32)sleep_time);

    int64_t current_time = SDL_GetTicks();
    elapsed_time = (int)(current_time - last_display_time);
    last_display_time = current_time;
}
static String get_debug_string() {
    String result = new_string();
    result->append("fps: ");
    if (elapsed_time != 0) {
        int fps = 1000 / (int)elapsed_time;
        result->format("%d", fps);
    } else {
        result->append('?');
    }

    result->format("  input: %s%s%s%s",
        input_state[INPUT_UP]    ? "1" : "0",
        input_state[INPUT_LEFT]  ? "1" : "0",
        input_state[INPUT_DOWN]  ? "1" : "0",
        input_state[INPUT_RIGHT] ? "1" : "0");
    return result;
}

static void set_color(SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

static void render_sprite(RuckSackImage * sprite, int alpha, SDL_Point screen_location) {
    SDL_Rect source_rect;
    source_rect.x = sprite->x;
    source_rect.y = sprite->y;
    source_rect.w = sprite->width;
    source_rect.h = sprite->height;

    SDL_Rect dest_rect;
    dest_rect.x = screen_location.x;
    dest_rect.y = screen_location.y;
    dest_rect.w = sprite->width;
    dest_rect.h = sprite->height;

    SDL_SetTextureAlphaMod(sprite_sheet_texture, alpha);
    SDL_RenderCopyEx(renderer, sprite_sheet_texture, &source_rect, &dest_rect, 0.0, nullptr, SDL_FLIP_VERTICAL);
}
// {0, 0, w, h}
static inline SDL_Rect get_texture_bounds(SDL_Texture * texture) {
    SDL_Rect result = {0, 0, 0, 0};
    Uint32 format;
    int access;
    SDL_QueryTexture(texture, &format, &access, &result.w, &result.h);
    return result;
}
static void render_texture(SDL_Texture * texture, int x, int y) {
    SDL_Rect source_rect = get_texture_bounds(texture);
    SDL_Rect dest_rect;
    dest_rect.x = x;
    dest_rect.y = y;
    dest_rect.w = source_rect.w;
    dest_rect.h = source_rect.h;
    SDL_RenderCopyEx(renderer, texture, &source_rect, &dest_rect, 0.0, nullptr, SDL_FLIP_NONE);
}

static void render_text(String string, int x, int y) {
    ByteBuffer utf8;
    string->encode(&utf8);
    if (utf8.length() == 0) return;

    SDL_Surface * surface = TTF_RenderUTF8_Blended(font, utf8.raw(), white);
    SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    render_texture(texture, x, y);

    SDL_DestroyTexture(texture);
}

void render(List<Entity> * entities) {
    set_color(black);
    SDL_RenderClear(renderer);

    set_color(dark_green);
    for (int i = 0; i < entities->length(); i++) {
        Entity * entity = &(*entities)[i];
        if (entity->type == Entity::WALL) {
            SDL_Rect rect = to_screen(entity->bounds);
            SDL_RenderFillRect(renderer, &rect);
        } else if (entity->type == Entity::YOU) {
            render_sprite(man_stand_image, 0xff, to_screen(entity->bounds.position));
        }
    }

    render_text(version_string, 0, window_height - 17);
    render_text(get_debug_string(), 0, window_height - 17 * 2);

    SDL_RenderPresent(renderer);
}
