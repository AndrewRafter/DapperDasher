#include "raylib.h"

struct AnimData {
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};

AnimData updateAnimData(AnimData data, float deltaTime, int maxFrame) {
    data.runningTime += deltaTime;
    if(data.runningTime >= data.updateTime) {
        data.runningTime = 0.0;

        data.rec.x = data.frame * data.rec.width;
        data.frame++;

        if(data.frame > maxFrame) {
            data.frame = 0;
        }
    }

    return data;
}

int main() {
    //window dimensions
    const int windowDimensions[2]{512, 380}; //width, height

    //initialise window
    InitWindow(windowDimensions[0], windowDimensions[1], "Dapper Dasher");

    const int gravity = 1000;

    //sprites
    //nebula
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");

    const int sizeOfNebulae = 3;
    AnimData nebulae[sizeOfNebulae]{};

    for(int i = 0; i < sizeOfNebulae; i++) {
        nebulae[i].rec.x = 0;
        nebulae[i].rec.y = 0;
        nebulae[i].rec.width = nebula.width/8;
        nebulae[i].rec.height = nebula.height/8;
        nebulae[i].pos.x = windowDimensions[0] + (i * 300);
        nebulae[i].pos.y = windowDimensions[1] - nebula.height/8;
        nebulae[i].frame = 0;
        nebulae[i].runningTime = 0.0;
        nebulae[i].updateTime = 1.0/16.0;
    }

    float finishLine = nebulae[sizeOfNebulae - 1].pos.x;

    int nebVel = -200;

    //scarfy
    Texture2D scarfy = LoadTexture("textures/scarfy.png");

    //AnimData for Scarfy
    AnimData scarfyData;
    scarfyData.rec.width = scarfy.width/6;
    scarfyData.rec.height = scarfy.height;
    scarfyData.rec.x = 0;
    scarfyData.rec.y = 0;
    scarfyData.pos.x = windowDimensions[0]/2 - scarfyData.rec.width/2;
    scarfyData.pos.y = windowDimensions[1] - scarfyData.rec.height;
    scarfyData.frame = 0;
    scarfyData.updateTime = 1.0/12.0;
    scarfyData.runningTime = 0.0;

    bool isInAir = false;
    const int jumpVelocity = -600;

    int velocity = 0;

    Texture2D background = LoadTexture("textures/far-buildings.png");
    float bgX = 0.0;
    Texture2D midground = LoadTexture("textures/back-buildings.png");
    float mgX = 0.0;
    Texture2D foreground = LoadTexture("textures/foreground.png");
    float fgX = 0.0;

    bool collision = false;

    SetTargetFPS(60);
    while(!WindowShouldClose()) {
        const float dT = GetFrameTime();

        BeginDrawing();
        ClearBackground(WHITE);

        //scroll the background, midground, foreground
        bgX -= 20 * dT;
        if(bgX <= -background.width * 2) {
            bgX = 0.0;
        }

        mgX -= 40 * dT;
        if(mgX <= -midground.width * 2) {
            mgX = 0.0;
        }

        fgX -= 80 * dT;
        if(fgX <= -foreground.width * 2) {
            fgX = 0.0;
        }

        //draw background
        Vector2 bg1Pos = {bgX, 0.0};
        DrawTextureEx(background, bg1Pos, 0.0, 2.0, WHITE);
        Vector2 bg2Pos = {bgX + background.width * 2, 0.0};
        DrawTextureEx(background, bg2Pos, 0.0, 2.0, WHITE);

        Vector2 mg1Pos = {mgX, 0.0};
        DrawTextureEx(midground, mg1Pos, 0.0, 2.0, WHITE);
        Vector2 mg2Pos = {mgX + midground.width * 2, 0.0};
        DrawTextureEx(midground, mg2Pos, 0.0, 2.0, WHITE);

        Vector2 fg1Pos = {fgX, 0.0};
        DrawTextureEx(foreground, fg1Pos, 0.0, 2.0, WHITE);
        Vector2 fg2Pos = {fgX + foreground.width * 2, 0.0};
        DrawTextureEx(foreground, fg2Pos, 0.0, 2.0, WHITE);
        
        //check if scarfy is on the ground
        if(scarfyData.pos.y >= windowDimensions[1] - scarfyData.rec.height) {
            velocity = 0;
            isInAir = false;
        } else {
            velocity += gravity * dT;
            isInAir = true;
        }

        //jump
        if(IsKeyPressed(KEY_SPACE) && !isInAir) {
            velocity += jumpVelocity;
        }

        //update positions
        for(int i = 0; i < sizeOfNebulae; i++) {
            nebulae[i].pos.x += nebVel * dT;
        }

        finishLine += nebVel * dT;

        scarfyData.pos.y += velocity * dT;

        //update scarfy animation frame
        if(!isInAir){
            scarfyData = updateAnimData(scarfyData, dT, 5); 
        }

        //update nebula animation frame
        for(int i = 0; i < sizeOfNebulae; i++) {
            nebulae[i] = updateAnimData(nebulae[i], dT, 7); 
        }

        for(AnimData nebula : nebulae) {
            float pad = 50.0;
            Rectangle nebRec{
                nebula.pos.x + pad,
                nebula.pos.y + pad,
                nebula.rec.width - 2 * pad,
                nebula.rec.height - 2 * pad
            };

            Rectangle scarfyRec{
                scarfyData.pos.x,
                scarfyData.pos.y,
                scarfyData.rec.width,
                scarfyData.rec.height
            };

            if(CheckCollisionRecs(nebRec, scarfyRec)) {
                collision = true;
            }
        }


        if(collision) {
            DrawText("Game Over!", windowDimensions[0]/4, windowDimensions[1]/2, 40, RED);
        } else if(scarfyData.pos.x >= finishLine) {
            DrawText("You Win!", windowDimensions[0]/4, windowDimensions[1]/2, 40, RED);
        } else {
            //draw nebulae
            for(int i = 0; i < sizeOfNebulae; i++) {
                DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);
            }

            //draw scarfy
            DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);
        }
        
        

        EndDrawing();
    }
    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);
    CloseWindow();
}