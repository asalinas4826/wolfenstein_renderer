#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <SDL.h>

#include "dyn_array.h"

#define ASSERT(_e, ...) if (!(_e)) { fprintf(stderr, __VA_ARGS__); exit(1); }
#define ABS(x) ((x < 0) ? x * -1 : x)

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

#define MAP_WIDTH 24
#define MAP_HEIGHT 24
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define TEXTURE_WIDTH 64
#define TEXTURE_HEIGHT 64
#define TEXTURE_COUNT 8

u32 world_map[MAP_WIDTH][MAP_HEIGHT]= {
  {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,7,7,7,7,7,7,7,7},
  {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,7},
  {4,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7},
  {4,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7},
  {4,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,7},
  {4,0,4,0,0,0,0,5,5,5,5,5,5,5,5,5,7,7,0,7,7,7,7,7},
  {4,0,5,0,0,0,0,5,0,5,0,5,0,5,0,5,7,0,0,0,7,7,7,1},
  {4,0,6,0,0,0,0,5,0,0,0,0,0,0,0,5,7,0,0,0,0,0,0,8},
  {4,0,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,7,7,1},
  {4,0,8,0,0,0,0,5,0,0,0,0,0,0,0,5,7,0,0,0,0,0,0,8},
  {4,0,0,0,0,0,0,5,0,0,0,0,0,0,0,5,7,0,0,0,7,7,7,1},
  {4,0,0,0,0,0,0,5,5,5,5,0,5,5,5,5,7,7,7,7,7,7,7,1},
  {6,6,6,6,6,6,6,6,6,6,6,0,6,6,6,6,6,6,6,6,6,6,6,6},
  {8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
  {6,6,6,6,6,6,0,6,6,6,6,0,6,6,6,6,6,6,6,6,6,6,6,6},
  {4,4,4,4,4,4,0,4,4,4,6,0,6,2,2,2,2,2,2,2,3,3,3,3},
  {4,0,0,0,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,0,0,0,2},
  {4,0,0,0,0,0,0,0,0,0,0,0,6,2,0,0,5,0,0,2,0,0,0,2},
  {4,0,0,0,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,2,0,2,2},
  {4,0,6,0,6,0,0,0,0,4,6,0,0,0,0,0,5,0,0,0,0,0,0,2},
  {4,0,0,5,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,2,0,2,2},
  {4,0,6,0,6,0,0,0,0,4,6,0,6,2,0,0,5,0,0,2,0,0,0,2},
  {4,0,0,0,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,0,0,0,2},
  {4,4,4,4,4,4,4,4,4,4,1,1,1,2,2,2,2,2,2,3,3,3,3,3}
};

typedef struct {
  f64 x;
  f64 y;
} vec2;

typedef struct {
	i32 x;
	i32 y;
} vec2i;

struct {
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Texture* texture;
	DynArray textures[TEXTURE_COUNT];
  u32 pixels[SCREEN_WIDTH * SCREEN_HEIGHT];

  vec2 pos;
  vec2 dir;
  vec2 plane;
} state;

void setTextures() {
	f32 height_map = (f32) 255 / TEXTURE_HEIGHT;
	f32 width_map = (f32) 255 / TEXTURE_WIDTH;
	// generate some textures
  for(u32 row = 0; row < TEXTURE_HEIGHT; row++) {
		for(u32 col = 0; col < TEXTURE_WIDTH; col++) {
			u32 xy_color = row * height_map / 2 + col * width_map / 2;
			
			u32* color = GET_PTR(state.textures[0], u32, TEXTURE_WIDTH * row + col);
			*color = 0xFF000000 + 0x00FF0000 * (row != col && row != TEXTURE_WIDTH - col); // red w/ black cross
			color = GET_PTR(state.textures[1], u32, TEXTURE_WIDTH * row + col);
			*color = 0xFF000000 + 0x00010000 * xy_color + 0x00000100 * xy_color + 0x00000001 * xy_color; // sloped greyscale
			color = GET_PTR(state.textures[2], u32, TEXTURE_WIDTH * row + col);
			*color = 0xFF000000 + 0x00010000 * xy_color + 0x00000100 * xy_color; // sloped yellow gradient
			color = GET_PTR(state.textures[3], u32, TEXTURE_WIDTH * row + col);
			*color = 0xFF000000 +
							(0x00010101 * (u32) (row * height_map)) ^
							(0x00010101 * (u32) (col * height_map)); // xor greyscale
			color = GET_PTR(state.textures[4], u32, TEXTURE_WIDTH * row + col);
			*color = 0xFF000000 + 
							(0x00000100 * (u32) (row * height_map)) ^ 
							(0x00000100 * (u32) (col * width_map)); // xor green
			color = GET_PTR(state.textures[5], u32, TEXTURE_WIDTH * row + col);
			*color = 0xFF000000 + 0x00FF0000 * (row % 16 && col % 16); // red bricks
			color = GET_PTR(state.textures[6], u32, TEXTURE_WIDTH * row + col);
			*color = 0xFF000000 + 0x00010000 * (u32) (row * height_map); // red gradient
			color = GET_PTR(state.textures[7], u32, TEXTURE_WIDTH * row + col);
			*color = 0xFF888888; // grayscale
		}
	}
}

void render() {
	for (size_t col = 0; col < SCREEN_WIDTH; col++) {
		f64 camera_x = 2 * ((f64) col) / SCREEN_WIDTH - 1; // x in camera space
		vec2 ray_dir = { state.dir.x + state.plane.x * camera_x, 
										 state.dir.y + state.plane.y * camera_x };
		vec2i map_pos = { (int) state.pos.x, (int) state.pos.y }; // map coords
		vec2 side_dist = { 0, 0 }; // length of ray from current pos to next x or y side
		vec2 delta_dist = { (ray_dir.x == 0) ? 1e30 : ABS(1 / ray_dir.x), 
												(ray_dir.y == 0) ? 1e30 : ABS(1 / ray_dir.y) }; 
											// length of ray from one x or y side to next x or y side
		f64 perp_wall_dist = 0; // distance from camera plane to wall
		vec2i step = { 0, 0 }; // what direction to step in, x, or y (-1 or +1)
		i8 hit = 0; // hit or no?
		i8 side = 0; // NS (1) or EW (0) hit?
		
		// calculate step and initial side_dist
		if (ray_dir.x < 0) {
			step.x = -1;
			side_dist.x = (state.pos.x - map_pos.x) * delta_dist.x;
		}
		else {
			step.x = 1;
			side_dist.x = (map_pos.x + 1.0 - state.pos.x) * delta_dist.x;
		}
		if (ray_dir.y < 0) {
			step.y = -1;
			side_dist.y = (state.pos.y - map_pos.y) * delta_dist.y;
		}
		else {
			step.y = 1;
			side_dist.y = (map_pos.y + 1.0 - state.pos.y) * delta_dist.y;
		}

		// perform DDA
		while (hit == 0) {
			// jump to next square, either in x or y direction
			if (side_dist.x < side_dist.y) {
				side_dist.x += delta_dist.x;
				map_pos.x += step.x;
				side = 0;
			}
			else {
				side_dist.y += delta_dist.y;
				map_pos.y += step.y;
				side = 1;
			}

			// check if ray has hit a wall
			if (world_map[map_pos.x][map_pos.y] > 0) hit = 1;
		}

		if (side == 0) perp_wall_dist = side_dist.x - delta_dist.x;
		else					 perp_wall_dist = side_dist.y - delta_dist.y;

		// calculate height of line to draw
		i32 line_height = (i32) (SCREEN_HEIGHT / perp_wall_dist);

		// calculate lowest and highest pixel to draw current stripe
		i32 lowest = -1 * line_height / 2 + SCREEN_HEIGHT / 2;
		if (lowest < 0) lowest = 0;
		i32 highest = line_height / 2 + SCREEN_HEIGHT / 2;
		if (highest >= SCREEN_HEIGHT) highest = SCREEN_HEIGHT - 1;
		
		// texture calculations
		i32 texture_id = world_map[map_pos.x][map_pos.y] - 1;

		// calculate wall_x
		f64 wall_x; // find exactly where wall was hit
		if (side == 0) wall_x = state.pos.y + perp_wall_dist * ray_dir.y;
		else					 wall_x = state.pos.x + perp_wall_dist * ray_dir.x;
		wall_x -= floor(wall_x);

		// x coord on texture
		i32 texture_x = (i32) (wall_x * TEXTURE_WIDTH);
		if (side == 0 && ray_dir.x > 0) texture_x = TEXTURE_WIDTH - texture_x - 1;
		if (side == 1 && ray_dir.y < 0) texture_x = TEXTURE_WIDTH - texture_x - 1;

		// how much to increase texture coord per pixel
		f64 texture_step = 1.0 * TEXTURE_HEIGHT / line_height;
		// starting texture coordinate
		f64 texture_pos = (lowest - SCREEN_HEIGHT / 2 + line_height / 2) * texture_step; 
		for (i32 y = lowest; y < highest; y++) {
			// convert texture_pos to int coord & mask w/ TEXTURE_HEIGHT - 1 to prevent overflow
			i32 texture_y = (i32) texture_pos & (TEXTURE_HEIGHT - 1);
			texture_pos += texture_step;
			u32 color = GET(state.textures[texture_id], u32, TEXTURE_HEIGHT * texture_y + texture_x);

			// divide by 2
			if (side == 1) color = (color >> 1) & 0x7F7F7F7F;
			state.pixels[y * SCREEN_WIDTH + col] = color;
		}
	}
}

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

void rotate(f32 rot_speed) {
	const vec2 d = state.dir;
	const vec2 p = state.plane;
	state.dir.x = d.x * cos(rot_speed) - d.y * sin(rot_speed); 
	state.dir.y = d.x * sin(rot_speed) + d.y * cos(rot_speed);
	state.plane.x = p.x * cos(rot_speed) - p.y * sin(rot_speed); 
	state.plane.y = p.x * sin(rot_speed) + p.y * cos(rot_speed);
}

void handleKeyPress(const u8* key_state) {
	f32 move_speed = 0.025f;
	f32 rotate_speed = 0.01f;
	if (key_state[SDL_SCANCODE_W]) {
		if (world_map[(i32) (state.pos.x + state.dir.x * move_speed)][(i32) state.pos.y] == false) {
			state.pos.x += state.dir.x * move_speed;
		}
		if (world_map[(i32) state.pos.x][(i32) (state.pos.y + state.dir.y * move_speed)] == false) {
			state.pos.y += state.dir.y * move_speed;
		}
	}
	if (key_state[SDL_SCANCODE_S]) {
		if (world_map[(i32) (state.pos.x - state.dir.x * move_speed)][(i32) state.pos.y] == false) {
			state.pos.x -= state.dir.x * move_speed;
		}
		if (world_map[(i32) state.pos.x][(i32) (state.pos.y - state.dir.y * move_speed)] == false) {
			state.pos.y -= state.dir.y * move_speed;
		}
	}
	if (key_state[SDL_SCANCODE_A]) {
		rotate(rotate_speed);
	}
	if (key_state[SDL_SCANCODE_D]) {
		rotate(-1 * rotate_speed);
	}
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

	state.pos.x 	= 22;
	state.pos.y 	= 12;
	state.dir.x 	= -1;
	state.dir.y		=  0;
	state.plane.x =	 0;
	state.plane.y = 0.66;
	u32 val = 0;
	for (u32 i = 0; i < TEXTURE_COUNT; i++) {
		initializeDynArray(&state.textures[i], 
										 	 (void*) &val, 
										 	 sizeof(u32), 
										 	 TEXTURE_WIDTH * TEXTURE_HEIGHT);

	}
	setTextures();

  SDL_Event event;
  int quit = 0;
  while (!quit) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        quit = 1;
      }
		}
		
		const u8* key_state = SDL_GetKeyboardState(NULL);
		handleKeyPress(key_state);

		SDL_SetRenderDrawColor(state.renderer, 0, 0, 0, 255);
		SDL_RenderClear(state.renderer);

		memset(state.pixels, 0, sizeof(state.pixels));
		render();
    draw();

    SDL_RenderPresent(state.renderer);
  }

  SDL_DestroyTexture(state.texture);
  SDL_DestroyRenderer(state.renderer);
  SDL_DestroyWindow(state.window);
  SDL_Quit();
  return 0;
}
