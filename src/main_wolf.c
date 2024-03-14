#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <SDL.h>

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

i32 world_map[MAP_WIDTH][MAP_HEIGHT] = {
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
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
  u32 pixels[SCREEN_WIDTH * SCREEN_HEIGHT];

  vec2 pos;
  vec2 dir;
  vec2 plane;
} state;

u32 getWallColor(i32 x, i32 y, i8 side) {
	u32 color = 0xFF000000;
	switch (world_map[x][y]) {
		case 1: { // red
			color += 0x00AA0000;
			if (side == 1) color -= 0x00550000;
			break;
		}
		case 2: { // green
			color += 0x0000AA00;	
			if (side == 1) color -= 0x00005500;
			break;
		}
		case 3: { // blue
			color += 0x000000AA;
			if (side == 1) color -= 0x00000055;
			break;
		}
		case 4: { // white
			color += 0x00AAAAAA;
			if (side == 1) color -= 0x00555555;
			break;
		}
		default: { // yellow
			color += 0x00AAAA00;
			if (side == 1) color -= 0x00555500;
			break;
		}
	}

	return color;
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
		
		// get wall color
		u32 color = getWallColor(map_pos.x, map_pos.y, side);

		for (i32 row = lowest; row < highest; row++) {
			state.pixels[row * SCREEN_WIDTH + col] = color;
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
