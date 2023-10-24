/*******************************************************************************************
*
*   SeaHorseGui v1.0.0 - Front end to demonstrate SeaHorse
*
*   MODULE USAGE:
*       #define GUI_SEAHORSEGUI_IMPLEMENTATION
*       #include "gui_SeaHorseGui.h"
*
*       INIT: GuiSeaHorseGuiState state = InitGuiSeaHorseGui();
*       DRAW: GuiSeaHorseGui(&state);
*
*   LICENSE: Propietary License
*
*   Copyright (c) 2022 Harry Kendell. All Rights Reserved.
*
*   Unauthorized copying of this file, via any medium is strictly prohibited
*   This project is proprietary and confidential unless the owner allows
*   usage in any other form by expresely written permission.
*
**********************************************************************************************/
#include "include/seahorse.h"

#include "raylib/raylib.h"
#undef RAYGUI_IMPLEMENTATION
#include "raylib/raygui.h"

#define TEXT_SPACING_VALUE 0
#include "GuiComponents.cpp"
#include "Globals.cpp"

#ifndef GUI_SEAHORSEGUI_H
#define GUI_SEAHORSEGUI_H

struct GuiSeaHorseGuiState {
    bool stepper_numstepsEditMode;
    int stepper_numstepsValue;
    bool stepper_initialstateEditMode;
    int stepper_initialstateActive;
    bool PerfStatsWindowActive;

    // Custom state variables (depend on development software)
    // NOTE: This variables should be added manually if required

};

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
GuiSeaHorseGuiState InitGuiSeaHorseGui(void);
void GuiSeaHorseGui(GuiSeaHorseGuiState *state);
static void MainplotClearplot();
static void MainplotSetlims();
static void MainplotPlotspectrum();
static void StepperStep();
static void StepperEvolve();
static void StepperReset();
static void StepperSetlims();
static void Perfstats();
static void LabelButton014();
static void LabelButton015();
static void LabelButton016();

#endif // GUI_SEAHORSEGUI_H

/***********************************************************************************
*
*   GUI_SEAHORSEGUI IMPLEMENTATION
*
************************************************************************************/
#if defined(GUI_SEAHORSEGUI_IMPLEMENTATION)

#include "raygui.h"

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Internal Module Functions Definition
//----------------------------------------------------------------------------------
void GenerateFontFile(std::string fontFile = "resources/UbuntuMonoBold.ttf")
{
    // Pull in file
    int dataSize = 0;
    unsigned char *fileData = LoadFileData(fontFile.c_str(), &dataSize);
    // Redirect stdout to the new file
    FILE *saved = stdout;
    stdout = fopen((fontFile + ".h").c_str(), "a");

    // Write data to file
    printf("unsigned char text_txt_data[] = {");
    for (auto i = 0; i < dataSize; i++)
    {
        if (fileData[i] == 0)
        {
            printf("0x00,");
        }
        else
        {
            printf("%#04x,", fileData[i]);
        }
    }
    printf("};");

    // Replace stdout
    fclose(stdout);
    stdout = saved;
}
void SetStyles()
{
    GuiSetStyle(DEFAULT, LINE_COLOR, 255); // Black lines

    // fontTtf = LoadFontEx("UbuntuMonoBold.ttf",20,0,0);

    fontTtf = LoadFontFromMemory(".ttf", UbuntuMonoBold, (sizeof(UbuntuMonoBold) / sizeof(*UbuntuMonoBold)), 20, 0, 0);

    GuiSetFont(fontTtf);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 15);
    GuiSetStyle(DEFAULT, TEXT_SPACING, TEXT_SPACING_VALUE);
}

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
GuiSeaHorseGuiState InitGuiSeaHorseGui(void)
{
    GuiSeaHorseGuiState state = { 0 };

    state.stepper_numstepsEditMode = false;
    state.stepper_numstepsValue = 0;
    state.stepper_initialstateEditMode = false;
    state.stepper_initialstateActive = 0;
    state.PerfStatsWindowActive = true;

    // Custom variables initialization

    return state;
}
static void MainplotClearplot()
{
    // TODO: Implement control logic
}
static void MainplotSetlims()
{
    // TODO: Implement control logic
}
static void MainplotPlotspectrum()
{
    // TODO: Implement control logic
}
static void StepperStep()
{
    // TODO: Implement control logic
}
static void StepperEvolve()
{
    // TODO: Implement control logic
}
static void StepperReset()
{
    // TODO: Implement control logic
}
static void StepperSetlims()
{
    // TODO: Implement control logic
}
static void Perfstats()
{
    // TODO: Implement control logic
}
static void LabelButton014()
{
    // TODO: Implement control logic
}
static void LabelButton015()
{
    // TODO: Implement control logic
}
static void LabelButton016()
{
    // TODO: Implement control logic
}


void GuiSeaHorseGui(GuiSeaHorseGuiState *state)
{
    if (state->stepper_initialstateEditMode) GuiLock();

    if (state->PerfStatsWindowActive)
    {
        state->PerfStatsWindowActive = !GuiWindowBox((Rectangle){ 288, 256, 120, 136 }, "Performance");
        if (GuiLabelButton((Rectangle){ 296, 288, 40, 24 }, "FPS:")) Perfstats();
        if (GuiLabelButton((Rectangle){ 296, 312, 40, 24 }, "THR:")) LabelButton014();
        if (GuiLabelButton((Rectangle){ 296, 336, 40, 24 }, "CPU:")) LabelButton015();
        if (GuiLabelButton((Rectangle){ 296, 360, 40, 24 }, "RAM:")) LabelButton016();
    }
    GuiPanel((Rectangle){ 560, 392, 264, 136 }, NULL);
    if (GuiSpinner((Rectangle){ 696, 432, 120, 24 }, NULL, &state->stepper_numstepsValue, 0, 100, state->stepper_numstepsEditMode)) state->stepper_numstepsEditMode = !state->stepper_numstepsEditMode;
    GuiPanel((Rectangle){ 592, 256, 336, 40 }, NULL);
    if (GuiButton((Rectangle){ 824, 264, 96, 24 }, "Clear Plot")) MainplotClearplot(); 
    if (GuiButton((Rectangle){ 712, 264, 96, 24 }, "Set Lims.")) MainplotSetlims(); 
    if (GuiButton((Rectangle){ 600, 264, 96, 24 }, "Plot Spectrum")) MainplotPlotspectrum(); 
    if (GuiButton((Rectangle){ 568, 400, 120, 24 }, "Step")) StepperStep(); 
    if (GuiButton((Rectangle){ 568, 432, 120, 24 }, "Evolve by...")) StepperEvolve(); 
    if (GuiButton((Rectangle){ 568, 464, 120, 24 }, "Reset State to...")) StepperReset(); 
    if (GuiButton((Rectangle){ 632, 496, 120, 24 }, "Set Lims.")) StepperSetlims(); 
    if (GuiDropdownBox((Rectangle){ 696, 464, 120, 24 }, "H[0],H[1],H[2]", &state->stepper_initialstateActive, state->stepper_initialstateEditMode)) state->stepper_initialstateEditMode = !state->stepper_initialstateEditMode;
    
    GuiUnlock();
}

#endif // GUI_SEAHORSEGUI_IMPLEMENTATION
