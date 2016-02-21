#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <SDL2/SDL.h>

#define TILE_SIZE 30
#define TILE_BOUND_SIZE 5
#define TILE_X_SIZE TILE_SIZE
#define TILE_Y_SIZE TILE_SIZE
#define SCREEN_X_SIZE (SCREEN_WIDTH/TILE_X_SIZE)
#define SCREEN_Y_SIZE (SCREEN_HEIGHT/TILE_Y_SIZE)
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

static unsigned int world[SCREEN_X_SIZE][SCREEN_Y_SIZE];
static unsigned int lines[SCREEN_X_SIZE][SCREEN_Y_SIZE+1];
static unsigned int columns[SCREEN_X_SIZE+1][SCREEN_Y_SIZE];

static SDL_Renderer *renderer;
static unsigned int current_player = 1;
static unsigned int max_players = 2;
static int current_line_x = -1, current_line_y = -1, current_column_x = -1, current_column_y = -1;

struct player
{
    int r;
    int g;
    int b;
    int rc;
    int gc;
    int bc;
};

struct player players[3];

void init_players(void)
{
    players[0].rc = 235;
    players[0].gc = 235;
    players[0].bc = 235;
    players[0].r = 100;
    players[0].g = 100;
    players[0].b = 100;
    players[1].rc = 200;
    players[1].gc = 100;
    players[1].bc = 0;
    players[1].r = 100;
    players[1].g = 0;
    players[1].b = 0;
    players[2].rc = 0;
    players[2].gc = 100;
    players[2].bc = 200;
    players[2].r = 0;
    players[2].g = 0;
    players[2].b = 100;
}

unsigned int check_case(unsigned int x, unsigned int y)
{
    if(x > SCREEN_X_SIZE)
        return 0;
    if(y > SCREEN_Y_SIZE)
        return 0;
    if(lines[x][y] == 0 || lines[x][y + 1] == 0 || columns[x][y] == 0 || columns [x + 1][y] == 0)
        return 0;
    return 1;
}

void init_world(void)
{
    memset(world, 0, sizeof(world));
    memset(lines, 0, sizeof(lines));
    memset(columns, 0, sizeof(columns));
}

void _draw_case(unsigned int x, unsigned int y, int r, int g, int b)
{
    SDL_Rect pos = {x * TILE_X_SIZE + TILE_BOUND_SIZE,  y * TILE_Y_SIZE + TILE_BOUND_SIZE, TILE_X_SIZE - 2 * TILE_BOUND_SIZE, TILE_Y_SIZE - 2 * TILE_BOUND_SIZE};
    SDL_SetRenderDrawColor(renderer, r, g, b, 1);
    if(SDL_RenderFillRect(renderer, &pos) == -1)
    {
        printf("Can't fill rect : %s\n", SDL_GetError());
    }
}

void draw_case(unsigned int x, unsigned int y)
{
    struct player *player = &players[world[x][y]];
    printf("player %d (%d %d %d)\n", world[x][y], player->r, player->g, player->b);
    _draw_case(x, y, player->rc, player->gc, player->bc);
}

void draw_plot(unsigned int x, unsigned int y)
{
    SDL_Rect pos = {x * TILE_X_SIZE - TILE_BOUND_SIZE,  y * TILE_Y_SIZE - TILE_BOUND_SIZE, TILE_BOUND_SIZE * 2, TILE_BOUND_SIZE * 2};
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 1);
    if(SDL_RenderFillRect(renderer, &pos) == -1)
    {
        printf("Can't fill rect : %s\n", SDL_GetError());
    }
}

void _draw_line(unsigned int x, unsigned int y, int r, int g, int b, int a)
{
    SDL_Rect pos = {x * TILE_X_SIZE + TILE_BOUND_SIZE,  y * TILE_Y_SIZE - TILE_BOUND_SIZE, TILE_X_SIZE - 2 * TILE_BOUND_SIZE, TILE_BOUND_SIZE * 2};
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    if(SDL_RenderFillRect(renderer, &pos) == -1)
    {
        printf("Can't fill rect : %s\n", SDL_GetError());
    }
}

void draw_line(unsigned int x, unsigned int y)
{
    struct player *player = &players[lines[x][y]];
    printf("player %d (%d %d %d)\n", lines[x][y], player->r, player->g, player->b);
    _draw_line(x, y, player->r, player->g, player->b, 1);
}

void draw_selected_line(unsigned int x, unsigned int y)
{
    _draw_line(x, y, 80, 0, 0, 1);
}

void _draw_column(unsigned int x, unsigned int y, int r, int g, int b, int a)
{
    SDL_Rect pos = {x * TILE_X_SIZE - TILE_BOUND_SIZE,  y * TILE_Y_SIZE + TILE_BOUND_SIZE, 2 * TILE_BOUND_SIZE, TILE_Y_SIZE - 2 * TILE_BOUND_SIZE};
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    if(SDL_RenderFillRect(renderer, &pos) == -1)
    {
        printf("Can't fill rect : %s\n", SDL_GetError());
    }
}

void draw_column(unsigned int x, unsigned int y)
{
    struct player *player = &players[columns[x][y]];
    printf("player %d (%d %d %d)\n", columns[x][y], player->r, player->g, player->b);
    _draw_column(x, y, player->r, player->g, player->b, 1);
}

void draw_selected_column(unsigned int x, unsigned int y)
{
    _draw_column(x, y, 80, 0, 0, 1);
}

void draw_world(void)
{
    for(int x = 0; x < SCREEN_X_SIZE; x++)
    {
        for(int y = 0; y < SCREEN_Y_SIZE; y++)
            draw_case(x, y);
    }
    for(int x = 0; x < SCREEN_X_SIZE + 1; x++)
    {
        for(int y = 0; y < SCREEN_Y_SIZE + 1; y++)
            draw_plot(x, y);
    }
    for(int x = 0; x < SCREEN_X_SIZE; x++)
    {
        for(int y = 0; y < SCREEN_Y_SIZE + 1; y++)
            draw_line(x, y);
    }
    for(int x = 0; x < SCREEN_X_SIZE + 1; x++)
    {
        for(int y = 0; y < SCREEN_Y_SIZE; y++)
            draw_column(x, y);
    }
}

int main( int argc, char *argv[ ] )
{
    srand(time(NULL));//Init randome seed.
    SDL_Window *window;
    if( SDL_Init( SDL_INIT_VIDEO ) == -1 )
    {
        printf( "Can't init SDL:  %s\n", SDL_GetError( ) );
        return EXIT_FAILURE;
    }

    atexit( SDL_Quit );
    SDL_Surface *surface;
    window = SDL_CreateWindow("Ma fenêtre de jeu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0 );
    surface = SDL_GetWindowSurface(window);
    renderer = SDL_GetRenderer(window);

    if( surface == NULL )
    {
        printf( "Can't set video mode: %s\n", SDL_GetError( ) );
        return EXIT_FAILURE;
    }   

    init_players();
    init_world();
    draw_world();
    // Main loop
    SDL_Event event;
    while(1)
    {
        // Check for messages
        if (SDL_PollEvent(&event))
        {
            // Check for the quit message
            switch (event.type)
            {
                case SDL_QUIT:
                    SDL_Quit();
                    return EXIT_SUCCESS;// Quit the program
                    break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                            SDL_Quit();
                            return EXIT_SUCCESS;// Quit the program
                            break;
                    }
                case SDL_MOUSEMOTION:
                {
                    unsigned int case_x, case_y;
                    printf("%d [%d] %d [%d]\n", event.motion.x, event.motion.xrel, event.motion.y, event.motion.yrel);
                    case_x = event.motion.x % TILE_X_SIZE;
                    case_y = event.motion.y % TILE_Y_SIZE;

                    if(current_line_x >= 0 && current_line_y >= 0)
                        draw_line(current_line_x, current_line_y);
                    if(current_column_x >= 0 && current_column_y >= 0)
                        draw_column(current_column_x, current_column_y);
                    if(case_x < TILE_BOUND_SIZE && (case_y >= TILE_BOUND_SIZE && case_y < TILE_Y_SIZE - TILE_BOUND_SIZE))//left column
                    {
                        current_column_x = event.motion.x / TILE_X_SIZE;
                        current_column_y = event.motion.y / TILE_Y_SIZE;
                        current_line_x = -1;
                        current_line_y = -1;
                    }
                    else if(case_x > TILE_X_SIZE - TILE_BOUND_SIZE && (case_y >= TILE_BOUND_SIZE && case_y < TILE_Y_SIZE - TILE_BOUND_SIZE))//right column
                    {
                        current_column_x = (event.motion.x / TILE_X_SIZE) + 1;
                        current_column_y = event.motion.y / TILE_Y_SIZE;
                        current_line_x = -1;
                        current_line_y = -1;
                    }
                    else if(case_y > TILE_Y_SIZE - TILE_BOUND_SIZE && (case_x >= TILE_BOUND_SIZE && case_x < TILE_X_SIZE - TILE_BOUND_SIZE))//bottom line
                    {
                        current_line_x = event.motion.x / TILE_X_SIZE;
                        current_line_y = (event.motion.y / TILE_Y_SIZE) + 1;
                        current_column_x = -1;
                        current_column_y = -1;
                    }
                    else if(case_y < TILE_Y_SIZE && (case_x >= TILE_BOUND_SIZE && case_x < TILE_X_SIZE - TILE_BOUND_SIZE))//top line
                    {
                        current_line_x = event.motion.x / TILE_X_SIZE;
                        current_line_y = event.motion.y / TILE_Y_SIZE;
                        current_column_x = -1;
                        current_column_y = -1;
                    }
                    if(current_line_x >= 0 && current_line_y >= 0)
                        draw_selected_line(current_line_x, current_line_y);
                    if(current_column_x >= 0 && current_column_y >= 0)
                        draw_selected_column(current_column_x, current_column_y);
                    break;
                    case SDL_MOUSEBUTTONDOWN:
                    {
                        int change_player = 0;
                        if(current_line_x >= 0 && current_line_x <= SCREEN_X_SIZE && current_line_y >= 0 && current_line_y <= SCREEN_Y_SIZE)
                        {
                            SDL_PumpEvents(); 
                            if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT) && lines[current_line_x][current_line_y] == 0)
                            {
                                change_player = 1;
                                lines[current_line_x][current_line_y] = current_player;
                                printf("click line %d %d for %d\n", current_line_x, current_line_y, current_player);
                                draw_line(current_line_x, current_line_y);
                                if(check_case(current_line_x, current_line_y) == 1 && world[current_line_x][current_line_y] == 0)
                                {
                                    world[current_line_x][current_line_y] = current_player;
                                    draw_case(current_line_x, current_line_y);
                                }
                                if(current_line_y > 0 && check_case(current_line_x, current_line_y - 1) == 1 && world[current_line_x][current_line_y - 1] == 0)
                                {
                                    world[current_line_x][current_line_y - 1] = current_player;
                                    draw_case(current_line_x, current_line_y - 1);
                                }
                                if(current_line_y < SCREEN_Y_SIZE - 1 && check_case(current_line_x, current_line_y + 1) == 1 && world[current_line_x][current_line_y + 1] == 0)
                                {
                                    world[current_line_x][current_line_y + 1] = current_player;
                                    draw_case(current_line_x, current_line_y + 1);
                                }
                            }
                        }
                        if(current_column_x >= 0 && current_column_x <= SCREEN_X_SIZE && current_column_y >= 0 && current_column_y <= SCREEN_Y_SIZE)
                        {
                            SDL_PumpEvents(); 
                            if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT) && columns[current_column_x][current_column_y] == 0)
                            {
                                change_player = 1;
                                columns[current_column_x][current_column_y] = current_player;
                                printf("click column %d %d for %d\n", current_column_x, current_column_y, current_player);
                                draw_column(current_column_x, current_column_y);
                                if(check_case(current_column_x, current_column_y) == 1 && world[current_column_x][current_column_y] == 0)
                                {
                                    world[current_column_x][current_column_y] = current_player;
                                    draw_case(current_column_x, current_column_y);
                                }
                                if(current_column_x > 0 && check_case(current_column_x - 1, current_column_y) == 1 && world[current_column_x - 1][current_column_y] == 0)
                                {
                                    world[current_column_x - 1][current_column_y] = current_player;
                                    draw_case(current_column_x - 1, current_column_y);
                                }
                                if(current_column_x < SCREEN_X_SIZE - 1 && check_case(current_column_x + 1, current_column_y) == 1 && world[current_column_x + 1][current_column_y] == 0)
                                {
                                    world[current_column_x + 1][current_column_y] = current_player;
                                    draw_case(current_column_x + 1, current_column_y);
                                }
                            }
                        }
                        if(change_player)
                        {
                            current_player++;
                            if(current_player > max_players)
                                current_player = 1;
                        }
                    }
                    break;
                }
            }
        }
        
        
        //Update the display
	SDL_RenderPresent(renderer);
        
    }

    // Tell the SDL to clean up and shut down
    SDL_Quit();

    return EXIT_SUCCESS;
}
