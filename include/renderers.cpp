#include "raylib/raylib.h"
#include "raylib/raygui.h"
#include <eigen/Eigen/Core>

#include <vector>
#include <thread>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <sstream>

#define VEC_FROM_EIGEN(v) std::vector<double>(v.data(),v.data()+v.size())
#define VEC_VEC_FROM_EIGEN(m) ([](Eigen::MatrixXd z){std::vector<std::vector<double>> m2; for(int i = 0; i<z.cols(); i++){m2.push_back(std::vector<double>(z.data()+i*z.rows(),z.data()+z.rows()*(i+1)));}; return m2; }(m))

struct Renderer
{
    // Lets renderers subscribe themselves to this manager to re-draw them when needed
    void subscribe(Renderer* r){subscribers.push_back(r);r->manager=this;}
    virtual void draw() {
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
        for(auto iter = subscribers.begin(); iter != subscribers.end(); ++iter) {
            (*iter)->draw();
        }
    }
    Renderer* manager; // who controls our redrawing
    private:
    std::vector<Renderer*> subscribers; // whos redrawing we control
};

struct PerfStats : Renderer
{
    int fontSize = 12;
    Font fontTtf = LoadFontEx("UbuntuMonoBold.ttf",fontSize,0,0);

    bool off = false;

    int totalthreads, nthreads, rss, fps;
    double pcpu, rssGB;
    std::chrono::system_clock::time_point time;
    char statscommand[1024];

    Rectangle bounds{0,0,0,0};
    // Constructor
    PerfStats(int posX, int posY) : bounds(Rectangle{(float)posX,(float)posY,0,0})
    {
        PerfStats::time = std::chrono::system_clock::now();
        PerfStats::totalthreads = std::thread::hardware_concurrency();
        #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        throw std::runtime_error("Windows performance statistics not supported yet");
        #elif __APPLE__
        snprintf(PerfStats::statscommand, 1024, "echo $(ps %i -o 'pcpu=,rss=') $(ps %i -M | wc -l)", getpid(), getpid());
        #else // Linux hopefully
        snprintf(PerfStats::statscommand, 1024, "echo $(ps -p %i -o 'pcpu=,rss=') $(ps -p %i -T | wc -l)", getpid(), getpid());
        #endif
        update(true);
    };

    // Execute command through pipe
    std::string exec(const char *cmd)
    {
        std::array<char, 256> buffer; // holds data - we dont expect that much as we only pull rss and pcpu
        std::string result;
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
        if (!pipe) throw std::runtime_error("popen() failed!");

        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) result += buffer.data();

        return result;
    }

    // Get stats again if we have waited long enough
    int update(bool force = false)
    {
        auto newtime = std::chrono::system_clock::now();

        if ((newtime - time).count() > 0.17e6 || force)
        { // only update every 0.17s
            sscanf(exec(PerfStats::statscommand).c_str(), "%lf %i %i", &(PerfStats::pcpu), &(PerfStats::rss), &(PerfStats::nthreads));
            PerfStats::fps = GetFPS();

            PerfStats::nthreads -= 1;
            PerfStats::rssGB = PerfStats::rss * 1024e-9; // convert 1024 byte units to gigabytes
            PerfStats::time = newtime;

            draw();
            return 1;
        }
        return 0;
    }

    void draw()
    {
        if (off==true) return;
        Color color;

        // FIND TEXT SIZES
        const char resUnits[1024] = "FPS: \nTHR: \nCPU: \nRAM: ";
        const Vector2 unitsSize = MeasureTextEx(fontTtf, resUnits, fontSize, TEXT_SPACING_VALUE);

        auto resNums = TextFormat("%2i\n%i/%i\n%.1f %%\n%.2f GB", PerfStats::fps, PerfStats::nthreads, PerfStats::totalthreads, PerfStats::pcpu, PerfStats::rssGB);
        const Vector2 numsSize = MeasureTextEx(fontTtf, resNums, fontSize, TEXT_SPACING_VALUE);

        // Remove old stats
        DrawRectangleRec(bounds,WHITE);

        bounds.width = numsSize.x + unitsSize.x + fontSize;
        bounds.height = fontSize + std::max(numsSize.y, unitsSize.y);
        BeginDrawing();
        BeginScissorMode(bounds.x,bounds.y,bounds.width,bounds.height);

        DrawRectangleLinesEx(bounds,1,BLACK);
        
        color = PerfStats::fps < 15 ? RED : PerfStats::fps < 30 ? ORANGE : LIME;
        DrawTextEx(fontTtf, "FPS: ", Vector2{bounds.x+fontSize/2, bounds.y+fontSize/2}, fontSize, TEXT_SPACING_VALUE, BLACK);
        DrawTextEx(fontTtf, TextFormat("%2i", PerfStats::fps), Vector2{bounds.x+fontSize/2 + unitsSize.x, bounds.y+fontSize/2}, fontSize, TEXT_SPACING_VALUE, color);

        color = PerfStats::nthreads > PerfStats::totalthreads ? RED : 2 * PerfStats::nthreads > PerfStats::totalthreads ? ORANGE : LIME;
        DrawTextEx(fontTtf, "THR: ", Vector2{bounds.x+fontSize/2, bounds.y+fontSize/2 + numsSize.y / 4}, fontSize, TEXT_SPACING_VALUE, BLACK);
        DrawTextEx(fontTtf, TextFormat("%i/%i", PerfStats::nthreads, PerfStats::totalthreads), Vector2{bounds.x+fontSize/2 + unitsSize.x, bounds.y+fontSize/2 + numsSize.y / 4}, fontSize, TEXT_SPACING_VALUE, color);

        color = PerfStats::pcpu > 90 ? RED : PerfStats::pcpu > 50 ? ORANGE : LIME;
        DrawTextEx(fontTtf, "CPU: ", Vector2{bounds.x+fontSize/2, bounds.y+fontSize/2 + 2 * numsSize.y / 4}, fontSize, TEXT_SPACING_VALUE, BLACK);
        DrawTextEx(fontTtf, TextFormat("%.1f %%", PerfStats::pcpu), Vector2{bounds.x+fontSize/2 + unitsSize.x, bounds.y+fontSize/2 + 2 * numsSize.y / 4}, fontSize, TEXT_SPACING_VALUE, color);

        color = PerfStats::rssGB > 10 ? RED : PerfStats::rssGB > 5 ? ORANGE : LIME;
        DrawTextEx(fontTtf, "RAM: ", Vector2{bounds.x+fontSize/2, bounds.y+fontSize/2 + 3 * numsSize.y / 4}, fontSize, TEXT_SPACING_VALUE, BLACK);
        DrawTextEx(fontTtf, TextFormat("%.2f GB", PerfStats::rssGB), Vector2{bounds.x+fontSize/2 + unitsSize.x, bounds.y+fontSize/2 + 3 * numsSize.y / 4}, fontSize, TEXT_SPACING_VALUE, color);
        
        EndScissorMode();
        EndDrawing();
    }
    void toggle(){
        off = !off;
        manager->draw();
    }
};

struct graphData
{
    // General graph data, specialises to line/heatmap etc

    // Constructor
    graphData(Color c, std::string n) : color(c), name(n) {}
    // Destructor
    virtual ~graphData() = default;

    public:
    Color color;
    std::string name;
    float lims[4] = {0,1,0,1}; // {lims[0],lims[1],lims[2],lims[3]}

    inline void updateName(std::string n){name = n;}
    inline void updateColor(Color c){color = c;}

    virtual void draw(Rectangle inside_bounds, double plot_lims[4]) = 0;
};

struct graphLine : graphData
{
    std::vector<double> xData;
    std::vector<double> yData;

    bool drawPoints = false;
    bool drawLine = true;

    // Constructor
    graphLine(std::vector<double> x, std::vector<double> y, std::string n, Color c) : graphData(c,n), xData(x), yData(y)
    {
        if(x.size() != y.size()){std::cerr<<"Non-matching sizes in graph data for "<<name<<std::endl;}
        setLims();
    }

    public:
    void draw(Rectangle inside_bounds, double plot_lims[4])
    {
        auto xFactor = (inside_bounds.width)/(plot_lims[1] - plot_lims[0]);
        auto yFactor = (inside_bounds.height)/(plot_lims[3] - plot_lims[2]);
        auto xToPos = [&](double x){return (float)((x-plot_lims[0])*xFactor+inside_bounds.x);};
        auto yToPos = [&](double y){return (float)(-(y-plot_lims[2])*yFactor+inside_bounds.y+inside_bounds.height);};

        Vector2 start, end;
        for (auto i = 0; i < xData.size()-1; i++) {
            start = {xToPos(xData[i]), yToPos(yData[i])};
            end = {xToPos(xData[i+1]), yToPos(yData[i+1])};
            if (drawLine) DrawLineEx(start, end, 2, color);

            if (drawPoints) DrawCircleV(start,2,ColorAlpha(color,0.5));
        }
    }

    inline size_t len(){return xData.size();}
    void updateData(std::vector<double> x,std::vector<double> y){
        if(x.size() != y.size()){std::cerr<<"Non-matching sizes in graph data for "<<name<<std::endl;}
        // Updates the underlying data and subsequently the limits
        yData = x;
        xData = y;
        setLims();
    }
    void appendPoint(double x, double y){
        xData.push_back(x);
        yData.push_back(y);
        // Update Lims without looping over all data
        if (x<lims[0]) lims[0]=x;
        if (x>lims[1]) lims[1]=x;
        if (y<lims[2]) lims[2]=y;
        if (y>lims[3]) lims[3]=y;
    }
    void setLims()
    {
        const auto [x0,x1] = std::minmax_element(begin(xData), end(xData));
        const auto [y0,y1] = std::minmax_element(begin(yData), end(yData));
        lims[0] = (*x0);
        lims[1] = (*x1);
        lims[2] = (*y0);
        lims[3] = (*y1);
    }
};

struct graphHeatmap : graphData
{
    // Used for limits
    std::vector<double> xData;
    std::vector<double> yData;

    std::vector<std::vector<double>> zData;
    Color color2;

    // Constructor
    graphHeatmap(std::vector<double> x, std::vector<double> y, std::vector<std::vector<double>> z, std::string n, Color c, Color c2) : graphData(c,n),xData(x), yData(y), zData(z), color2(c2)
    {
        setLims();
    }

    public:
    void draw(Rectangle inside_bounds, double plot_lims[4])
    {
        auto xWidth = inside_bounds.width/zData.size();
        auto yWidth = inside_bounds.height/zData[0].size();
        auto xOrigin = (lims[0]-plot_lims[0])*(inside_bounds.width)/(plot_lims[1] - plot_lims[0])+inside_bounds.x;
        auto yOrigin = -(lims[3]-plot_lims[2])*(inside_bounds.height)/(plot_lims[3] - plot_lims[2])+inside_bounds.y+inside_bounds.height;
        // const auto [zMin,zMax] = std::minmax_element(begin(zData), end(zData));
        
        SetRandomSeed(random());
        for (auto x=0;x<zData.size();x++)
        {
            for (auto y=0;y<zData[x].size();y++)
            {
                Color c = ColorFromHSV(zData[x][y]*2.55,0.5,0.5);
                DrawRectangle(floor(xOrigin+xWidth*x),floor(yOrigin+yWidth*y),ceil(xWidth),ceil(yWidth),c);
            }
        }
    }

    inline size_t len(){return xData.size();}
    void updateData(std::vector<double> x, std::vector<double> y, std::vector<std::vector<double>> z){
        // Updates the underlying data and subsequently the limits
        xData = x;
        yData = y;
        zData = z;
        setLims();
    }
    void updateData(std::vector<std::vector<double>> z){
        // Updates the underlying data and subsequently the limits
        zData = z;
    }
    void setLims()
    {
        const auto [x0,x1] = std::minmax_element(begin(xData), end(xData));
        const auto [y0,y1] = std::minmax_element(begin(yData), end(yData));
        lims[0] = (*x0);
        lims[1] = (*x1);
        lims[2] = (*y0);
        lims[3] = (*y1);
    }
};

struct graph : Renderer
{
    std::vector<std::unique_ptr<graphData>> lines;

    std::vector<double> xTicks;
    std::vector<std::string> xTickLabels;
    std::vector<double> yTicks;
    std::vector<std::string> yTickLabels;
    double lims[4] = {0,1,0,1};

    std::string xLabel;
    std::string yLabel;
    int fontSize = 20;
    Font fontTtf = LoadFontEx("UbuntuMonoBold.ttf",fontSize,0,0);

    Vector2 padding = {0,8};
    bool legend = false;

    Rectangle bounds = Rectangle{0,0,-100,-100};

    //Constructors
    graph(Rectangle bounds, std::string xLabel,std::string yLabel) : lines(std::vector<std::unique_ptr<graphData>>()),xLabel(xLabel),yLabel(yLabel),bounds(bounds)
    {
        setLims();
        draw();
    }
    graph(Rectangle bounds): graph(bounds,"",""){}

    // Internal functions
    private:
    void ReplaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace)
    {
        size_t pos = 0;
        while((pos = subject.find(search, pos)) != std::string::npos) {
            subject.replace(pos, search.length(), replace);
            pos += replace.length();
        }
    }
    double NiceNumber (const double Value, const int Round)
    {
        int    Exponent;
        double Fraction;
        double NiceFraction;

        Exponent = (int) floor(log10(Value));
        Fraction = Value/pow(10, (double)Exponent);

        if (Round) {
            if (Fraction < 1.75)  NiceFraction = 1.0;
            else if (Fraction < 3.0) NiceFraction = 2.0;
            else if (Fraction < 5.0) NiceFraction = 3.0;
            else if (Fraction < 7.0) NiceFraction = 5.0;
            else NiceFraction = 10.0;
        }
        else {if (Fraction <= 1.0) NiceFraction = 1.0;
            else if (Fraction <= 2.0) NiceFraction = 2.0;
            else if (Fraction <= 5.0) NiceFraction = 5.0;
            else NiceFraction = 10.0;
        }

        return NiceFraction*pow(10, (double)Exponent);
    }
    std::string FormatNumber(double f, double step)
    {
        std::stringstream ss;
        if (step > 1000) { ss << std::scientific << std::setprecision(0) << f << std::fixed;}
        else if (step>1) {ss << std::round(f);}
        else {ss << std::setprecision(2) << f;}
        auto output = ss.str();

        if(output=="-0") output = "0";
        ReplaceStringInPlace(output,"e+0",".e");
        ReplaceStringInPlace(output,"e-0",".e-");
        return output;
    }

    // Plotting functions
    public:
    void plot(std::vector<double> x, std::vector<double> y, std::string n, Color c)
    {
        lines.push_back(std::make_unique<graphLine>(x,y,n,c));
        setLims();
        draw();
    }
    void plot(std::vector<double> x, std::vector<double> y) {plot(x,y, "", BLACK);}
    void plot(Eigen::VectorXd x, Eigen::VectorXd y, std::string n, Color c) {plot(VEC_FROM_EIGEN(x),VEC_FROM_EIGEN(y),n,c);}
    void plot(Eigen::VectorXd x, Eigen::VectorXd y) {plot(VEC_FROM_EIGEN(x), VEC_FROM_EIGEN(y));}
    void plot(bool swap)// Testing
    {
        std::vector<double> x;
        std::vector<double> y;
        std::string n = swap==true ? "Test - sin" : "Test - cos";
        Color c = swap==true ? RED : BLUE;

        for(double i=-100;i<=50;i+=.1)
        {
            x.push_back(i);
            y.push_back(swap==true ? 7+sin(i) : cos(i)-0.1*i);
        }
        plot(x,y,n,c);  
    }

    void heatmap(std::vector<double> x, std::vector<double> y, std::vector<std::vector<double>> z, std::string n, Color c, Color c2){
        lines.push_back(std::make_unique<graphHeatmap>(x,y,z,n,c,c2));
        setLims();
        draw();
    }
    void heatmap(std::vector<double> x, std::vector<double> y, std::vector<std::vector<double>> z) {heatmap(x, y, z, "", BLUE, PURPLE);}
    void heatmap(std::vector<std::vector<double>> z) {heatmap(std::vector<double>(0,1), std::vector<double>(0,1), z);}
    void heatmap(Eigen::VectorXd x, Eigen::VectorXd y, Eigen::MatrixXd z, std::string n, Color c, Color c2) {heatmap(VEC_FROM_EIGEN(x),VEC_FROM_EIGEN(y),VEC_VEC_FROM_EIGEN(z),n,c,c2);}
    void heatmap(Eigen::VectorXd x, Eigen::VectorXd y, Eigen::MatrixXd z) {heatmap(VEC_FROM_EIGEN(x),VEC_FROM_EIGEN(y),VEC_VEC_FROM_EIGEN(z));}
    void heatmap(Eigen::MatrixXd z) {heatmap(VEC_VEC_FROM_EIGEN(z));}
    void heatmap()// Testing
    {
        std::vector<double> x = {0,1};
        std::vector<double> y = {0,5};
        std::vector<std::vector<double>> z = {};

        z.push_back(std::vector<double>{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20});
        z.push_back(std::vector<double>{-2,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18});
        z.push_back(std::vector<double>{-4,-3,-2,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16});
        heatmap(x,y,z);
    }

    // Util. functions
    void setLims(double xmin, double xmax, double ymin, double ymax)
    {
        if (xmax<xmin) {auto temp = xmax;xmax=xmin;xmin=temp;}
        if (ymax<ymin) {auto temp = lims[3];lims[3]=lims[2];lims[2]=temp;}
        if (xmin==xmax) {xmax+=1e-10;}
        if (ymin==ymax) {ymax+=1e-10;}
        // set lims
        lims[0] = xmin;
        lims[1] = xmax;
        lims[2] = ymin;
        lims[3] = ymax;
        // find nice ticks
        auto xRange = NiceNumber(lims[1] - lims[0], 0);
        auto xTick = NiceNumber(xRange/(8 - 1), 1);
        auto yRange = NiceNumber(lims[3] - lims[2], 0);
        auto yTick = NiceNumber(yRange/(6 - 1), 1);
        // set ticks
        xTicks.clear();
        xTickLabels.clear();
        for (auto i = ceil(lims[0]/xTick)*xTick; i<=lims[1]; i+=xTick){
            xTicks.push_back(i);
            xTickLabels.push_back(FormatNumber(i,xTick));
        }
        yTicks.clear();
        yTickLabels.clear();
        for (auto i = ceil(lims[2]/yTick)*yTick; i<=lims[3]; i+=yTick){
            yTicks.push_back(i);
            yTickLabels.push_back(FormatNumber(i,yTick));
        }
        draw();
    }
    void setLims(){
        // Automatically assigns limits to include all data in graph
        bool first = true;
        for(auto const &line: lines)
        {
            if (first || line->lims[0] < lims[0]) lims[0] = line->lims[0];
            if (first || line->lims[1] > lims[1]) lims[1] = line->lims[1];
            if (first || line->lims[2] < lims[2]) lims[2] = line->lims[2];
            if (first || line->lims[3] > lims[3]) lims[3] = line->lims[3];
            first=false;
        }
        if (lines.empty()){lims[0]=0;lims[1]=1;lims[2]=0;lims[3]=1;}
        setLims(lims[0],lims[1],lims[2],lims[3]);
    }

    void draw()
    {
        BeginDrawing();
        // Clear our area...
        DrawRectangleRec(bounds,GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        // Debugging outline
        // DrawRectangleLinesEx(bounds, 1, RED);
        
        // Account for axis titles/ticks in bounds
        auto xLabelSize = MeasureTextEx(fontTtf,xLabel.c_str(),fontSize,0);
        auto yLabelSize = MeasureTextEx(fontTtf,yLabel.c_str(),fontSize,0);
        auto data_bounds = Rectangle{bounds.x + yLabelSize.y * 2, bounds.y, bounds.width - yLabelSize.y * 2, bounds.height - xLabelSize.y * 2};
        DrawRectangleRec(data_bounds,WHITE);
        
        // Draw the plot data
        auto inside_bounds = Rectangle{data_bounds.x + padding.x,data_bounds.y + padding.y,data_bounds.width -padding.x*2,data_bounds.height -padding.y*2};
        auto xFactor = (inside_bounds.width)/(lims[1] - lims[0]);
        auto yFactor = (inside_bounds.height)/(lims[3] - lims[2]);
        auto xToPos = [&](double x){return (float)((x-lims[0])*xFactor+inside_bounds.x);};
        auto yToPos = [&](double y){return (float)(-(y-lims[2])*yFactor+inside_bounds.y+inside_bounds.height);};

        // Draw lines
        BeginScissorMode(data_bounds.x,data_bounds.y,data_bounds.width,data_bounds.height);
        for(auto const &line: lines) line->draw(inside_bounds,lims);
        EndScissorMode();

        // Draw axis labels
        BeginScissorMode(bounds.x,bounds.y,bounds.width,bounds.height);
        DrawTextEx(fontTtf,xLabel.c_str(),Vector2{data_bounds.x + data_bounds.width/2 - xLabelSize.x/2,data_bounds.y + data_bounds.height + xLabelSize.y},fontSize,0,BLACK);
        DrawTextPro(fontTtf,yLabel.c_str(),Vector2{data_bounds.x - yLabelSize.y * 2,data_bounds.y + data_bounds.height/2 + yLabelSize.x/2},Vector2{0,0},-90,fontSize,0,BLACK);

        // Draw ticks + numbers
        for (auto i=0;i<xTicks.size();i++){
            auto xtick = xTicks.at(i);
            auto xticklabel = xTickLabels.at(i).c_str();
            DrawLineEx(Vector2{xToPos(xtick),data_bounds.y+data_bounds.height},Vector2{xToPos(xtick),data_bounds.y+data_bounds.height-fontSize/3},2,DARKGRAY);

            auto textSize = MeasureTextEx(fontTtf,xticklabel,fontSize*0.8,0);
            // If we arent fully in the plot area dont show the tick label
            auto pos = Vector2{xToPos(xtick)-textSize.x/2,data_bounds.y+data_bounds.height};
            if (pos.x < bounds.x || pos.x+textSize.x > bounds.x+bounds.width){continue;}
            DrawTextEx(fontTtf,xticklabel,pos,fontSize*0.8,0,DARKGRAY);
        }
        for (auto i=0;i<yTicks.size();i++){
            auto ytick = yTicks.at(i);
            auto yticklabel = yTickLabels.at(i).c_str();
            DrawLineEx(Vector2{data_bounds.x,yToPos(ytick)},Vector2{data_bounds.x+fontSize/3,yToPos(ytick)},2,DARKGRAY);

            auto textSize = MeasureTextEx(fontTtf,yticklabel,fontSize*0.8,0);
            // If we arent fully in the plot area dont show the tick label
            auto pos = Vector2{data_bounds.x-textSize.y,yToPos(ytick)+textSize.x/2};
            if (pos.y < bounds.y || pos.y+textSize.y > data_bounds.y+data_bounds.height){continue;}
            DrawTextPro(fontTtf,yticklabel,pos,Vector2{0,0},-90,fontSize*0.8,0,DARKGRAY);
        }

        // Draw graph outline
        auto outline_bounds = Rectangle{data_bounds.x-1,data_bounds.y-1,data_bounds.width+2,data_bounds.height+2};
        DrawRectangleLinesEx(outline_bounds, 2, BLACK);
        EndScissorMode();
        EndDrawing();
    }
};