#pragma once
#include <unordered_map>
extern int MSAA;
extern int PREVIEW_MSAA;
extern int PREVIEW_RES_X;
extern int PREVIEW_RES_Y;
extern int PREVIEW_PARTS;
extern int PREVIEW_PART_SIZE;
extern int WINDOW_RES_X;
extern int WINDOW_RES_Y;
extern int RAYTRACE_RES_X;
extern int RAYTRACE_RES_Y;
extern int MAX_TRACE_DEPTH;
const static std::string configname = "raytrace.cfg";
extern std::unordered_map<std::string, int*> allowed_variables;
void loadConfig();
