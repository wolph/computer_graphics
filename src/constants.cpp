#include <unordered_map>
#include <fstream>
#include "constants.hpp"
#include <iostream> 
int MSAA = 0;
int PREVIEW_MSAA = 0;
int PREVIEW_RES_X = 0;
int PREVIEW_RES_Y = 0;
int PREVIEW_PARTS = 0;
int PREVIEW_PART_SIZE = 0;
int WINDOW_RES_X = 0;
int WINDOW_RES_Y = 0;
int RAYTRACE_RES_X = 0;
int RAYTRACE_RES_Y = 0;
int MAX_TRACE_DEPTH = 0;

std::unordered_map<std::string, int*> allowed_variables = {
	{"MSAA", &MSAA},
	{"PREVIEW_MSAA", &PREVIEW_MSAA},
	{"PREVIEW_RES_X", &PREVIEW_RES_X},
	{"PREVIEW_RES_Y", &PREVIEW_RES_Y},
	{"PREVIEW_PARTS", &PREVIEW_PARTS},
	{"WINDOW_RES_X", &WINDOW_RES_X},
	{"WINDOW_RES_Y", &WINDOW_RES_Y},
	{"RAYTRACE_RES_X", &RAYTRACE_RES_X},
	{"RAYTRACE_RES_Y", &RAYTRACE_RES_Y},
	{"MAX_TRACE_DEPT", &MAX_TRACE_DEPTH}
};

void loadConfig() {
	std::ifstream in(configname);
	std::string optionname;
	int* optionptr;
	int value; 

	while (in >> optionname >> value) {
		try {
			optionptr = allowed_variables[optionname];
		} catch (const std::out_of_range& oor) {
    		std::cerr << "Invalid option in" << configname << ": " << oor.what() << '\n';
    		continue;
  		}
  		if(!in.good()) {
  			std::cout << "Incorrect config file: " << configname << " at option: " << optionname << '\n';
  		} else {
  			*optionptr = value;
  		}
	} 
	in.close();

	PREVIEW_PART_SIZE = PREVIEW_RES_X / PREVIEW_PARTS;
}



