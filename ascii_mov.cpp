#include <chrono>
#include <iostream>
#include <ncurses.h>
#include <regex>
#include <string>
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

class Car;
extern std::vector<Car*> cars;

class Car : public Entity {
public:
		int id;
		Pump* target_pump;
		int driving_to_pump = 0;
		int exiting = 0;

    Car(int _id, int _x0, int _y0) : Entity(_x0, _y0) {
        sprite = {
					  "         ",
            "   ___   ",
            "  ( 'o)  ",
            " (@\\./@) ",
            "  0   0  ",
						"         "
        };
				id = _id;
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
			if (driving_to_pump && position.x < target_pump->position.x+9)
				drive_right();
			else if (driving_to_pump && position.y < target_pump->position.y) {
				drive_downward();
			}
			else {
				driving_to_pump = 0;
				target_pump = nullptr;
			}

		}

		void exit() {
			if (exiting) {
				if (position.y < 100)
					drive_downward();
				else
					cars[id] = NULL;
			}
		}
};

vector<Pump*> pumps;
vector<Car*> cars; 

void draw_entrance(int x0) {	
		mvprintw(8, x0, "/");
		for(int i = x0 + 1; i < x0+10; i++) { 
			mvprintw(7, i, " ");
			mvprintw(8, i, " ");
		}
		mvprintw(8, x0 + 10, "\\");
}

void initialize_scene(int height, int width) {
	for(int i = 8; i < height; i++) {	
		mvprintw(i, 1, "|");
		mvprintw(i, 20, "|");
	}
	mvprintw(8, 1, "\\");
	mvprintw(0, 1, "/");
	mvprintw(0, 20, "\\");
	mvprintw(8, 20, "/");
	for(int j = 21; j < width; j++) {
		mvprintw(0, j, "_");
		mvprintw(8, j, "_");
		mvprintw(7, j, "_");
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


Car* create_car(int id, vector<Car*> cars) {
		int count = std::count_if(cars.begin(), cars.end(), [](Car* car) {
			return car != nullptr && car->driving_to_pump == 1;
		});
		Car *car = new Car(id, 0-count*10, 1);
		return car;
}


long parse_timestamp_ms(const smatch& match) {
    int h = stoi(match[1]);
    int m = stoi(match[2]);
    int s = stoi(match[3]);
    int ms = stoi(match[4]);
    return ((h * 3600 + m * 60 + s) * 1000 + ms);
}

int run_script() {
    static regex timestampRegex(R"(\[(\d{2}):(\d{2}):(\d{2})\.(\d{3})\])");
    static regex configCarsRegex(R"(Number of cars: (\d+))");
    static regex configPumpsRegex(R"(Number of pumps: (\d+))");
    static regex dispatchRegex(R"(Car (\d+) is fueling at pump (\d+))");
    static regex fuelInfoRegex(R"(Car (\d+) -> Got (\d+)L \| Remaining: (\d+)L)");
    static regex endMessageRegex(R"(Todos os carros abastecidos\. Fim\.)");
    static smatch match;

    static long base_ts_ms = -1;
    static auto start_time = chrono::steady_clock::now();

    static ifstream file("log.txt");
    if (!file) return 0;

    string line;
    if (!getline(file, line)) return 0;

    if (regex_search(line, match, timestampRegex)) {
        long current_ts = parse_timestamp_ms(match);
        if (base_ts_ms == -1) {
            base_ts_ms = current_ts;
            start_time = chrono::steady_clock::now();
        }

        long elapsed_ms = chrono::duration_cast<chrono::milliseconds>(
            chrono::steady_clock::now() - start_time
        ).count();

        long log_offset = current_ts - base_ts_ms;

        if (elapsed_ms < log_offset) {
            file.seekg(-(line.length() + 1), ios::cur);
            return 1;
        }
    }

    if (regex_search(line, match, configCarsRegex)) {
        n_cars = stoi(match[1]);
    } else if (regex_search(line, match, configPumpsRegex)) {
        n_pumps = stoi(match[1]);
    } else if (regex_search(line, match, dispatchRegex)) {
        int car_id = stoi(match[1]);
        int pump_id = stoi(match[2]); 
				Car *car = create_car(car_id, cars);
				car->go_to_pump(pumps[pump_id]);
				cars[car_id]=car;
    } else if (regex_search(line, match, fuelInfoRegex)) {
				int car_id = stoi(match[1]);
				cars[car_id]->exiting = 1;
		}

    return 1;
}





int main(int argc, char** argv) {
	for (int i = 0; i < 2; i++) 
		run_script();
	initscr();
	
	cars.resize(n_cars);
	pumps = alocate_gas_pumps(n_pumps);
	

	while(run_script()){
		initialize_scene(60, 500);
		run_script();
		for(Car* car : cars) {
			if (car != NULL) {
				car->draw();
				car->drive_to_pump();
				car->exit();
			}
		}
		for(Pump* pump : pumps) {
			pump->draw();
		}
		refresh();
		napms(50);
	}
		int count = std::count_if(cars.begin(), cars.end(), [](Car* car) {
			return car != nullptr;
		});

	while(count > 0) {
		count = std::count_if(cars.begin(), cars.end(), [](Car* car) {
			return car != nullptr;
		});	
	  for(Car* car : cars) {
			if (car != NULL) {
				car->draw();
				car->drive_to_pump();
				car->exit();
			}
		}
		refresh();
		napms(50);
	}
	
	getch();
	endwin();	

	return 0;
}
