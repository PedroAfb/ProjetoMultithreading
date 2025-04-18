#include <iostream>
#include <ncurses.h>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <queue>

using namespace std;
using namespace chrono;

atomic<bool> running{true};

class Coord {
public:
    int x, y;
    Coord(int _x, int _y) : x(_x), y(_y) {}
};

class Entity {
public:
    Coord position;
    vector<string> sprite;
    Entity(int x, int y) : position(x, y) {}
    
    void draw() {
        for (size_t i = 0; i < sprite.size(); i++) {
            mvprintw(position.y + i, position.x, sprite[i].c_str());
        }
    }
};

class Pump : public Entity {
public:
    int id;
    bool occupied;
    
    Pump(int x, int y, int _id) : Entity(x, y), id(_id), occupied(false) {
        sprite = {
            "  _____   ",
            " ['''''] ",
            " ]_____[ ",
            " | === | ",
            " | GAS |>",
            " [_____] "
        };
    }
};

class Car : public Entity {
public:
    int id;
    Pump* target;
    bool moving;
    bool fueling;
    system_clock::time_point start_time;
    milliseconds delay;
    
    Car(int x, int y, int _id, milliseconds _delay) : 
        Entity(x, y), id(_id), target(nullptr), 
        moving(false), fueling(false), delay(_delay) {
        start_time = system_clock::now();
        sprite = {
            "         ",
            "   ___   ",
            "  ( 'o)  ",
            " (@\\./@) ",
            "  0   0  ",
            "         "
        };
    }
    
    bool ready() {
        return system_clock::now() > start_time + delay;
    }
    
    void move_right() { position.x++; }
    void move_down() { position.y++; }
    
    void assign_pump(Pump* pump) {
        if (!ready()) return;
        target = pump;
        moving = true;
        pump->occupied = true;
    }
    
    void move_to_pump() {
        if (!ready() || !moving || !target) return;
        
        if (position.x < target->position.x + 17) {
            move_right();
        } else if (position.y < target->position.y) {
            move_down();
        } else {
            moving = false;
            fueling = true;
        }
    }
};

vector<Pump*> pumps;
vector<Car*> cars;
queue<int> car_queue;

void draw_entrance(int x) {
    mvprintw(7, x, "/");
    for (int i = x + 1; i < x + 10; i++) {
        mvprintw(6, i, " ");
        mvprintw(7, i, " ");
    }
    mvprintw(7, x + 10, "\\");
}

void draw_scene() {
    // Limites laterais
    for (int i = 0; i < 20; i++) {
        mvprintw(i, 1, "|");
        mvprintw(i + 8, 20, "|");
    }
    
    // Teto e piso
    for (int j = 21; j < 200; j++) {
        mvprintw(0, j, "_");
        mvprintw(7, j, "_");
        mvprintw(6, j, "_");
    }
    
    // Entradas
    for (int k = 38; k < 180; k += 35) {
        draw_entrance(k);
    }
}

vector<Pump*> create_pumps(int count) {
    vector<Pump*> result;
    for (int i = 0; i < count; i++) {
        int row = i / 4;
        int col = i % 4;
        int x = col * 35 + 30;
        int y = row * 8 + 10;
        result.push_back(new Pump(x, y, i));
    }
    return result;
}

vector<Car*> create_cars(int count) {
    vector<Car*> result;
    for (int i = 0; i < count; i++) {
        result.push_back(new Car(0, 7, i, milliseconds(i * 2000))); // 2 segundos entre carros
        car_queue.push(i);
    }
    return result;
}

void assign_pumps() {
    while (!car_queue.empty()) {
        int car_id = car_queue.front();
        
        if (cars[car_id]->ready()) {
            for (auto& pump : pumps) {
                if (!pump->occupied) {
                    cars[car_id]->assign_pump(pump);
                    car_queue.pop();
                    break;
                }
            }
            break;
        } else {
            break;
        }
    }
}

void run_simulation() {
    while (running) {
        assign_pumps();
        
        for (auto& car : cars) {
            car->move_to_pump();
        }
        
        this_thread::sleep_for(milliseconds(50));
    }
}

int main() {
	
    initscr();
    curs_set(0);
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    
    // Criar 4 bombas e 4 carros
    pumps = create_pumps(11);
    cars = create_cars(10);
    
    thread sim_thread(run_simulation);
    
    while (running) {
        clear();
        draw_scene();
        
        // Desenhar bombas
        for (auto& pump : pumps) {
            pump->draw();
        }
        
        // Desenhar carros
        for (auto& car : cars) {
            car->draw();
        }
        
        refresh();
        
        // Verificar se o usuário quer sair
        int ch = getch();
        if (ch == 'q') {
            running = false;
        }
        
        this_thread::sleep_for(milliseconds(50));
    }
    
    sim_thread.join();
    
    // Limpar memória
    for (auto& car : cars) delete car;
    for (auto& pump : pumps) delete pump;
    
    endwin();
    return 0;
}