#include <SDL2/SDL.h>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <cstdlib>

const int MAX_ROWS = 32, MAX_COLS = 32;

using namespace emscripten;

struct context
{
  SDL_Renderer *renderer;
  int generation;
  int **cells_prev_data;
  int **cells_next_data;
  int cell_width;
  int cell_height;
} *g_ctx;

void copy_glider_pattern(int **data)
{
    int pattern[3][3] = {
        0,1,0,
        0,0,1,
        1,1,1
    };
    for (int i=0; i < 3; i++) {
        for (int j=0; j < 3; j++) {
            data[i][j] = pattern[i][j];
        }
    }
}

//Allocate a 2D array with base pointer type 'T'
template <class T>
inline void allocate2DArray(T** &dest, int nrows, int ncols)
{
    dest = (T**)malloc(nrows * ncols * sizeof(T));
    memset(dest, 0, nrows * ncols * sizeof(T));
    for (int i=0; i < nrows; i++)
        *(dest + i) = (T*)calloc(ncols, sizeof(T));
}

template <class T>
inline void free2DArray(T** &dest, int nrows, int ncols)
{
    for (int i=0; i < nrows; i++) {
        for (int j=0; j < ncols; j++) {
            dest[i][j] = 0;
        }
    }
    dest = 0;
}

void reset()
{
  g_ctx->generation = 0;
  for (int y=0; y < MAX_ROWS; y++) {
    for (int x=0; x < MAX_COLS; x++) {
      g_ctx->cells_prev_data[y][x] = 0;
      g_ctx->cells_next_data[y][x] = 0;
    }
  }
  copy_glider_pattern(g_ctx->cells_prev_data);
  EM_ASM({
    Module.updateCounter($0);
  }, g_ctx->generation);
}

EMSCRIPTEN_BINDINGS(my_module) {
    function("reset", &reset);
}

void next_generation(context *arg)
{
  int **cells_prev = arg->cells_prev_data;
  int **cells_next = arg->cells_next_data;
  for (int y=0; y < MAX_ROWS; y++) {
      for (int x=0; x < MAX_COLS; x++) {
          // Search cell neighbors
          uint sum = 0;
          for (signed int i=-1; i <= 1; i++) {
              for (signed int j=-1; j <= 1; j++) {
                  sum += cells_prev[(y+i+MAX_ROWS)%MAX_ROWS][(x+j+MAX_COLS)%MAX_COLS];
              }
          }
          if (cells_prev[y][x] == 1)
              sum--;
          if (cells_prev[y][x] == 1) {
              if (sum < 2 || sum > 3) {
                  cells_next[y][x] = 0;
              }
              if (sum == 2 || sum == 3) {
                  cells_next[y][x] = 1;
              }
          }
          if (cells_prev[y][x] == 0) {
              if (sum == 3) {
                  cells_next[y][x] = 1;
              }
          }
      }
  }
  for (int y=0; y < MAX_ROWS; y++) {
    for (int x=0; x < MAX_COLS; x++) {
      cells_prev[y][x] = cells_next[y][x];
    }
  }
}

void mainloop(void *arg)
{
  SDL_Renderer *renderer = g_ctx->renderer;
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  SDL_Rect cr;
  cr.w = g_ctx->cell_width;
  cr.h = g_ctx->cell_height;
  next_generation(g_ctx);
  for (int y=0; y < MAX_ROWS; y++) {
    cr.y = y * g_ctx->cell_height;
    for (int x=0; x < MAX_COLS; x++) {
      cr.x = x * g_ctx->cell_width;
      if (g_ctx->cells_next_data[y][x] == 1) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      } else if (g_ctx->cells_next_data[y][x] == 0) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      }
      SDL_RenderFillRect(renderer, &cr);
    }
  }
  SDL_RenderPresent(renderer);
  g_ctx->generation++;
  EM_ASM({
    Module.updateCounter($0);
  }, g_ctx->generation);
}

int main()
{
  const int WINDOW_WIDTH = 640, WINDOW_HEIGHT = 480;
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer);

  g_ctx->renderer = renderer;
  g_ctx->generation = 0;
  g_ctx->cell_width = WINDOW_WIDTH / MAX_COLS;
  g_ctx->cell_height = WINDOW_HEIGHT / MAX_ROWS;
  allocate2DArray(g_ctx->cells_prev_data, MAX_ROWS, MAX_COLS);
  allocate2DArray(g_ctx->cells_next_data, MAX_ROWS, MAX_COLS);
  copy_glider_pattern(g_ctx->cells_prev_data);

  const int simulate_infinite_loop = 1; // call the function repeatedly
  const int fps = 15;
  emscripten_set_main_loop_arg(mainloop, &g_ctx, fps, simulate_infinite_loop);
  
  free2DArray(g_ctx->cells_prev_data, MAX_ROWS, MAX_COLS);
  free2DArray(g_ctx->cells_next_data, MAX_ROWS, MAX_COLS);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return EXIT_SUCCESS;
}
