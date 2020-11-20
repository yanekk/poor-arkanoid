#include <SDL2/SDL.h>
#include <stdbool.h>
#include <math.h>

#define WIDTH 320
#define HEIGHT 240

#define BRICKS_PER_LINE 32
#define BRICK_LINES 6
#define BRICK_SIZE 10

#define LEFT(box) (box.rect.x)
#define RIGHT(box) (box.rect.x + box.rect.w)
#define TOP(box) (box.rect.y)
#define BOTTOM(box) (box.rect.y + box.rect.h)

#define WITHIN_WIDTH(box1, box2) LEFT(box1) >= LEFT(box2) && RIGHT(box1) <= RIGHT(box2)
#define WITHIN_HEIGHT(box1, box2) TOP(box1) >= TOP(box2) && BOTTOM(box1) <= BOTTOM(box2)
typedef struct Vector
{
  double x;
  double y;
  int velocity;
} Vector;

typedef struct Box
{
  SDL_Rect rect;
  SDL_Color color;
  Vector vector;
  bool isHidden;
} Box;

Box paddle, ball, bricks[BRICK_LINES][BRICKS_PER_LINE];

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Event event;

int main(int argc, char *argv[])
{
  SDL_Init(SDL_INIT_VIDEO);

  window = SDL_CreateWindow("Arkanoid!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);

  paddle = (Box){
    .rect =  (SDL_Rect) { .x = -1, .y = HEIGHT - 20, .w = 40, .h = 10},
    .vector = (Vector) { .x = 0, .y = 0, .velocity = 2},
    .color = (SDL_Color) { .r = 128, .g = 128, .b = 128}
  };
  paddle.rect.x = (WIDTH / 2) - paddle.rect.w / 2;

  ball = (Box){
    .rect = (SDL_Rect) { .x = WIDTH / 2, .y = HEIGHT / 2, .w = 5, .h = 5},
    .vector = (Vector) { .x = 1, .y = -1, .velocity = 2 },
    .color = (SDL_Color) { .r = 255, .g = 0, .b = 0}
  };

  for(int line = 0; line < BRICK_LINES; line++) {
    for(int brick = 0; brick < BRICKS_PER_LINE; brick++) {
      bricks[line][brick] = (Box) {
        .rect = (SDL_Rect){.x = BRICK_SIZE * brick, .y = BRICK_SIZE * line, .h = BRICK_SIZE, .w = BRICK_SIZE},
        .color = (SDL_Color){ .r =  rand() % 255, .g =  rand() % 255, .b =  rand() % 255 }
      };
    }
  }

  bool running = true;

  Uint32 lastTime, currTime, milis;

  while (running)
  {
    while (SDL_PollEvent(&event) != 0)
    {
      if (event.type == SDL_QUIT)
        running = false;

      if (event.type == SDL_KEYDOWN)
      {
        if (event.key.keysym.sym == SDLK_LEFT)
          paddle.vector.x = -1;
        if (event.key.keysym.sym == SDLK_RIGHT)
          paddle.vector.x = 1;
      }
      if(event.type == SDL_KEYUP) {
        paddle.vector.x = 0;
      }      
    }
  
    // rendering
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);
  
    for(int line = 0; line < BRICK_LINES; line++) {
      for(int brick = 0; brick < BRICKS_PER_LINE; brick++) {
          Box current_brick = bricks[line][brick];
          if(!current_brick.isHidden) {
            SDL_SetRenderDrawColor(renderer, current_brick.color.r, current_brick.color.g, current_brick.color.b, 255);
            SDL_RenderFillRect(renderer, &current_brick.rect);
          }
      }
    }

    SDL_SetRenderDrawColor(renderer, paddle.color.r, paddle.color.g, paddle.color.b, 255);
    SDL_RenderFillRect(renderer, &paddle.rect);

    SDL_SetRenderDrawColor(renderer, ball.color.r, ball.color.g, ball.color.b, 255);
    SDL_RenderFillRect(renderer, &ball.rect);

    // collisions
    // collide ball against boundaries
    if (RIGHT(ball) >= WIDTH || LEFT(ball) <= 0)
      ball.vector.x = -ball.vector.x;
    if(TOP(ball) <= 0)
      ball.vector.y = -ball.vector.y;

    if (BOTTOM(ball) >= HEIGHT) 
      running = false;

    // collide ball against paddle
    if(BOTTOM(ball) >= TOP(paddle) && WITHIN_WIDTH(ball, paddle)) 
      ball.vector.y = -ball.vector.y;

    // collid ball against bricks
    Box current_brick;
    for(int line = 0; line < BRICK_LINES; line++) {
      for(int brick = 0; brick < BRICKS_PER_LINE; brick++) {
        current_brick = bricks[line][brick];
        if(current_brick.isHidden)
          continue;

        if(TOP(ball) <= BOTTOM(current_brick) 
          && BOTTOM(current_brick) - TOP(ball) <= ball.vector.velocity 
          && WITHIN_WIDTH(ball, current_brick)) {
          ball.vector.y = -ball.vector.y;
          bricks[line][brick].isHidden = true;
        }  
        if(((LEFT(ball) >= RIGHT(current_brick) && LEFT(ball) - RIGHT(current_brick) <= ball.vector.velocity)
          || RIGHT(ball) <= LEFT(current_brick) && LEFT(current_brick) - RIGHT(ball) <= ball.vector.velocity)
          && WITHIN_HEIGHT(ball, current_brick)) {
          ball.vector.x = -ball.vector.x;
          bricks[line][brick].isHidden = true;
        }
      }
    }

    ball.rect.x += ball.vector.x * ball.vector.velocity;
    ball.rect.y += ball.vector.y * ball.vector.velocity;

    // move paddle left and right
    if(paddle.vector.x == -1 && LEFT(paddle) > 0) {
      paddle.rect.x += paddle.vector.x * paddle.vector.velocity;
      paddle.rect.y += paddle.vector.y * paddle.vector.velocity;
    }
    if(paddle.vector.x == 1 && RIGHT(paddle) < WIDTH) {
      paddle.rect.x += paddle.vector.x * paddle.vector.velocity;
      paddle.rect.y += paddle.vector.y * paddle.vector.velocity;
    }

    SDL_RenderPresent(renderer);
  }
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
}
