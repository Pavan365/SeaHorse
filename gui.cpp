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
    const int dim = 1 << 11;

    const auto k = sqrt(2);
    const auto xlim = PI / k / 2 * 3;

    auto hs = HilbertSpace(dim, xlim);
    const RVec x = hs.x();

    const auto depth = 400;

    auto V0 = [&, x](double phase){return -0.5 * depth * (cos(2 * k * (x - phase)) + 1) * box(x - phase, -PI / k / 2, PI / k / 2);};

    HamiltonianFn H(hs,V0);

    plot->plot(x,V0(0),"Potential");
    Hamiltonian H0 = H(0);

    H0.calcSpectrum(num);

    for (auto i = 0; i < H0.spectrum.numEigvs; i++)plot->plot(x, H0[i] * 100 + H0.eigenvalue(i), "Eigenvector "+std::to_string(i));

// TESTING
    RVec rvec = H0[0];
    CVec cvec = H0[0]*1i;

    // RVec -> CVec
    CVec t = rvec;

    // RVec + double -> RVec
    RVec t1 = rvec + 5.6;
    // RVec * double -> RVec
    RVec t2 = rvec * 5.6;
    // RVec + complex -> CVec
    RVec t3 = rvec * 5.6;
    // Rvec * complex -> CVec
    CVec t4 = rvec * 5.6i;

    // CVec + double -> CVec
    CVec t5 = cvec + 4.5;
    // CVec * double -> CVec
    CVec t6 = cvec * 4.5;
    // CVec + complex -> CVec
    CVec t7 = cvec + 4.5i;
    // CVec * complex -> CVec
    CVec t8 = cvec * 4.5i;

    // RExpr + double -> RVec
    RVec t9 = (rvec * 5.6) + 4.5;
    // RExpr * double -> RVec
    RVec t10 = (rvec * 5.6) * 4.5;
    // RExpr + complex -> CVec
    CVec t11 = (rvec * 5.6) + 4.5i;
    // RExpr * complex -> CVec
    CVec t12 = (rvec * 5.6) * 4.5i;

    // CExpr + double -> CVec
    CVec t13 = (cvec * 5.6) + 4.5;
    // CExpr * double -> CVec
    CVec t14 = (cvec * 5.6) * 4.5;
    // CExpr + complex -> CVec
    CVec t15 = (cvec * 5.6) + 4.5i;
    // CExpr * complex -> CVec
    CVec t16 = (cvec * 5.6) * 4.5i;

    // auto test = x*x*x;
    // auto test = rvec + cvec;

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
        if (GuiButton(Rectangle{50, 220, 100, 20}, "Set lims.")) mainplot.setLims();
        if (GuiButton(Rectangle{50, 190, 100, 20}, "Clear plot")) mainplot.clearLines();

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
