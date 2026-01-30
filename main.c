#include <SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int SCREEN_WIDTH_PX = 1200;
const int SCREEN_HEIGHT_PX = 800;
const int SNAKE_HEIGHT_PX = 40;
const int SNAKE_WIDTH_PX = 40;
const int INIT_SNAKE_LENGTH = 3;
const int MAX_SNAKE_LENGTH = 20;
const float TICK_RATE = 0.075f;
typedef struct {
  Uint16 x;
  Uint16 y;
} Position;

typedef enum {
  LEFT,
  RIGHT,
  UP,
  DOWN,
} Direction;

typedef struct {
  Direction direction;
  Position currentPositions[MAX_SNAKE_LENGTH];
  Position nextPositions[MAX_SNAKE_LENGTH];
  int currLength;
} Snake;

int generateRandomIntWithStep(int min_value, int max_value, int step) {
  int num_possible_values = (max_value - min_value) / step + 1;

  int random_index = rand() % num_possible_values;

  int random_value = min_value + random_index * step;

  return random_value;
}

void updateFoodPos(Position *food) {
  food->x = generateRandomIntWithStep(0, SCREEN_WIDTH_PX - SNAKE_WIDTH_PX,
                                      SNAKE_WIDTH_PX);
  food->y = generateRandomIntWithStep(0, SCREEN_HEIGHT_PX - SNAKE_HEIGHT_PX,
                                      SNAKE_HEIGHT_PX);
}

bool moveSnake(Uint8 direction, Snake *snake, Position *food,
               SDL_Window *window) {
  int changeX = 0;
  int changeY = 0;
  switch (direction) {
  case LEFT:
    changeX = -SNAKE_WIDTH_PX;
    break;
  case UP:
    changeY = -SNAKE_HEIGHT_PX;
    break;
  case RIGHT:
    changeX = SNAKE_WIDTH_PX;
    break;
  case DOWN:
    changeY = SNAKE_HEIGHT_PX;
    break;
  }

  // Game Over Conditions
  //  Player loses if snake touches the wall
  Position next_head_pos = snake->nextPositions[0];
  Position curr_head_pos = snake->currentPositions[0];
  if (next_head_pos.x < 0 || next_head_pos.x >= SCREEN_WIDTH_PX ||
      next_head_pos.y < 0 || next_head_pos.y >= SCREEN_HEIGHT_PX)
    return false;

  for (int i = 1; i < snake->currLength; i++) {
    if (next_head_pos.x == snake->currentPositions[i].x &&
        next_head_pos.y == snake->currentPositions[i].y) {
      return false;
    }
  }
  // growing mechanic
  if (curr_head_pos.x == food->x && curr_head_pos.y == food->y) {
    snake->currLength++;
    snake->nextPositions[snake->currLength - 1] =
        snake->currentPositions[snake->currLength - 2];
    updateFoodPos(food);
    char buffer[50];
    snprintf(buffer, sizeof(buffer), "C_Snake! Score: %d", snake->currLength);
    SDL_SetWindowTitle(window, buffer);
  }

  // Update positions (snake movement)
  for (int i = 0; i < snake->currLength; i++) {
    snake->currentPositions[i] = snake->nextPositions[i];
  }

  snake->nextPositions[0].x += changeX;
  snake->nextPositions[0].y += changeY;
  for (int i = 1; i < snake->currLength; i++) {
    snake->nextPositions[i] = snake->currentPositions[i - 1];
  }
  return true;
}

int main() {
  // Initialize SDL
  bool running = true;
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
    printf("SDL initialization failed: %s\n", SDL_GetError());
    return 1;
  }
  Uint64 startTime = SDL_GetTicks64();

  // Create window
  SDL_Window *window = SDL_CreateWindow("C_Snake", SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH_PX,
                                        SCREEN_HEIGHT_PX, SDL_WINDOW_SHOWN);
  if (!window) {
    printf("Window creation failed: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  if (!renderer) {
    printf("Renderer creation failed: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  SDL_Event event;

  Snake snake = {LEFT,
                 {
                     {600, 400},
                     {600 + SNAKE_WIDTH_PX, 400},
                     {600 + 2 * SNAKE_WIDTH_PX, 400},
                 },
                 {
                     {600 - SNAKE_WIDTH_PX, 400},
                     {600, 400},
                     {600 + SNAKE_WIDTH_PX, 400},
                 },
                 INIT_SNAKE_LENGTH};
  Uint64 lastMoveTime = startTime;
  float deltaTime = 0.0f;

  char titleBuffer[50];
  snprintf(titleBuffer, sizeof(titleBuffer), "C_Snake! Score: %d",
           snake.currLength);
  SDL_SetWindowTitle(window, titleBuffer);

  srand((unsigned)time(NULL));
  Position food = {0, 0};
  updateFoodPos(&food);

  SDL_Rect tempRect;
  tempRect.w = SNAKE_WIDTH_PX;
  tempRect.h = SNAKE_HEIGHT_PX;
  while (running) {
    while (SDL_PollEvent(&event) != 0) {
      switch (event.type) {
      case SDL_QUIT:
        running = false;
        break;
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:
          running = false;
          break;
        case SDLK_x:
          running = false;
          break;
        case SDLK_w:
          if (snake.direction == DOWN) {
            break;
          }
          snake.direction = UP;
          break;
        case SDLK_UP:
          if (snake.direction == DOWN) {
            break;
          }
          snake.direction = UP;
          break;
        case SDLK_a:
          if (snake.direction == RIGHT) {
            break;
          }
          snake.direction = LEFT;
          break;
        case SDLK_LEFT:
          if (snake.direction == RIGHT) {
            break;
          }
          snake.direction = LEFT;
          break;
        case SDLK_s:
          if (snake.direction == UP) {
            break;
          }
          snake.direction = DOWN;
          break;
        case SDLK_DOWN:
          if (snake.direction == UP) {
            break;
          }
          snake.direction = DOWN;
          break;
        case SDLK_d:
          if (snake.direction == LEFT) {
            break;
          }
          snake.direction = RIGHT;
          break;
        case SDLK_RIGHT:
          if (snake.direction == LEFT) {
            break;
          }
          snake.direction = RIGHT;
          break;
        }
        if (event.key.keysym.sym == SDLK_ESCAPE) {
          running = false;
        }
      }
    }

    Uint8 r = 218;
    Uint8 g = 145;
    Uint8 b = 145;
    Uint8 a = 200;
    if (SDL_SetRenderDrawColor(renderer, r, g, b, a) < 0) {
      printf("Setting Renderer Color failed: %s\n", SDL_GetError());
      return 1;
    }

    if (SDL_RenderClear(renderer) < 0) {
      printf("Clearing Renderer with Color failed: %s\n", SDL_GetError());
      return 1;
    }

    // Draw grid outlines
    if (SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255) < 0) {
      printf("Setting Renderer Color failed: %s\n", SDL_GetError());
      return 1;
    }
    for (uint16_t i = 0; i <= SCREEN_WIDTH_PX - SNAKE_WIDTH_PX;
         i += SNAKE_WIDTH_PX) {
      for (uint16_t j = 0; j <= SCREEN_HEIGHT_PX - SNAKE_HEIGHT_PX;
           j += SNAKE_HEIGHT_PX) {
        tempRect.x = i;
        tempRect.y = j;
        SDL_RenderDrawRect(renderer, &tempRect);
      }
    }

    // Draw Food
    if (SDL_SetRenderDrawColor(renderer, 98, 187, 232, 255) < 0) {
      printf("Setting Renderer Color failed: %s\n", SDL_GetError());
      return 1;
    }
    tempRect.x = food.x;
    tempRect.y = food.y;
    SDL_RenderFillRect(renderer, &tempRect);

    // Draw Snake
    if (SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255) < 0) {
      printf("Setting Renderer Color failed: %s\n", SDL_GetError());
      return 1;
    }
    for (int i = 0; i < snake.currLength; i++) {
      tempRect.x = snake.currentPositions[i].x;
      tempRect.y = snake.currentPositions[i].y;
      tempRect.w = SNAKE_WIDTH_PX;
      tempRect.h = SNAKE_HEIGHT_PX;
      SDL_RenderFillRect(renderer, &tempRect);
    }

    SDL_RenderPresent(renderer);
    deltaTime = (float)(SDL_GetTicks64() - lastMoveTime) / 1000.0f;
    if (deltaTime > TICK_RATE) {
      deltaTime = 0.0f;
      lastMoveTime = SDL_GetTicks64();
      if (!moveSnake(snake.direction, &snake, &food, window)) {
        running = false;
        printf("You Lose! Score: %d\n", snake.currLength);
      }

      // Win Condition
      if (snake.currLength == MAX_SNAKE_LENGTH) {
        running = false;
        printf("You win! You reached the scored of %d!\n", MAX_SNAKE_LENGTH);
      }
    }
  }

  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_Quit();
  return 0;
}
