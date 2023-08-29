#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>
#include <map>
#include <iostream>
#include <cmath>

using namespace std;

const int padding = 15;
const float waitTime = 0.3; // 30 ms
const int teamSize = 1;

const int FL_A = 97;
const int FL_S = 115;
const int FL_D = 100;
const int FL_W = 119;

class SoccerFieldWindow : public Fl_Window {
private:
    int radius;
    std::map<int, bool> keysState;

    int pos[1 + (2*teamsize)];
    int mv[1 + (2*teamsize)];
    // int ballPos[2];
    // int t1Pos[teamSize][2];
    // int t2Pos[teamSize][2];

    // int ballMv[2];
    // int t1Mv[teamSize][2];
    // int t2Mv[teamSize][2];

    const int incr = 5;
    // this ensures we are not looking are the same object when checking for overlap
    const float lowerDis = incr * sqrt(2);

public:
    SoccerFieldWindow(int width, int height, const char* title)
        : Fl_Window(width, height, title)
    {
        ballPos[0] = width / 2;
        ballPos[1] = height / 2;

        t1Pos[0][0] = ballPos[0] - 200;
        t1Pos[0][1] = ballPos[1];

        t2Pos[0][0] = ballPos[0] + 200;
        t2Pos[0][1] = ballPos[1];

        ballMv[0] = 0;
        ballMv[1] = 0;

        for (int i=0; i < teamSize; i++) {
            t1Mv[i][0] = 0;
            t1Mv[i][1] = 0;

            t2Mv[i][0] = 0;
            t2Mv[i][1] = 0;
        }

        radius = height / 30;
        cout << "radius: " << radius << endl;

        Fl::add_timeout(waitTime, timerCallback, this); // Set up the timer
    }

    void draw() override {
        drawSoccerField();
        drawPlayer(ballPos[0], ballPos[1], FL_BLUE); // ball

        int i;
        // teams
        for (i=0; i < teamSize; i++) {
            drawPlayer(t1Pos[i][0], t1Pos[i][1], FL_RED);
            drawPlayer(t2Pos[i][0], t2Pos[i][1], FL_BLACK);
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

    void drawPlayer(int x, int y, int color) {
        fl_color(color);
        fl_pie(x - radius, y - radius, radius * 2, radius * 2, 0, 360);
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
        window->updateAllPositions();
        window->redraw();
        Fl::repeat_timeout(waitTime, timerCallback, userdata); // Re-schedule the timer
    }

    void updateMove() {
        int keyTypes[2][4] = {{FL_A, FL_D, FL_W, FL_S},
                              {FL_Left, FL_Right, FL_Up, FL_Down}};

        int* t[2];
        for (int i=0; i<2; i++){
            if (i == 0) {
                t[0] = &t1Mv[0][0];
                t[1] = &t1Mv[0][1];
            } else {
                t[0] = &t2Mv[0][0];
                t[1] = &t2Mv[0][1];
            }

            *t[0] = 0;
            *t[1] = 0;

            if (keysState[keyTypes[i][0]]) {
                *t[0] -= incr;
            }
            if (keysState[keyTypes[i][1]]) {
                *t[0] += incr;
            }
            if (keysState[keyTypes[i][2]]) {
                *t[1] -= incr;
            }
            if (keysState[keyTypes[i][3]]) {
                *t[1] += incr;
            }
        }
    }

    void updateAllPositions() {
        int ballPosTmp[2];
        int ballMvTmp[2];

        int t1PosTmp[2];
        int t1MvTmp[2];

        int t2PosTmp[2];
        int t2MvTmp[2];

        std::copy(ballPos, ballPosTmp + 2, ballPosTmp);
        std::copy(ballPos, ballPosTmp + 2, ballPosTmp);
        std::copy(ballPos, ballPosTmp + 2, ballPosTmp);
        std::copy(ballPos, ballPosTmp + 2, ballPosTmp);
        std::copy(ballPos, ballPosTmp + 2, ballPosTmp);

        // cout << "updating ball" << endl;
        // cout << "t1: " << t1Mv[0][0] << ", " << t1Mv[0][1] << endl;
        // cout << "t2: " << t2Mv[0][0] << ", " << t2Mv[0][1] << endl;
        // cout << "ball: " << ballMv[0] << ", " << ballMv[1] << endl;
        updatePos(ballPos, ballMv);

        for (int i=0; i < teamSize; i++) {
            // cout << "updating t1" << endl;
            // cout << "t1: " << t1Mv[0][0] << ", " << t1Mv[0][1] << endl;
            // cout << "t2: " << t2Mv[0][0] << ", " << t2Mv[0][1] << endl;
            // cout << "ball: " << ballMv[0] << ", " << ballMv[1] << endl;
            updatePos(t1Pos[i], t1Mv[i]);
            // cout << "updating t2" << endl;
            // cout << "t1: " << t1Mv[0][0] << ", " << t1Mv[0][1] << endl;
            // cout << "t2: " << t2Mv[0][0] << ", " << t2Mv[0][1] << endl;
            // cout << "ball: " << ballMv[0] << ", " << ballMv[1] << endl;
            updatePos(t2Pos[i], t2Mv[i]);
        }
        // cout << endl;
        // cout << endl;
    }

    void overlap(int &nx, int &ny, int x2, int y2, int (&pos)[2]) {
        float dis = sqrt(pow((nx - x2), 2) + pow((ny - y2), 2));
        if ((pos[0] == x2) && (pos[1] == y2)) return;
        
        // idea is we want the objects to stick together
        // but not overlap, so to prevent a gap between them
        if (dis < (2 * radius)) {
            float dmv = ((2 * radius) - dis + 1) / sqrt(2);
            // cout << "dmv float: " << ((2 * radius) - dis) / sqrt(2) << endl;
            // cout << "dmv: " << dmv << endl;
            // cout << "dis: " << dis << endl;
            // cout << "(nx, ny): " << nx << ", " << ny << endl;
            // cout << "(x2, y2): " << x2 << ", " << y2 << endl;

            nx -= ((x2 - nx) / abs(x2 - nx)) * dmv;
            ny -= ((y2 - ny) / abs(y2 - ny)) * dmv;
            // cout << "after (nx, ny): " << nx << ", " << ny << endl;
            // cout << "dis after: " << sqrt(pow((nx - x2), 2) + pow((ny - y2), 2)) << endl;
        }

    }

    void updatePos(int (&pos)[2], int (&mv)[2]) {
        int newX = pos[0] + mv[0];
        int newY = pos[1] + mv[1];

        // keep object from overlapping x
        cout << "checking ball" << endl;
        overlap(newX, newY, ballPos[0], ballPos[1], pos);
        for (int i=0; i < teamSize; i++) {
            cout << "checking t1" << endl;
            overlap(newX, newY, t1Pos[i][0], t1Pos[i][1], pos);
            cout << "checking t2" << endl;
            overlap(newX, newY, t2Pos[i][0], t2Pos[i][1], pos);
        }
        
        // Keep the object within the soccer field
        int rp = radius + padding;

        if (newX > rp && newX < w() - rp)
            pos[0] = newX;
        if (newY > rp && newY < h() - rp)
            pos[1] = newY;            

    }
};

int main() {
    SoccerFieldWindow window(900, 500, "Soccer");
    window.show();
    return Fl::run();
}
