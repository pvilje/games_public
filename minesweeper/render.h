#pragma once

typedef enum {
    SKYBOX_U,
    SKYBOX_D,
    SKYBOX_N,
    SKYBOX_S,
    SKYBOX_W,
    SKYBOX_E,
    SKYBOX_COUNT,
} skybox_tag_t;

typedef Texture skybox_t[SKYBOX_COUNT];

void renderCube(float x, float y, float z, float scale, float red, float green, float blue, float alpha)
{
    float min_x = x - scale;
    float min_y = y - scale;
    float min_z = z - scale;
    float max_x = x + scale;
    float max_y = y + scale;
    float max_z = z + scale;

    rlBegin(RL_QUADS);

    rlColor4f(red, green, blue, alpha);
    rlVertex3f(min_x, min_y, max_z);
    rlVertex3f(max_x, min_y, max_z);
    rlVertex3f(max_x, max_y, max_z);
    rlVertex3f(min_x, max_y, max_z);

    rlColor4f(0.9 * red, 0.9 * green, 0.9 * blue, alpha);
    rlVertex3f(min_x, max_y, min_z);
    rlVertex3f(max_x, max_y, min_z);
    rlVertex3f(max_x, min_y, min_z);
    rlVertex3f(min_x, min_y, min_z);

    rlColor4f(0.8 * red, 0.8 * green, 0.8 * blue, alpha);
    rlVertex3f(min_x, min_y, min_z);
    rlVertex3f(max_x, min_y, min_z);
    rlVertex3f(max_x, min_y, max_z);
    rlVertex3f(min_x, min_y, max_z);
                     
    rlColor4f(0.7 * red, 0.7 * green, 0.7 * blue, alpha);
    rlVertex3f(min_x, max_y, max_z);
    rlVertex3f(max_x, max_y, max_z);
    rlVertex3f(max_x, max_y, min_z);
    rlVertex3f(min_x, max_y, min_z);

    rlColor4f(0.6 * red, 0.6 * green, 0.6 * blue, alpha);
    rlVertex3f(min_x, min_y, max_z);
    rlVertex3f(min_x, max_y, max_z);
    rlVertex3f(min_x, max_y, min_z);
    rlVertex3f(min_x, min_y, min_z);
                       
    rlColor4f(0.5 * red, 0.5 * green, 0.5 * blue, alpha);
    rlVertex3f(max_x, min_y, min_z);
    rlVertex3f(max_x, max_y, min_z);
    rlVertex3f(max_x, max_y, max_z);
    rlVertex3f(max_x, min_y, max_z);

    rlEnd();
}

void renderSkybox(skybox_t skybox, Camera3D camera, Color color)
{
    float min_x = camera.position.x - 250;
    float min_y = camera.position.y - 250;
    float min_z = camera.position.z - 250;
    float max_x = camera.position.x + 256;
    float max_y = camera.position.y + 256;
    float max_z = camera.position.z + 256;

    rlColor4ub(color.r, color.g, color.g, color.a);

    // up:
    rlSetTexture(skybox[SKYBOX_U].id);
    rlBegin(RL_QUADS);
    rlTexCoord2f(1, 1); rlVertex3f(min_x, max_y, max_z);
    rlTexCoord2f(0, 1); rlVertex3f(max_x, max_y, max_z);
    rlTexCoord2f(0, 0); rlVertex3f(max_x, min_y, max_z);
    rlTexCoord2f(1, 0); rlVertex3f(min_x, min_y, max_z);
    rlEnd();

    // down
    rlSetTexture(skybox[SKYBOX_D].id);
    rlBegin(RL_QUADS);
    rlTexCoord2f(0, 0); rlVertex3f(min_x, min_y, min_z);
    rlTexCoord2f(1, 0); rlVertex3f(max_x, min_y, min_z);
    rlTexCoord2f(1, 1); rlVertex3f(max_x, max_y, min_z);
    rlTexCoord2f(0, 1); rlVertex3f(min_x, max_y, min_z);
    rlEnd();

    // south
    rlSetTexture(skybox[SKYBOX_S].id);
    rlBegin(RL_QUADS);
    rlTexCoord2f(0, 0); rlVertex3f(min_x, min_y, max_z);
    rlTexCoord2f(1, 0); rlVertex3f(max_x, min_y, max_z);
    rlTexCoord2f(1, 1); rlVertex3f(max_x, min_y, min_z);
    rlTexCoord2f(0, 1); rlVertex3f(min_x, min_y, min_z);
    rlEnd();
                     
    // north
    rlSetTexture(skybox[SKYBOX_N].id);
    rlBegin(RL_QUADS);
    rlTexCoord2f(1, 1); rlVertex3f(min_x, max_y, min_z);
    rlTexCoord2f(0, 1); rlVertex3f(max_x, max_y, min_z);
    rlTexCoord2f(0, 0); rlVertex3f(max_x, max_y, max_z);
    rlTexCoord2f(1, 0); rlVertex3f(min_x, max_y, max_z);
    rlEnd();

    // west
    rlSetTexture(skybox[SKYBOX_W].id);
    rlBegin(RL_QUADS);
    rlTexCoord2f(1, 1); rlVertex3f(min_x, min_y, min_z);
    rlTexCoord2f(0, 1); rlVertex3f(min_x, max_y, min_z);
    rlTexCoord2f(0, 0); rlVertex3f(min_x, max_y, max_z);
    rlTexCoord2f(1, 0); rlVertex3f(min_x, min_y, max_z);
    rlEnd();

    // east
    rlSetTexture(skybox[SKYBOX_E].id);
    rlBegin(RL_QUADS);
    rlTexCoord2f(0, 0); rlVertex3f(max_x, min_y, max_z);
    rlTexCoord2f(1, 0); rlVertex3f(max_x, max_y, max_z);
    rlTexCoord2f(1, 1); rlVertex3f(max_x, max_y, min_z);
    rlTexCoord2f(0, 1); rlVertex3f(max_x, min_y, min_z);
    rlEnd();

    rlSetTexture(0);
}

typedef struct {
    Rectangle source;
    Vector3 position;
    Vector3 up;
    Vector2 size; 
    Vector2 origin;
    float rotation;
    Color tint;
} billboard_t;

static struct {
    Camera3D camera;
    Texture2D texture;

    int count;
    billboard_t array[1024 * 1024];
} billboard;

static int cmpBillboardDepth(const void* va, const void* vb)
{
    const billboard_t* a = (const billboard_t*)va;
    const billboard_t* b = (const billboard_t*)vb;
    float a_depth = Vector3Distance(billboard.camera.position, a->position);
    float b_depth = Vector3Distance(billboard.camera.position, b->position);
    return a_depth < b_depth? 1 : -1;
}

void beginBillboard(Camera3D camera, Texture2D texture)
{
    billboard.camera = camera;
    billboard.texture = texture;
    billboard.count = 0;
}

void endBillboard(void)
{
    qsort(billboard.array, billboard.count, sizeof (billboard_t), cmpBillboardDepth);

    // Disable depth writes and use alpha blending for correct transparent billboards
    rlDisableDepthMask();
    rlSetBlendMode(RL_BLEND_ALPHA);

    for (int i = 0; i < billboard.count; ++i)
    {
        billboard_t* e = &billboard.array[i];
        DrawBillboardPro(
            billboard.camera,
            billboard.texture,
            e->source,
            e->position,
            e->up,
            e->size,
            e->origin,
            e->rotation,
            e->tint         
        );
    }

    rlEnableDepthMask();
}

void pushBillboard(billboard_t b)
{
    billboard.array[billboard.count++] = b;
}

