#ifndef MINESWEEPER_H
#define MINESWEEPER_H 1

#define MINE 9

typedef enum {
    GAME_NEW = 0,
    GAME_ONGOING,
    GAME_3D,
    GAME_OVER_3D,
    GAME_WON_3D,
    GAME_OVER,
    GAME_WON,
    QUIT
} gameState_e;

typedef struct {
    int width;
    int height;
    char name[12];
} window_t;

typedef enum {
    BUTTON_UNPRESSED = 0,
    BUTTON_FLAGGED,
    OPEN_NO_MINE,
    OPEN_WITH_HINT,
    OPEN_MINE
} boxType_e;

typedef struct {
    int rows;
    int cols;
    int mines;
    int cells;
    unsigned char *minePositions;
    int *boxContent;
    unsigned int *grassSeeds;
    int spacing;
    int buttonSize;
    int headerHeight;
} layout_t;

typedef struct {
    vec3_t pos;
    vec3_t vel;
    vec3_t dir;
    float fallTimer;

    struct {
        unsigned alive : 1;
        
        unsigned can_jump : 1;
    };
} player_t;

typedef struct {
    double timer;
    double timeDiff; 
    int minesleft;
} gameStats_t;

typedef struct {
    float volume;
    bool mute;
} settings_t;

typedef struct {
    gameState_e state;
    layout_t layout;
    window_t window;
    gameStats_t stats;
    settings_t settings;
    // 3D mode stuff:
    player_t player;
} game_t;

typedef struct {
    int active;
    bool edit;
} dropdown_t;

#endif
