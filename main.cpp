#include <SDL.h>
#include <iostream>
#include <SDL2_gfxPrimitives.h>
#include <cmath>
#include <chrono>
#include "client.h"

// g++ -std=c++11 -o t test.cpp client.cpp -I /opt/homebrew/Cellar/sdl2/2.28.2/include/SDL2 -L /opt/homebrew/Cellar/sdl2/2.28.2/lib/ -L /opt/homebrew/Cellar/sdl2_gfx/1.0.4/lib -I /opt/homebrew/Cellar/sdl2_gfx/1.0.4/include/SDL2/ -lsdl2_gfx -lsdl2 -I /opt/homebrew/Cellar/flatbuffers/23.5.26/include

// Constants
const int SCREEN_WIDTH = 900 * 2;
const int SCREEN_HEIGHT = 500 * 2;
const int RADIUS = 30;
const float MAX_FRAME_TIME = 0.1f; // Maximum frame time to prevent spiral of death
const float VELOCITY = 400.0f; // pixels per second

const int PADDING = 40;
const int GOAL_HEIGHT = 300;
const int GOAL_WIDTH = 160;

// time step
const Uint32 deltaTime = 16;
const float deltaTimeSeconds = static_cast<float>(deltaTime) / 1000.0f;

// Initial Position and Velocity
// PID 0 is ball
int PID;
const int NUM_OF_OBJS = 3;
std::vector<std::vector<float>> pos = {{SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2},
                                       {SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2}, 
                                       {SCREEN_WIDTH / 2 + 200, SCREEN_HEIGHT / 2}}; // positions
std::vector<std::vector<float>> vel(3, std::vector<float>(2, 0)); // velocities

// SDL variables
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;


// Function to initialize SDL
bool initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Soccer Field", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

void detectKeyPress() {
    SDL_Event event;

    // Poll for SDL events
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            exit(0);
        }
        else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                    vel[PID][1] = -VELOCITY;
                    break;
                case SDLK_DOWN:
                    vel[PID][1] = +VELOCITY;
                    break;
                case SDLK_LEFT:
                    vel[PID][0] = -VELOCITY;
                    break;
                case SDLK_RIGHT:
                    vel[PID][0] = +VELOCITY;
                    break;
            }
        }
    }
}

void handleCollision(int i, int j) {
    // COLLISION PHYSICS
    // Calculate the collision normal
    float dx = pos[j][0] - pos[i][0];
    float dy = pos[j][1] - pos[i][1];
    float distance = std::sqrt(dx * dx + dy * dy);
    float normalX = dx / distance;
    float normalY = dy / distance;

    // Calculate the relative velocity
    float relativeVelocityX = vel[j][0] - vel[i][0];
    float relativeVelocityY = vel[j][1] - vel[i][1];

    // impulse formula
    float impulse = (2.0f * (relativeVelocityX * normalX + relativeVelocityY * normalY)) /
                    (1.0f / RADIUS + 1.0f / RADIUS);

    // Update velocities of both objects after the collision
    vel[i][0] += impulse / RADIUS * normalX;
    vel[i][1] += impulse / RADIUS * normalY;
    vel[j][0] -= impulse / RADIUS * normalX;
    vel[j][1] -= impulse / RADIUS * normalY;
}

bool overlaps(float x, float y, int i) {
    // check against every object for overlapping
    for (int j=0; j < NUM_OF_OBJS; j++) {
        if (i != j) {
            float dx = x - pos[j][0];
            float dy = y - pos[j][1];
            float distance = std::sqrt(dx * dx + dy * dy);

            if (distance <= 2 * RADIUS) {
                handleCollision(i, j);
                return true;
            }
        }
    }
    return false;
}

void boundaryCheck(int i) {
    if (pos[i][0] < RADIUS+PADDING) 
        pos[i][0] = RADIUS+PADDING;

    if (pos[i][0] > SCREEN_WIDTH - RADIUS - PADDING) 
        pos[i][0] = SCREEN_WIDTH - RADIUS - PADDING;

    if (pos[i][1] < RADIUS+PADDING) 
        pos[i][1] = RADIUS+PADDING;

    if (pos[i][1] > SCREEN_HEIGHT - RADIUS - PADDING) 
        pos[i][1] = SCREEN_HEIGHT - RADIUS - PADDING;
}

void nextState(int i) {
    // PHYSICS OF OBJECT
    // Smoothly move the object with velocity + friction

    float frictionCoefficient = 1.2;
    vel[i][0] = vel[i][0] - (vel[i][0] * frictionCoefficient * deltaTimeSeconds);
    vel[i][1] = vel[i][1] - (vel[i][1] * frictionCoefficient * deltaTimeSeconds);

    float x = pos[i][0] + (vel[i][0] * deltaTimeSeconds);
    float y = pos[i][1] + (vel[i][1] * deltaTimeSeconds);

    if (!overlaps(x, y, i))
        pos[i].assign({x, y});

    // Boundary checking
    boundaryCheck(i);
}

// Function to render the scene
void render() {
    SDL_SetRenderDrawColor(renderer, 46, 200, 46, 0);  // Green field color
    SDL_RenderClear(renderer);

    SDL_Rect fieldRect = { PADDING, PADDING, SCREEN_WIDTH - (PADDING * 2), SCREEN_HEIGHT - (PADDING * 2) };
    SDL_Rect goalLeft = { PADDING, SCREEN_HEIGHT/2-(GOAL_HEIGHT/2), GOAL_WIDTH, GOAL_HEIGHT};
    SDL_Rect goalRight = { SCREEN_WIDTH-PADDING-GOAL_WIDTH, SCREEN_HEIGHT/2-(GOAL_HEIGHT/2), GOAL_WIDTH, GOAL_HEIGHT};

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // White field lines

    SDL_RenderDrawRect(renderer, &fieldRect); // outer rect
    SDL_RenderDrawLine(renderer, SCREEN_WIDTH/2, PADDING, SCREEN_WIDTH/2, SCREEN_HEIGHT-PADDING-1); // midline
    ellipseRGBA(renderer, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 200, 200, 255, 255, 255, 255); // white center circle
    SDL_RenderDrawRect(renderer, &goalLeft); // goal left
    SDL_RenderDrawRect(renderer, &goalRight); // goal right

    filledCircleRGBA(renderer, pos[0][0], pos[0][1], RADIUS, 255, 0, 0, 255); // ball
    filledCircleRGBA(renderer, pos[1][0], pos[1][1], RADIUS, 0, 0, 255, 255); // p1
    filledCircleRGBA(renderer, pos[2][0], pos[2][1], RADIUS, 0, 0, 0, 255); // p2

    // for (int i=0; i<NUM_OF_OBJS; i++) {
    //     filledCircleRGBA(renderer, pos[i][0], pos[i][1], RADIUS, 0, 0, 255, 255);
    // }

    SDL_RenderPresent(renderer);
}

int main(int argc, char* args[]) {
    // ./t 1 12346 # player 1
    // ./t 2 12347 # player 2

    PID = std::stoi(args[1]);
    int port = std::stoi(args[2]);

    Client myClient(port);

    if (!initSDL()) {
        return 1;
    }
    detectKeyPress(); // reduces 1 source of lag    

    int count = 0; // DEBUG
    Uint32 currentTime;
    Uint32 execTime;

    bool quit = false;
    while (!quit) {
        currentTime = SDL_GetTicks();
        detectKeyPress();

        // update states from network
        // std::this_thread::sleep_for(std::chrono::milliseconds(1));
        
        while (!myClient.isEmptyRecv()) {
            std::array<float, 5> data = myClient.popRecv();
            // std::cout << data[0] << "," << data[1] << "," << data[2] << "," << data[3] << "," << data[4] << std::endl;
            int curPid = static_cast<int>(data[0]);
            if (curPid != PID) {
                pos[curPid].assign({data[1], data[2]});
                vel[curPid].assign({data[3], data[4]});
            }
        }
        
        // Update the game states
        for (int i=0; i<NUM_OF_OBJS; i++)
            nextState(i);

        render();

        myClient.send(PID, pos[PID][0], pos[PID][1], vel[PID][0], vel[PID][1]);

        execTime = SDL_GetTicks() - currentTime;
        if (execTime <= deltaTime)
            SDL_Delay(deltaTime - execTime);
        else {
            count++; // DEBUG
            std::cout << execTime << std::endl; // DEBUG
            std::cout << count << std::endl; // DEBUG
        }

    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
