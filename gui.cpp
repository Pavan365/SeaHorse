#include "include/seahorse.h"

#include "raylib/raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raylib/raygui.h"
#undef RAYGUI_IMPLEMENTATION

#define TEXT_SPACING_VALUE 0
#include "Renderers.h"

Font fontTtf;
//----------------------------------------------------------------------------------
// Useful Functions Implementation
//----------------------------------------------------------------------------------
#define QUIT()         \
    {                  \
        CloseWindow(); \
        return 0;      \
    };
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

// Subscribing to this allows items to be forced to be redrawn if needed
Renderer renderer{};

//------------------------------------------------------------------------------------
// Button Functions
//------------------------------------------------------------------------------------
void CalcSpectrum(graph *plot, int num = 10)
{
    const int dim = 1 << 10;

    const auto k = sqrt(2);
    const auto xlim = PI / k / 2 * 3;

    auto hs = HilbertSpace(dim, xlim);

    const auto depth = 400;

    auto V0 = [&, x = hs.x()](double phase){return -depth * 0.5 * (cos(2 * k * (x - phase)) + 1) * box(x + x - phase, -PI / k / 2, PI / k / 2);};

    HamiltonianFn H(hs,V0);

    Hamiltonian H0 = H(0);

    RVec test = H0[0] + 3*H0[2];
    RVec t2 = test + test;
    RVec test2 = 3 * H0[0] + H0[2];

    H0.calcSpectrum(num);

    const RVec x = hs.x();
    
    plot->plot(x,V0(0),"Potential");

    for (auto i = 0; i < H0.spectrum.numEigvs; i++)
    {
        plot->plot(x, H0[i] * 100 + H0.eigenvalue(i), "Eigenvector "+std::to_string(i));
    }
}
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

    SetTraceLogLevel(LOG_WARNING);

    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_TOPMOST);
    InitWindow(screenWidth, screenHeight, "Control Panel");

    SetStyles();
    SetExitKey(KEY_NULL);
    SetTargetFPS(30);

    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
    //--------------------------------------------------------------------------------------

    // Monitors RAM/CPU/etc.
    PerfStats perfStats(0, 0);
    renderer.subscribe(&perfStats);

    graph mainplot(Rectangle{150, 100, 600, 300}, "position (au)", "Energy");
    renderer.subscribe(&mainplot);
    // mainplot.plot(true);

    // graph heatmapplot(Rectangle{150, 400, 600, 300},"position (nm)","Amplitude");
    // renderer.subscribe(&heatmapplot);
    // heatmapplot.setPadding(0,0);
    // heatmapplot.heatmap();

    // Main loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        perfStats.update();
        if (IsFileDropped())
        {
            auto files = LoadDroppedFiles();
            for (auto i = 0; i < files.count; i++)
            {
                printf("%s\n", files.paths[i]);

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
        //----------------------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        // Check if we've invalidated the screen buffer and need to redraw
        if (GetScreenHeight() != screenHeight || GetScreenWidth() != screenWidth || GetCurrentMonitor() != monitor)
        {
            renderer.draw();
            screenHeight = GetScreenHeight();
            screenWidth = GetScreenWidth();
            monitor = GetCurrentMonitor();
        }

        if (!IsWindowFocused())
            GuiDisable();
        else
            GuiEnable();

        if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_C))
            QUIT();
        if (IsKeyPressed(KEY_Z))
        {
            perfStats.toggle();
        }

        if (GuiButton(Rectangle{50, 250, 100, 20}, "Plot Spectrum")) CalcSpectrum(&mainplot);
        if (GuiButton(Rectangle{50, 220, 100, 20}, "SetLims()")) mainplot.setLims();

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
