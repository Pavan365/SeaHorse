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

#define RAYGUI_IMPLEMENTATION
#include "raylib/raygui.h"

//----------------------------------------------------------------------------------
// Controls Functions Declaration
//----------------------------------------------------------------------------------
static void RUN();                // Button: RUN logic

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    // Initialization
    //---------------------------------------------------------------------------------------
    int screenWidth = 800;
    int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "layout_name");

    // layout_name: controls initialization
    //----------------------------------------------------------------------------------
    // Define controls variables
    bool TextBox007EditMode = false;
    char TextBox007Text[128] = "";            // TextBox: TextBox007
    bool TextBox004EditMode = false;
    char TextBox004Text[128] = "";            // TextBox: TextBox004

    // Define controls rectangles
    Rectangle layoutRecs[7] = {
        (Rectangle){ 384, 144, 224, 136 },    // GroupBox: GroupBox005
        (Rectangle){ 392, 152, 40, 24 },    // Label: PATH
        (Rectangle){ 432, 152, 168, 24 },    // TextBox: TextBox007
        (Rectangle){ 392, 184, 40, 24 },    // Label: SEED
        (Rectangle){ 432, 184, 168, 24 },    // TextBox: TextBox004
        (Rectangle){ 392, 248, 208, 24 },    // StatusBar: StatusBar005
        (Rectangle){ 392, 216, 208, 24 },    // Button: RUN
    };
    //----------------------------------------------------------------------------------

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Implement required update logic
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR))); 

            // raygui: controls drawing
            //----------------------------------------------------------------------------------
            // Draw controls
            GuiGroupBox(layoutRecs[0], "RUN PROGRAM");
            GuiLabel(layoutRecs[1], "PATH");
            if (GuiTextBox(layoutRecs[2], TextBox007Text, 128, TextBox007EditMode)) TextBox007EditMode = !TextBox007EditMode;
            GuiLabel(layoutRecs[3], "SEED");
            if (GuiTextBox(layoutRecs[4], TextBox004Text, 128, TextBox004EditMode)) TextBox004EditMode = !TextBox004EditMode;
            GuiStatusBar(layoutRecs[5], "Fidelity...");
            if (GuiButton(layoutRecs[6], "RUN")) RUN(); 
            //----------------------------------------------------------------------------------

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//------------------------------------------------------------------------------------
// Controls Functions Definitions (local)
//------------------------------------------------------------------------------------
// Button: RUN logic
static void RUN()
{
    // TODO: Implement control logic
}

