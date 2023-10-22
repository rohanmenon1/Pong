/*
 *Single player game of Pong with 1 level where bricks are to be destroyed to gain points
 *@author: Rohan Menon bossman
 *@date: 9th June 2023
 */
#include <stdio.h>
#define SDL_MAIN_HANDLED //TODO: Find better alternative to stop winMain error
#include <SDL2/SDL.h>
//Including text library
//#include <SDL2/SDL_

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define BRICK_HEIGHT 50
#define BRICK_WIDTH 50

#define PADDLE_WIDTH 80
#define PADDLE_HEIGHT 5 //Do not want to make it the same as ball height
#define PADDLE_SPEED 5

#define BALL_SIZE 10
#define delta 1
#define BALL_SPEED_X 1
#define BALL_SPEED_Y 1

#define abs(x) ((x) > 0 ? (x) : -(x)) 

/*
 *How to compile -  gcc -I src/include -L src/lib -o game main.c -lSDL2
 *Run ./game to play
 */
//SDL_Rect struct only takes ints as parameters!!
// Structure for the pong ball
typedef struct ballStruct {
    int x;
    int y;
    int dx;
    int dy; 
} Ball;

// Structure for the paddle
typedef struct paddleStruct {
    int x;
    int y;
} Paddle;

// Structure for a brick
typedef struct {
    int x;
    int y; // Position
    int destroyed; // Flag to indicate if the brick is destroyed
} Brick;


// Initialize the bricks
void initializeBricks(Brick *bricks, int numRows, int numCols) {
    int startX = (WINDOW_WIDTH - numCols * BRICK_WIDTH) / 2;
    int startY = 40;

    for (int row = 0; row < numRows; row++) {
        for (int col = 0; col < numCols; col++) {
            Brick *brick = &bricks[row * numCols + col];
            brick->x = startX + col * BRICK_WIDTH;
            brick->y = startY + row * BRICK_HEIGHT;
            brick->destroyed = 0;
        }
    }
}

// Move the paddle based on user input
void movePaddle(Paddle *paddle) {
    const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);

    if (keyboardState[SDL_SCANCODE_LEFT] && paddle->x > 0) {
        paddle->x -= PADDLE_SPEED;
    }
    if (keyboardState[SDL_SCANCODE_RIGHT] && paddle->x < WINDOW_WIDTH - PADDLE_WIDTH) {
        paddle->x += PADDLE_SPEED;
    }
}

// Move the ball and handle collisions
//Changed return type to int to indicate if paddle collision ocurred
int moveBall(Ball *ball, int *score, Paddle *paddle) {
    // Move the ball
    ball->x += ball->dx;
    ball->y += ball->dy;

    // Check for wall collisions
    if (ball->x <= 0 || ball->x + BALL_SIZE >= WINDOW_WIDTH) { //x collisions
        ball->dx = -ball->dx;     
            
    }
    
    if (ball->y <= 0) { //Top window collision
        ball->dy = -ball->dy; 
    }
    /* Dont want to do this*/
    
    if (ball->y >= WINDOW_HEIGHT - BALL_SIZE) {
        //Confirm loss of heart/life 

        printf("Collided");
        return 1;
    }

    // Check for paddle collision
    if (ball->y + BALL_SIZE >= paddle->y) {
        if (ball->x + BALL_SIZE >= paddle->x && ball->x <= paddle->x + PADDLE_WIDTH) {
            printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
            ball->dy = -ball->dy; // Reverse the vertical velocity 
        }
        //Not checking for horizontal collisions with paddle TODO
    }
    return 0;


}


int isInside(Ball *ball, Brick *brick) {
   if ((ball->x + BALL_SIZE >= brick->x) && (ball->x <= brick->x + BRICK_WIDTH) &&
       (ball->y + BALL_SIZE >= brick->y) && (ball->y <= brick->y + BRICK_HEIGHT)) {
        return 1;
    }
    else {
        return 0;
    }
}

// Check if the ball collides with a brick
int checkBallBrickCollision(Ball *ball, Brick *brick) {
    if (!brick->destroyed) {
        //Collision along x
        //Ball comes from left -> x + dx is inside and x - dx is not inside
        Ball ball1;
        ball1.x = ball->x + delta;
        ball1.y = ball->y;
        ball1.dx = ball->dx;
        ball1.dy = ball->dy;
        
        Ball ball2;
        ball2.x = ball->x - delta;
        ball2.y = ball->y;
        ball2.dx = ball->dx;
        ball2.dy = ball->dy;
        //Define deltax as a macro
        if ((ball->y + BALL_SIZE >= brick->y) && (ball->y <= brick->y + BRICK_HEIGHT) && (isInside(&ball, brick)) && (!isInside(&ball2, brick) || !isInside(&ball1, brick))) {
            brick->destroyed = 1;
            ball->dx = -ball->dx;
            return 1;
        }
        Ball ball3;
        ball3.x = ball->x;
        ball3.y = ball->y + delta;
        ball3.dx = ball->dx;
        ball3.dy = ball->dy;

        Ball ball4;
        ball3.x = ball->x;
        ball3.y = ball->y - delta;
        ball3.dx = ball->dx;
        ball3.dy = ball->dy;

        if ((ball->x + BALL_SIZE >= brick->x) && (ball->x <= brick->x + BRICK_WIDTH) && (isInside(&ball, brick)) && (!isInside(&ball3, brick) || !isInside(&ball4, brick))) {
            brick->destroyed = 1;
            ball->dy = -ball->dy;
            return 1;
        }


        /*
        if ((ball->x + BALL_SIZE >= brick->x) && (ball->x <= brick->x + BRICK_WIDTH) &&
            (ball->y + BALL_SIZE >= brick->y) && (ball->y <= brick->y + BRICK_HEIGHT)) {
            brick->destroyed = 1; // Destroy the brick
            //Reflect ball from brick here
            //Determining how to reflect based on relative position of ball and brick
            int ballX = ball->x + BALL_SIZE / 2;
            int ballY = ball->y + BALL_SIZE / 2;
            int brickX = brick->x + BRICK_WIDTH / 2;
            int brickY = brick->y + BRICK_HEIGHT / 2;
            
            if (abs(ballX - brickX) > abs(ballY - brickY)) {
                ball->dx = -ball->dx;
            }
            else {
                ball->dy = -ball->dy;
            }
            
                 

            
            return 1; // Collision occurred
        }*/
    }

    return 0; // No collision
}

// Draw the paddle on the screen
void drawPaddle(SDL_Renderer *renderer, Paddle *paddle) {
    SDL_Rect paddleRect = {paddle->x, paddle->y, PADDLE_WIDTH, PADDLE_HEIGHT};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &paddleRect);
}


// Draw the ball on the screen
void drawBall(SDL_Renderer *renderer, Ball *ball) {
    SDL_Rect ballRect = {ball->x, ball->y, BALL_SIZE, BALL_SIZE};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &ballRect);
}

// Draw the bricks on the screen
void drawBricks(SDL_Renderer *renderer, Brick *bricks, int numRows, int numCols) {
    for (int row = 0; row < numRows; row++) {
        for (int col = 0; col < numCols; col++) {
            Brick *brick = &bricks[row * numCols + col];
            if (!(brick->destroyed))  {
                SDL_Rect brickRect = {brick->x, brick->y, BRICK_WIDTH, BRICK_HEIGHT};
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderFillRect(renderer, &brickRect);
            }
        }
    }
}

void initializeBall(Ball *ball) {
    ball->x = WINDOW_WIDTH / 2 - BALL_SIZE / 2;
    ball->y = WINDOW_HEIGHT / 2 - BALL_SIZE / 2;
    ball->dx = BALL_SPEED_X;
    ball->dy = BALL_SPEED_Y;  
    
    
}

int main() {
    int mode;
    printf("Enter game more (1 or 2): ");
    int retVal = scanf("%d", &mode);
    if ((retVal == 1) && (mode == 1)) {
        
    }

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    // Create a window
    SDL_Window *window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (window == NULL) {
        printf("Window creation failed: %s\n", SDL_GetError());
        return 1;
    }

    // Create a renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Got rendering error: %s\n", SDL_GetError());
        return 1;
    }

    // Paddle and ball
    Paddle paddle;
    Ball ball;
    int lives;

    lives = 3;

    
    // Initialize the paddle's position
    paddle.x = WINDOW_WIDTH / 2 - PADDLE_WIDTH / 2;
    paddle.y = WINDOW_HEIGHT - PADDLE_HEIGHT - 10;
    /*
    // Initialize the ball's position and velocity
    ball.x = WINDOW_WIDTH / 2 - BALL_SIZE / 2;
    ball.y = WINDOW_HEIGHT / 2 - BALL_SIZE / 2;
    ball.dx = BALL_SPEED_X;
    ball.dy = BALL_SPEED_Y; */
    initializeBall(&ball);
    
    //Creating brick structs
    const int numRows = 5;
    const int numCols = 8;
    Brick bricks[40];
    initializeBricks(bricks, numRows, numCols);
    
    //Main loop
    int quit = 0;
    int score = 0;
    SDL_Event event;
    int paused = 0;
    while (!quit)  {
        // Process events
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT) {
            quit = 1;
        }
        

        // Move the paddle based on user input
        movePaddle(&paddle);

        // Move the ball and handle collisions
        if (moveBall(&ball, &score, &paddle)) {
            //Trying to create blinking effect
            for (int k = 0; k < 5; k++) {
                initializeBall(&ball);
                SDL_Delay(50);
            }
            lives -= 1;
            if (lives == 0) {
                quit = 1;
            }
        }

        // Check ball-brick collisions
        for (int row = 0; row < numRows; row++) {
            for (int col = 0; col < numCols; col++) {
                Brick *brick = &bricks[row * numCols + col];
                if (!brick->destroyed && checkBallBrickCollision(&ball, brick)) {
                    score += 10; // Increase the score
                }
            }
        }

        // NEED TO CLEAR SCREEN BEFORE REDRAWING
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        //Draw the paddle, ball, and bricks
        drawPaddle(renderer, &paddle);
        drawBall(renderer, &ball);
        drawBricks(renderer, bricks, numRows, numCols);

        //Update
        //not using SDL_flip
        SDL_RenderPresent(renderer);

        // Print the score
        printf("Score: %d Lives Left: %d\n", score, lives);
        SDL_Delay(10);
    }

    //Create window for game over
    printf("Game over. Thank you for playing.\n");



    // Clean up resources
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
