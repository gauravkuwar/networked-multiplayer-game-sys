#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>
#include <map>
#include <iostream>
#include <cmath>
#include <vector>
#include "client.h"

using namespace std;

// g++ -std=c++11 -o myapp main.cpp client.cpp -lfltk -I /opt/homebrew/Cellar/flatbuffers/23.5.26/include -I /opt/homebrew/Cellar/fltk/1.3.8_1/include/ -lfltk -lfltk_images -lfltk_forms -L /opt/homebrew/Cellar/fltk/1.3.8_1/lib

const int padding = 15;
const float waitTime = 0.03; // 30 ms;
const int incr = 5;
const int teamSize = 1;
const int objs = (2*teamSize) + 1;

class SoccerFieldWindow : public Fl_Window {
private:
    int radius;
    int dx = 0;
    int dy = 0;
    map<int, bool> keysState;
    vector<vector<int>> pos; // positions
    vector<vector<int>> mv; // moves
    Client myClient;
    int pid;
    int port;

public:
    SoccerFieldWindow(int width, int height, const char* title, int pid, int port)
        : Fl_Window(width, height, title), mv(objs, vector<int>(2, 0)), radius(height / 30), pid(pid), myClient(port)
    {
        pos.push_back({width / 2, height / 2}); // ball
        pos.push_back({pos[0][0] - 200, pos[0][1]}); // p1
        pos.push_back({pos[0][0] + 200, pos[0][1]}); // p2
        mv[0].assign({0, 0});

        Fl::add_timeout(waitTime, timerCallback, this); // Set up the timer
    }

    void draw() override {
        drawSoccerField();
        drawPlayer(pos[0], FL_BLUE); // ball
        // teams
        for (int i=1; i < objs; i++) {
            if (i <= teamSize)
                drawPlayer(pos[i], FL_RED); // t1
            else
                drawPlayer(pos[i], FL_BLACK); // t2
        }
    }

    void drawSoccerField() {
        int goalWidth = 150;
        int goalHeight = 80;

        // main field 
        fl_color(FL_GREEN);
        fl_rectf(0, 0, w(), h());

        // border
        fl_color(FL_WHITE);
        fl_rect(padding, padding, w() - (padding * 2) , h() - (padding * 2));

        // goal left
        fl_color(FL_WHITE);
        fl_rect(padding, (h() / 2) - (goalWidth / 2), goalHeight , goalWidth);
        // goal right
        fl_color(FL_WHITE);
        fl_rect(w() - padding - goalHeight, (h() / 2) - (goalWidth / 2), goalHeight , goalWidth);
        
        // middle line
        fl_color(FL_WHITE);
        fl_line(w() / 2, 0, w() / 2, h());

        // middle circle
        fl_color(FL_WHITE);
        fl_circle(w() / 2, h() / 2, w() / 8);
    }

    void drawPlayer(vector<int> &p, int color) {
        fl_color(color);
        fl_pie(p[0] - radius, p[1] - radius, radius * 2, radius * 2, 0, 360);
    }

    int handle(int event) override {
        // keep track of keys pressed during waitTime
        int key = Fl::event_key();
        switch (event) {
            case FL_KEYDOWN: {
                keysState[key] = true;
                updateMove();
                return 1;
            }
            case FL_KEYUP: {
                keysState[key] = false;
                updateMove();
                return 1;
            }
            default:
                return Fl_Window::handle(event);
        }
    }

    static void timerCallback(void* userdata) {
        SoccerFieldWindow* window = static_cast<SoccerFieldWindow*>(userdata);
        window->updateAllMoves();
        window->updateAllPositions();
        window->redraw();
        Fl::repeat_timeout(waitTime-0.01, timerCallback, userdata); // Re-schedule the timer - 0.01 ms
    }

    void updateAllMoves() {
        for (int i=0; i < objs; i++)
            mv[i].assign({0, 0});

        if (dx != 0 || dy != 0)
            myClient.send(pid, dx, dy);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        bool wasEmpty = myClient.isEmptyRecv();

        while (!myClient.isEmptyRecv()) {
            std::array<int, 3> data = myClient.popRecv();
            mv[data[0]].assign({data[1], data[2]});
        }
    }

    void updateMove() {
        dx = 0;
        dy = 0;

        if (keysState[FL_Left]) {
            dx -= incr;
        }
        if (keysState[FL_Right]) {
            dx += incr;
        }
        if (keysState[FL_Up]) {
            dy -= incr;
        }
        if (keysState[FL_Down]) {
            dy += incr;
        }
    }

    void updateAllPositions() {
        vector<vector<int>> mvCp = mv;
        // cout << "still running update pos" << endl;

        // cout << "updating ball" << endl;
        for (int i=0; i < objs; i++) {
            // cout << "updating t1" << endl;
            int nx = pos[i][0] + mvCp[i][0]; // new x
            int ny = pos[i][1] + mvCp[i][1]; // new y
            
            // check against every object for overlapping
            for (int j=0; j < objs; j++) {
                if (i != j) {
                    int x2 = pos[j][0];
                    int y2 = pos[j][1];

                    float dis = sqrt(pow((nx - x2), 2) + pow((ny - y2), 2));
                    int signx = ((x2 - nx) / abs(x2 - nx));
                    int signy = ((y2 - ny) / abs(y2 - ny));

                    // modify until we are not overlapping
                    while (dis < (2 * radius)) {
                        nx -= signx;
                        ny -= signy;
                        dis = sqrt(pow((nx - x2), 2) + pow((ny - y2), 2));
                    }
                }

                // Keep the object within the soccer field
                int rp = radius + padding;
                if (nx < rp) nx = rp;
                if (nx > w() - rp) nx = w() - rp;
                if (ny < rp) ny = rp;
                if (ny > h() - rp) ny = h() - rp;
                                
                pos[i].assign({nx, ny});
            }
        }
            
    }
};

int main(int argc, char* argv[]) {
    // ./myapp 1 12346 # player 1
    // ./myapp 2 12347 # player 2

    SoccerFieldWindow window(900, 500, "Soccer", std::stoi(argv[1]), std::stoi(argv[2]));
    window.show();
    return Fl::run();
}
