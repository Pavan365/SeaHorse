    /*******************************************************************************************
 *
 *   LayoutName v1.0.0 - Tool Description
 *
 *   LICENSE: Propietary License
 *
 *   Copyright (c) 2022 raylib technologies. All Rights Reserved.
 *
 *   Unauthorized copying of this file, via any medium is strictly prohibited
 *   This project is proprietary and confidential unless the owner allows
 *   usage in any other form by expresely written permission.
 *
 **********************************************************************************************/

#include "raylib/raylib.h"
#define TEXT_SPACING_VALUE 0
#include "include/renderers.cpp"
#include "include/random.cpp"

#define RAYGUI_IMPLEMENTATION
#include "raylib/raygui.h"
#undef RAYGUI_IMPLEMENTATION

#include <iostream>

Font fontTtf;
//----------------------------------------------------------------------------------
// Useful Functions Implementation
//----------------------------------------------------------------------------------
#define QUIT() {CloseWindow();return 0;};

void setStyles(){
    GuiSetStyle(DEFAULT,LINE_COLOR,255);    // Black lines

    fontTtf = LoadFontEx("UbuntuMonoBold.ttf",20,0,0);

    GuiSetFont(fontTtf);
    GuiSetStyle(DEFAULT,TEXT_SIZE,15);
    GuiSetStyle(DEFAULT,TEXT_SPACING,TEXT_SPACING_VALUE);
}
//----------------------------------------------------------------------------------
// Draw Functions Declaration
//----------------------------------------------------------------------------------


float screenWidth = 800;
float screenHeight = 450;

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    // Initialization
    //---------------------------------------------------------------------------------------
    int screenWidth = 1280;
    int screenHeight = 720;
    int monitor = GetCurrentMonitor();
    Renderer renderers = {};

    SetTraceLogLevel(LOG_WARNING);

    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI  | FLAG_WINDOW_TOPMOST);
    InitWindow(screenWidth, screenHeight, "Control Panel");
    
    SetExitKey(KEY_NULL);
    setStyles();

    SetTargetFPS(30);

    // EnableEventWaiting();
    //--------------------------------------------------------------------------------------

    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

    // These deal with their own re-rendering
    PerfStats perfStats(0,0);
    renderers.subscribe(&perfStats);
    
    graph mainplot(Rectangle{150, 100, 600, 300},"position (nm)","Amplitude");
    renderers.subscribe(&mainplot);
    mainplot.plot(true);

    graph heatmapplot(Rectangle{150, 400, 600, 300},"position (nm)","Amplitude");
    renderers.subscribe(&heatmapplot);
    heatmapplot.heatmap();

    // Main loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        perfStats.update();
        //----------------------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        // Check if we've invalidated the screen buffer and need to redraw
        if(GetScreenHeight()!=screenHeight || GetScreenWidth()!=screenWidth || GetCurrentMonitor()!=monitor) {
            renderers.draw();
            screenHeight = GetScreenHeight();
            screenWidth = GetScreenWidth();
            monitor = GetCurrentMonitor();
            std::cout<<"Needed to re-draw (Back Buffer invalidated)"<<std::endl;
        }
        
        if(!IsWindowFocused()) GuiDisable(); else GuiEnable();

        if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_C)) QUIT();
        if(IsKeyPressed(KEY_P)) {mainplot.plot(false);}
        if (IsKeyPressed(KEY_Z)) {perfStats.toggle();}

        GuiDummyRec(Rectangle{50,150,100,100},"Dummy");
        GuiButton(Rectangle{50,250,100,100},"X");

        if (IsFileDropped()) {
            auto files = LoadDroppedFiles();
            for (auto i = 0; i<files.count;i++){
                printf("%s\n",files.paths[i]);

                // IsFileExtension
                // FileExists
                // IsPathFile
                // GetFileName

                // GetApplicationDirectory

                // ChangeDirectory
                // LoadDirectoryFilesEx
                // UnloadDirectoryFiles
            }
            UnloadDroppedFiles(files);
        }



        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

