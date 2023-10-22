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
#define QUIT() { CloseWindow(); return 0; };
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

//------------------------------------------------------------------------------------
// Button Functions
//------------------------------------------------------------------------------------
void plotSpectrum(graph& plot, RVec x, Hamiltonian& H0, int num)
{
    plot.clearLines();

    H0.calcSpectrum(num);
    
    plot.plot(x,H0.V,"Potential",BLACK,1,4,0);

    double eig_scale = ((H0.eigenvalue(1)-H0.eigenvalue(0))/2)/(H0[0].maxCoeff());

    for (auto i = 0; i < H0.spectrum.numEigvs; i++)
    {
        if (H0.eigenvalue(i) > 0) {break;} // only plot bound
        plot.plot(x, H0[i] * eig_scale + H0.eigenvalue(i), "Eigenvector "+std::to_string(i));
    }
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    // RAYGUI Initialization
    //---------------------------------------------------------------------------------------
    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_TOPMOST);
    InitWindow(1280, 720, "Control Panel");
    SetStyles();
    SetExitKey(KEY_NULL);
    SetTargetFPS(30);
    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
    //--------------------------------------------------------------------------------------
    // MY Initialization
    //---------------------------------------------------------------------------------------
    PerfStats perfStats(0, 0);

    graph mainplot(Rectangle{50, 50, 400, 600});
    graph spaceplot(Rectangle{550, 50, 600, 295});
    graph momplot(Rectangle{550, 355, 600, 295});

    ADD_AUTO_REDRAW(&mainplot,&spaceplot,&momplot,&perfStats);

    const int dim = 1 << 11;
    const auto k = sqrt(2);
    const auto xlim = PI / k / 2 * 4;
    const double dt = 0.001;

    auto hs = HilbertSpace(dim, xlim);
    const RVec x = hs.x();
    const auto depth = 400;

    // DO NOT USE AUTO HERE OR THE VECTOR DATA IS FREED INSIDE THE SCOPE!
    std::function<RVec(double)> V0 = [&, x](double phase){return -0.5 * depth * (cos(2 * k * (x - phase)) + 1) * box(x - phase, -PI / k / 2, PI / k / 2);};

    HamiltonianFn H(hs,V0);
    Hamiltonian H0 = H(0);

    CVec psi_0 = H0[7];
    SplitStepper stepper = SplitStepper(dt,H,psi_0);

    int numsteps = 1;
    // GUI Loop
    //---------------------------------------------------------------------------------------
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
        CHECK_REDRAW();

        if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_C)) QUIT();
        if (IsKeyPressed(KEY_Z)) { perfStats.toggle(); }

        // mainplot buttons Rectangle {50, 50, 400, 600}
        if (GuiButton(Rectangle{90, 660, 100, 20}, "Clear plot")) mainplot.clearLines();
        if (GuiButton(Rectangle{200, 660, 100, 20}, "Set lims.")) mainplot.setLims();
        if (GuiButton(Rectangle{310, 660, 100, 20}, "Plot Spectrum")) plotSpectrum(mainplot,x,H0,10);

        // State plots
        if (GuiButton(Rectangle{1160, 505, 100, 20}, "Clear plots")) {momplot.clearLines();spaceplot.clearLines();};
        if (GuiButton(Rectangle{1160, 535, 100, 20}, "Set lims.")) {momplot.setLims();spaceplot.setLims();};

        if (GuiButton(Rectangle{1160, 350, 100, 20}, "Show state")) {
            spaceplot.clearLines();
            spaceplot.plot(x,stepper.state().real(),"real",RED);
            spaceplot.plot(x,stepper.state().imag(),"imag",BLUE);
            spaceplot.plot(x,stepper.state().cwiseAbs2(),"abs2",BLACK);

            momplot.plot(H.p,stepper.state_p().cwiseAbs2(),"abs2",BLACK);
            momplot.clearLines();
            };
        if (GuiButton(Rectangle{1160, 380, 100, 20}, "Step (1)")) {stepper.step(0);};
        if (GuiSpinner(Rectangle{1160, 410, 100, 20},"",&numsteps,1,10000,CheckCollisionPointRec(GetMousePosition(),{1160, 410, 100, 20}))){};
        if (GuiButton(Rectangle{1160, 440, 100, 20}, ("Evolve ("+std::to_string(numsteps)+")").c_str())) {stepper.evolve(RVec::Zero(numsteps));};
        if (GuiButton(Rectangle{1160, 470, 100, 20}, "Reset")) {stepper.reset();};

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
