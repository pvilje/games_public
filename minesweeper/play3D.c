
#define PLAYER_HEIGHT (1.7)

static Texture2D texture_grass_ground;
static Texture2D *texture_grass_billboards = NULL;
static int texture_grass_billboards_count = 0;
static Texture2D texture_dirt;
static skybox_t texture_skybox;

static Camera3D player_camera;
static RayCollision player_target;

/* Pointer to the active game used for HUD drawing in 3D mode */
static game_t *g_currentGame = NULL;

static void textToDisplay(const char *text, char setget)
{
    static char displayText[256];
    if (setget == 's')
    {
        strncpy(displayText, text, sizeof(displayText) - 1);
        displayText[sizeof(displayText) - 1] = '\0';
        return;
    }

    if (setget == 'g')
    {
        /* Row 1: persistent mines-left HUD */
        char minesLeftBuf[64] = "Mines left: 0/0";
        if (g_currentGame)
        {
            snprintf(minesLeftBuf, sizeof(minesLeftBuf), "Mines left: %d/%d",
                     g_currentGame->stats.minesleft, g_currentGame->layout.mines);
        }
        DrawText(minesLeftBuf, 16, 16, 20, LIME);

        /* Row 2: the queued message from prior `textToDisplay(...,'s')` calls */
        DrawText(displayText, 16, 16 + 24, 20, WHITE);
    }
}

int checkMouseClickResult(game_t *pGame)
{
    int idx, x, y;
    x = (int)player_target.point.x;
    y = (int)player_target.point.y;
    coordinateToArrayIndex(pGame, &idx, x, y);
    return pGame->layout.minePositions[idx];
}

static Texture2D loadTexture(const char *path)
{
    Texture2D texture = LoadTexture(path);

    GenTextureMipmaps(&texture);
    SetTextureFilter(texture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(texture, TEXTURE_FILTER_BILINEAR);
    SetTextureWrap(texture, TEXTURE_WRAP_CLAMP);

    return texture;
}

#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>

static void loadGrassBillboards(void)
{
    const char *dir = "assets/textures/grass";
    char searchPath[512];
    snprintf(searchPath, sizeof(searchPath), "%s\\*.png", dir);

    Texture2D *arr = NULL;
    int count = 0;
    int cap = 0;

    struct _finddata_t fd;
    intptr_t h = _findfirst(searchPath, &fd);
    if (h != -1)
    {
        do
        {
            if (!(fd.attrib & _A_SUBDIR))
            {
                char filepath[512];
                snprintf(filepath, sizeof(filepath), "%s\\%s", dir, fd.name);
                if (count + 1 > cap)
                {
                    cap = cap == 0 ? 8 : cap * 2;
                    arr = realloc(arr, cap * sizeof(Texture2D));
                }
                arr[count++] = loadTexture(filepath);
            }
        } while (_findnext(h, &fd) == 0);
        _findclose(h);
    }

    if (count == 0)
    {
        arr = realloc(arr, sizeof(Texture2D));
        arr[0] = loadTexture("assets/textures/grass/n_grass_diff_0_01.png");
        count = 1;
    }

    texture_grass_billboards = arr;
    texture_grass_billboards_count = count;
}

void loadTextures(void)
{
    texture_grass_ground = loadTexture("assets/textures/ground/Grass_01.png");
    SetTextureWrap(texture_grass_ground, TEXTURE_WRAP_REPEAT);
    loadGrassBillboards();
    texture_dirt = loadTexture("assets/textures/ground/dirt.png");
    SetTextureWrap(texture_dirt, TEXTURE_WRAP_REPEAT);

    texture_skybox[SKYBOX_U] = loadTexture("assets/textures/skybox/ypos.png");
    texture_skybox[SKYBOX_D] = loadTexture("assets/textures/skybox/yneg.png");
    texture_skybox[SKYBOX_N] = loadTexture("assets/textures/skybox/zpos.png");
    texture_skybox[SKYBOX_S] = loadTexture("assets/textures/skybox/zneg.png");
    texture_skybox[SKYBOX_W] = loadTexture("assets/textures/skybox/xpos.png");
    texture_skybox[SKYBOX_E] = loadTexture("assets/textures/skybox/xneg.png");
}

static Camera3D getPlayerCamera(player_t *pPlayer)
{
    return (Camera3D){
        .position = Vector3Add(pPlayer->pos, vec3(0, 0, PLAYER_HEIGHT)),
        .up = {0, 0, 1},
        .target = {
            pPlayer->pos.x + pPlayer->dir.x,
            pPlayer->pos.y + pPlayer->dir.y,
            pPlayer->pos.z + pPlayer->dir.z + PLAYER_HEIGHT,
        },
        .fovy = 80,
        .projection = CAMERA_PERSPECTIVE,
    };
}

static void updatePlayer(game_t *pGame, float dt)
{
    player_t *pPlayer = &pGame->player;

    vec3_t up = vec3(0, 0, 1);
    vec3_t side = Vector3Normalize(Vector3CrossProduct(pPlayer->dir, up));

    // keyboard input:
    {
        vec2_t move = {0};

        if (IsKeyDown(KEY_W))
        {
            move.x += pPlayer->dir.x;
            move.y += pPlayer->dir.y;
        }

        if (IsKeyDown(KEY_S))
        {
            move.x -= pPlayer->dir.x;
            move.y -= pPlayer->dir.y;
        }

        if (IsKeyDown(KEY_D))
        {
            move.x += side.x;
            move.y += side.y;
        }

        if (IsKeyDown(KEY_A))
        {
            move.x -= side.x;
            move.y -= side.y;
        }

        if (IsKeyPressed(KEY_SPACE) && pPlayer->can_jump)
        {
            pPlayer->can_jump = 0;
            pPlayer->vel.z = 6;
        }

        move = Vector2Normalize(move);

        /* Allow running when Ctrl is held */
        float baseSpeed = 24.0f;
        float runMultiplier = (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) ? 2.0f : 1.0f;
        float speed = baseSpeed * runMultiplier;
        pPlayer->vel.x += speed * move.x * dt;
        pPlayer->vel.y += speed * move.y * dt;
    }

    // mouse input:
    {
        vec2_t rot = GetMouseDelta();

        pPlayer->dir = Vector3RotateByAxisAngle(pPlayer->dir, side, -0.001 * rot.y);
        pPlayer->dir = Vector3RotateByAxisAngle(pPlayer->dir, up, -0.001 * rot.x);
        pPlayer->dir = Vector3Normalize(pPlayer->dir);

        player_camera = getPlayerCamera(pPlayer);
        
        /* Only allow clicks in active 3D gameplay */
        if (pGame->state == GAME_3D)
        {
                if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
                {
                    int idx, x, y;
                    x = (int)player_target.point.x;
                    y = (int)player_target.point.y;
                    coordinateToArrayIndex(pGame, &idx, x, y);

                    if (idx >= 0 && idx < pGame->layout.cells)
                    {
                        /* Ignore left-clicks on flagged cells */
                        if (pGame->layout.boxContent[idx] == BUTTON_FLAGGED)
                        {
                            /* do nothing */
                        }
                        else if (pGame->layout.minePositions[idx] == MINE)
                        {
                            /* mark mine as opened and end game in 3D */
                            pGame->layout.boxContent[idx] = OPEN_MINE;
                            textToDisplay("BOOM!", 's');
                            pGame->state = GAME_OVER_3D;
                        }
                        else
                        {
                            /* non-mine: if it's a zero, flood-fill; otherwise open with hint */
                            if (pGame->layout.minePositions[idx] == 0)
                            {
                                floodFill(pGame, idx);
                            }
                            else
                            {
                                pGame->layout.boxContent[idx] = OPEN_WITH_HINT;
                            }
                        }
                    }
                }

                /* Right-click toggles flag in 3D mode */
                if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT))
                {
                    int idx, x, y;
                    x = (int)player_target.point.x;
                    y = (int)player_target.point.y;
                    coordinateToArrayIndex(pGame, &idx, x, y);

                    if (idx >= 0 && idx < pGame->layout.cells)
                    {
                        if (pGame->layout.boxContent[idx] == BUTTON_FLAGGED)
                        {
                            pGame->layout.boxContent[idx] = BUTTON_UNPRESSED;
                            pGame->stats.minesleft++;
                        }
                        else if (pGame->layout.boxContent[idx] == BUTTON_UNPRESSED)
                        {
                            pGame->layout.boxContent[idx] = BUTTON_FLAGGED;
                            pGame->stats.minesleft--;
                        }
                    }
                }
        }

        /* After handling click, check win condition (only while still in 3D playing) */
        if (pGame->state == GAME_3D)
        {
            int leftToOpen = 0;
            for (int i = 0; i < pGame->layout.cells; ++i)
            {
                if (pGame->layout.boxContent[i] == BUTTON_FLAGGED || pGame->layout.boxContent[i] == BUTTON_UNPRESSED)
                    leftToOpen++;
            }
            if ((leftToOpen - pGame->layout.mines) == 0)
            {
                pGame->state = GAME_WON_3D;
            }
        }
    }

    {
        Ray ray = {
            .position = player_camera.position,
            .direction = pPlayer->dir,
        };

        player_target = GetRayCollisionQuad(
            ray,
            vec3(0, 0),
            vec3(0, 64),
            vec3(64, 64),
            vec3(64, 0));
    }

        // player physics:
    {
            // update position:
            pPlayer->pos.x += pPlayer->vel.x * dt;
            pPlayer->pos.y += pPlayer->vel.y * dt;
            pPlayer->pos.z += pPlayer->vel.z * dt;

            // apply friction:
            pPlayer->vel.x -= 8 * pPlayer->vel.x * dt;
            pPlayer->vel.y -= 8 * pPlayer->vel.y * dt;

            /* If player walks outside the map bounds, start a fall to death
             * with a gentle initial drop that accelerates over time. */
            bool outside = (pPlayer->pos.x < 0.0f || pPlayer->pos.x > (float)pGame->layout.cols ||
                            pPlayer->pos.y < 0.0f || pPlayer->pos.y > (float)pGame->layout.rows);

            if (outside)
            {
                if (pPlayer->alive)
                {
                    pPlayer->alive = 0; /* mark as falling/dead */
                    pPlayer->can_jump = 0;
                    /* remove horizontal control so the player can't fly back */
                    pPlayer->vel.x = 0;
                    pPlayer->vel.y = 0;
                    /* start fall timer and very small downward velocity so the
                     * fall begins slowly (player won't instantly plummet). */
                    pPlayer->fallTimer = 0.0f;
                    /* Increase initial impulse: make fall start 100x faster */
                    if (pPlayer->vel.z > -0.1f) pPlayer->vel.z = -0.1f;
                }
            }

            /* Gravity: apply only when airborne (pos.z>0) or when falling outside */
            if (outside)
            {
                /* stronger, more tangible gravity while falling outside the map */
                pPlayer->fallTimer += dt;
                float fallGravity = 0.5f + pPlayer->fallTimer * 2.0f; /* m/s^2-ish */
                if (fallGravity > 20.0f) fallGravity = 20.0f;
                pPlayer->vel.z -= fallGravity * dt;
            }
            else if (pPlayer->pos.z > 0.0f || !pPlayer->can_jump)
            {
                /* normal on-map gravity when airborne or during jump */
                pPlayer->vel.z -= 10.0f * dt;
            }
            else
            {
                /* on ground: ensure no vertical velocity accumulation */
                pPlayer->vel.z = 0.0f;
            }

            /* Only clamp to ground when player is alive (standing on map). */
            if (pPlayer->pos.z < 0.0f)
            {
                if (pPlayer->alive)
                {
                    pPlayer->can_jump = 1;
                    pPlayer->pos.z = 0.0f;
                }
            }

            /* When the player has fallen far enough below the world, end game */
            if (outside && pPlayer->pos.z < -10.0f)
            {
                textToDisplay("You fell to your death", 's');
                pGame->state = GAME_OVER_3D;
            }
    }
}

static void update3D(game_t *pGame)
{
    float dt = GetFrameTime();
    updatePlayer(pGame, dt);
}

static void renderMap(game_t *pGame)
{
    /* Draw each tile with appropriate texture (dirt for opened cells) */
    rlDisableBackfaceCulling();
    for (int y = 0; y < pGame->layout.rows; ++y)
    {
        for (int x = 0; x < pGame->layout.cols; ++x)
        {
            int idx;
            coordinateToArrayIndex(pGame, &idx, x, y);
            Texture2D *tex = &texture_grass_ground;
            bool isDirt = false;
            if (pGame->layout.boxContent && (pGame->layout.boxContent[idx] == OPEN_NO_MINE || pGame->layout.boxContent[idx] == OPEN_WITH_HINT))
            {
                tex = &texture_dirt;
                isDirt = true;
            }

            rlSetTexture(tex->id);
            rlBegin(RL_QUADS);
            /* avoid tinting dirt texture; use white color for true pixels */
            if (isDirt)
                rlColor3f(1.0f, 1.0f, 1.0f);
            else
                rlColor3f(0.6f, 1.0f, 0.7f);
            rlTexCoord2f(0, 0);
            rlVertex3f(x + 0, y + 0, 0);
            rlTexCoord2f(1, 0);
            rlVertex3f(x + 1, y + 0, 0);
            rlTexCoord2f(1, 1);
            rlVertex3f(x + 1, y + 1, 0);
            rlTexCoord2f(0, 1);
            rlVertex3f(x + 0, y + 1, 0);
            rlEnd();
            rlSetTexture(0);
        }
    }
    /* Draw a thin underside textured with dirt so the island is visible
     * when looking up while falling. This is one large quad just below
     * the main ground (z = -0.05). */
    {
        float z = -0.05f;
        rlSetTexture(texture_dirt.id);
        rlBegin(RL_QUADS);
        rlColor3f(1.0f, 1.0f, 1.0f);
        rlTexCoord2f(0, 0);
        rlVertex3f(0.0f, 0.0f, z);
        rlTexCoord2f((float)pGame->layout.cols, 0);
        rlVertex3f((float)pGame->layout.cols, 0.0f, z);
        rlTexCoord2f((float)pGame->layout.cols, (float)pGame->layout.rows);
        rlVertex3f((float)pGame->layout.cols, (float)pGame->layout.rows, z);
        rlTexCoord2f(0, (float)pGame->layout.rows);
        rlVertex3f(0.0f, (float)pGame->layout.rows, z);
        rlEnd();
        rlSetTexture(0);
    }
    rlEnableBackfaceCulling();

    // render billboards:
    {
        float t = GetTime();
        for (int y = 0; y < pGame->layout.rows; ++y)
        {
            for (int x = 0; x < pGame->layout.cols; ++x)
            {
                int idx;
                unsigned hash = hash2i(x, y);
                float ht = randf(&hash, -1, 1) * (t + randf(&hash, -PI, PI));
                coordinateToArrayIndex(pGame, &idx, x, y);

                if (pGame->layout.boxContent[idx] != BUTTON_UNPRESSED)
                {
                    continue; // don't draw billboards on opened/flagged cells
                }

                /* 50% chance to skip drawing a billboard for visual sparsity */
                if (pGame->layout.grassSeeds && (pGame->layout.grassSeeds[idx] & 1) == 0)
                {
                    continue;
                }

                billboard_t bb = {0};
                int chosenTex = 0;
                if (texture_grass_billboards_count > 0 && pGame->layout.grassSeeds)
                {
                    chosenTex = (int)(pGame->layout.grassSeeds[idx] % (unsigned)texture_grass_billboards_count);
                }
                Texture2D *tex = &texture_grass_billboards[chosenTex];
                bb.source = (Rectangle){0, 0, (float)tex->width, (float)tex->height};
                bb.position = (Vector3){ x + 0.5f + randf(&hash, -0.1f, 0.1f), y + 0.5f + randf(&hash, -0.1f, 0.1f), 0 };
                bb.up = (Vector3){ 0.1f * cosf(ht) + randf(&hash, -0.2f, 0.2f), 0.1f * sinf(ht) + randf(&hash, -0.2f, 0.2f), 1 };
                bb.size = (Vector2){ 1.05f, 1.1f + randf(&hash, -0.1f, 0.2f) };
                bb.origin = (Vector2){ 0.5f, 0.1f };
                bb.tint = WHITE;

                beginBillboard(player_camera, *tex);
                pushBillboard(bb);
                endBillboard();
            }
        }
    }
}

static void render3D(game_t *pGame)
{
    ClearBackground(BLACK);
    float time = GetTime();

    BeginMode3D(player_camera);
    {
        rlSetBlendMode(RL_BLEND_ALPHA);
        renderSkybox(texture_skybox, player_camera, (Color){255, 255, 255, 255});
        if (player_target.hit)
        {
            renderCube(player_target.point.x, player_target.point.y, player_target.point.z, 0.1, 1, 1, 1, 1);
        }
        renderMap(pGame);
    }
    EndMode3D();
    /* draw any queued text from textToDisplay() */
    textToDisplay("", 'g');

    /* Draw number clues for opened cells by projecting world positions to screen */
    for (int y = 0; y < pGame->layout.rows; ++y)
    {
        for (int x = 0; x < pGame->layout.cols; ++x)
        {
            int idx;
            coordinateToArrayIndex(pGame, &idx, x, y);
            if (!(pGame->layout.boxContent[idx] == OPEN_NO_MINE || pGame->layout.boxContent[idx] == OPEN_WITH_HINT))
                continue;

            int val = pGame->layout.minePositions[idx];
            if (val <= 0 || val == MINE)
                continue;

            Vector3 worldPos = { x + 0.5f, y + 0.5f, 0.25f };
            /* Skip if point is behind the camera to avoid mirrored projections */
            Vector3 camForward = Vector3Subtract(player_camera.target, player_camera.position);
            Vector3 toPoint = Vector3Subtract(worldPos, player_camera.position);
            if (Vector3DotProduct(camForward, toPoint) <= 0.0f) continue;
            Vector2 screenPos = GetWorldToScreen(worldPos, player_camera);
            int sw = GetScreenWidth();
            int sh = GetScreenHeight();
            if (screenPos.x < 0 || screenPos.x > sw || screenPos.y < 0 || screenPos.y > sh)
                continue;

            char txt[2] = { (char)('0' + val), '\0' };
            int fontSize = (int)(sh * 0.06f); /* scale with screen height for 3D mode */
            if (fontSize < 20) fontSize = 20;
            Color color;
            switch (val)
            {
                case 1: color = BLUE; break;
                case 2: color = DARKGREEN; break;
                case 3: color = RED; break;
                case 4: color = PURPLE; break;
                case 5: color = MAROON; break;
                case 6: color = SKYBLUE; break;
                case 7: color = BLACK; break;
                case 8: color = GRAY; break;
                default: color = WHITE; break;
            }
            int tw = MeasureText(txt, fontSize);
            DrawText(txt, (int)(screenPos.x - tw / 2), (int)(screenPos.y - fontSize / 2), fontSize, color);
        }
    }

    /* Draw flags for flagged cells (screen-space, similar to numbers) */
    for (int y = 0; y < pGame->layout.rows; ++y)
    {
        for (int x = 0; x < pGame->layout.cols; ++x)
        {
            int idx;
            coordinateToArrayIndex(pGame, &idx, x, y);
            if (pGame->layout.boxContent[idx] != BUTTON_FLAGGED)
                continue;

            Vector3 worldPos = { x + 0.5f, y + 0.5f, 0.25f };
            Vector3 camForward = Vector3Subtract(player_camera.target, player_camera.position);
            Vector3 toPoint = Vector3Subtract(worldPos, player_camera.position);
            if (Vector3DotProduct(camForward, toPoint) <= 0.0f) continue;
            Vector2 screenPos = GetWorldToScreen(worldPos, player_camera);
            int sw = GetScreenWidth();
            int sh = GetScreenHeight();
            if (screenPos.x < 0 || screenPos.x > sw || screenPos.y < 0 || screenPos.y > sh)
                continue;

            int fontSize = (int)(sh * 0.06f);
            if (fontSize < 20) fontSize = 20;
            /* Use the same icon as 2D mode: ICON_GEAR_EX, tint RED */
            float iconScale = (float)fontSize / 16.0f; /* 16 is base icon size */
            int iconX = (int)(screenPos.x - fontSize / 2);
            int iconY = (int)(screenPos.y - fontSize / 2);
            GuiDrawIcon(ICON_GEAR_EX, iconX, iconY, iconScale, RED);
        }
    }

}

/**
 * @brief init 3D mode!
 *
 * @param pGame pointer to game thing..
 */
void init3D(game_t *pGame)
{
    DisableCursor();

    player_t *pPlayer = &pGame->player;

    /* Place player roughly in the center of the game map and reset velocity */
    *pPlayer = (player_t){
        .pos = { (float)pGame->layout.cols / 2.0f, (float)pGame->layout.rows / 2.0f, 0.5f },
        .dir = { 0, 1, 0 },
        .vel = { 0, 0, 0 },
        .fallTimer = 0.0f,
        .alive = 1,
        .can_jump = 1,
    };

    /* Initialize camera to match player start */
    player_camera = getPlayerCamera(pPlayer);
    /* track the active game for HUD drawing */
    g_currentGame = pGame;
}

/**
 * @brief play 3D mode!
 *
 * @param pGame pointer to game thing..
 */
void play3D(game_t *pGame)
{
    update3D(pGame);
    render3D(pGame);
}
