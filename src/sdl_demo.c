#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>

#define ASSERT(_e, ...) if (!(_e)) { fprintf(stderr, __VA_ARGS__); exit(1); }

typedef float    f32;
typedef double   f64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef size_t   usize;
// typedef ssize_t  isize;

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

struct {
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Texture* texture;
  u32 pixels[SCREEN_WIDTH * SCREEN_HEIGHT];

  vec2 pos;
  vec2 dir;
  vec2 plane
} state;

void draw() {
  SDL_UpdateTexture(
      state.texture,
      NULL,
      state.pixels,
      SCREEN_WIDTH * sizeof(u32));

  SDL_RenderCopy(
      state.renderer,
      state.texture,
      NULL,
      NULL); 
}

int main(int argc, char** argv) {
  ASSERT(
      !SDL_Init(SDL_INIT_VIDEO),
      "SDL failed to initialize: %s\n",
      SDL_GetError());

  state.window = SDL_CreateWindow(
      "Wolfenstein 3D", 
      SDL_WINDOWPOS_UNDEFINED, 
      SDL_WINDOWPOS_UNDEFINED, 
      SCREEN_WIDTH, 
      SCREEN_HEIGHT,
      0);

  ASSERT(
      state.window,
      "Failed to create window: %s\n",
      SDL_GetError());


  state.renderer = SDL_CreateRenderer(
      state.window, 
      -1, 
      SDL_RENDERER_SOFTWARE);

  ASSERT(
      state.renderer,
      "Failed to create renderer: %s\n",
      SDL_GetError());

  state.texture = SDL_CreateTexture(
      state.renderer, 
      SDL_PIXELFORMAT_ARGB8888,
      SDL_TEXTUREACCESS_STREAMING,
      SCREEN_WIDTH,
      SCREEN_HEIGHT);

  ASSERT(
      state.texture,
      "Failed to create texture: %s\n",
      SDL_GetError());

  f32 height_map = (f32) 255 / SCREEN_HEIGHT;
  f32 width_map = (f32) 255 / SCREEN_WIDTH;
  for (u32 row = 0; row < SCREEN_HEIGHT; row++) {
    for (u32 col = 0; col < SCREEN_WIDTH; col++) {
      u32 r = (f32) row * height_map;
      // u32 r = 0;
      u32 g = ((f32) SCREEN_HEIGHT - row) * height_map;
      // u32 g = 0;
      u32 b = col * width_map;
      // u32 b = 0;
      u32 a = 0xFF000000;
      u32 color = 0x00010000 * r + 
                  0x00000100 * g + 
                  0x00000001 * b + a;

      state.pixels[row * SCREEN_WIDTH + col] = color;
    }
  }

  SDL_Event event;
  int quit = 0;
  while (!quit) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        quit = 1;
      }
		}    

    SDL_SetRenderDrawColor(state.renderer, 255, 0, 0, 0);
    SDL_RenderClear(state.renderer);

    draw();

    SDL_RenderPresent(state.renderer);
  }

  SDL_DestroyTexture(state.texture);
  SDL_DestroyRenderer(state.renderer);
  SDL_DestroyWindow(state.window);
  SDL_Quit();
  return 0;
}
