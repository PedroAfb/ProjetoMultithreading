#include <iostream>
#include <ncurses.h>
#include <regex>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>
#include <fstream>

using namespace std;
int n_cars;
int n_pumps;


class Coord {
	public:
		int x;
		int y;

		Coord(int _x, int _y) {
			x = _x;
			y = _y;
		}
};

class Entity {
public:
    Coord position;
    vector<string> sprite;

    Entity(int _x0, int _y0)
        : position(_x0, _y0), sprite() {}  // sprite começa vazio

    void draw() {
        for (int i = 0; i < sprite.size(); ++i) {
            mvprintw(position.y + i, position.x, sprite[i].c_str());
        }
    }
};

class Pump : public Entity {
	public:
		Pump(int _x0, int _y0) : Entity(_x0, _y0) {
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
		Pump* target_pump;
		int driving_to_pump = 0;

    Car(int _x0, int _y0) : Entity(_x0, _y0) {
        sprite = {
					  "         ",
            "   ___   ",
            "  ( 'o)  ",
            " (@\\./@) ",
            "  0   0  ",
						"         "
        };
    }

    void drive_downward() {
        position.y++;
    }
		void drive_upward() {
				position.y--;
		}
		void drive_right() {
			  position.x++;
		}
		void draw() {
			Entity::draw();
		}
	
		void go_to_pump(Pump* pump) {
				driving_to_pump = 1;
				target_pump = pump; 
		}

		void drive_to_pump() {
			if (driving_to_pump && position.x < target_pump->position.x+17)
				drive_right();
			else if (driving_to_pump && position.y < target_pump->position.y) {
				drive_downward();
			}
			else {
				driving_to_pump = 0;
				target_pump = nullptr;
			}
		}
};

vector<Pump*> pumps;
vector<Car*> cars; 


void draw_entrance(int x0) {	
		mvprintw(7, x0, "/");
		for(int i = x0 + 1; i < x0+10; i++) { 
			mvprintw(6, i, " ");
			mvprintw(7, i, " ");
		}
		mvprintw(7, x0 + 10, "\\");
}

void initialize_scene(int height, int width) {
	for(int i = 0; i < height; i++) {	
		mvprintw(i, 1, "|");
		mvprintw(i+8, 20, "|");
	}
	mvprintw(0, 20, "\\");
	mvprintw(8, 20, "/");
	for(int j = 21; j < width; j++) {
		mvprintw(0, j, "_");
		mvprintw(7, j, "_");
		mvprintw(6, j, "_");
	}

	for(int k = 38; k < 300; k+=35)
		draw_entrance(k);
}

vector<Pump*> alocate_gas_pumps(int n_pumps) {
	vector<Pump*> pumps;
	for(int i = 0; i < n_pumps; i++) {
		int pumps_per_row = 4;
		int pump_x_margin = 35;
		int pump_y_margin = 8;

		int row = i / pumps_per_row;
		int col = i % pumps_per_row;	
		int x = col * pump_x_margin + 30;
		int y = row * pump_y_margin + 10;		
		
		Pump *pump = new Pump(x,y);
		pumps.push_back(pump);
	}
	return pumps;
}

vector<Car*> alocate_cars(int n_cars) {
	vector<Car*> cars;
	for(int i = 0; i < n_cars; i++) {
		Car *car = new Car(0, 7);
		cars.push_back(car);
	}
	return cars;
}


void run_script() {
	this_thread::sleep_for(std::chrono::seconds(2));
	static regex configCarsRegex(R"(Number of cars: (\d+))");
	regex configPumpsRegex(R"(Number of pumps: (\d+))");
	regex dispatchRegex(R"(Car (\d+) is fueling at pump (\d+))");
	regex fuelInfoRegex(R"(Car (\d+) -> Got (\d+)L \| Remaining: (\d+)L)");
	regex endMessageRegex(R"(Todos os carros abastecidos\. Fim\.)");
	static smatch match;

	string line;
	static ifstream file("log.txt");
	if (!file) {
        return;
    }
	if (!getline(file, line)) {
        return;
    }
	if (regex_search(line, match, configCarsRegex)) {
		n_cars = stoi(match[1]);
	}
	else if (regex_search(line, match, configPumpsRegex)) {
		n_pumps = stoi(match[1]);
	}
	else if (regex_search(line, match, dispatchRegex)) {
		int car = stoi(match[1]);
		int pump = stoi(match[1]);
		cars[car]->go_to_pump(pumps[pump]);
	} 	
}



int main(int argc, char** argv) {
	thread(run_script).detach();
	for (int i = 0; i < 2; i++) 
		run_script();
	initscr();

	cars = alocate_cars(n_cars);
	pumps = alocate_gas_pumps(n_pumps);


	while(1){
		initialize_scene(60, 500);
		run_script();
		for(Car* car : cars) {
			car->draw();
			car->drive_to_pump();
		}
		for(Pump* pump : pumps) {
			pump->draw();
		}

		refresh();
		napms(80);

	}
	
	getch();
	endwin();	

	return 0;
}
