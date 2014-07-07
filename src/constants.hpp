#pragma once

#ifndef MSAA
#define MSAA 2
#endif

#ifndef PREVIEW_MSAA
#define PREVIEW_MSAA 1
#endif

#ifndef PREVIEW_RES_X
#ifdef PREVIEW_RES
#define PREVIEW_RES_X PREVIEW_RES
#else
#define PREVIEW_RES_X 128
#endif
#endif

#ifndef PREVIEW_RES_Y
#ifdef PREVIEW_RES
#define PREVIEW_RES_Y PREVIEW_RES
#else
#define PREVIEW_RES_Y 128
#endif
#endif

#define PREVIEW_PARTS 32
#define PREVIEW_PART_SIZE (PREVIEW_RES_X / PREVIEW_PARTS)

#ifndef WINDOW_RES_X
#ifdef WINDOW_RES
#define WINDOW_RES_X WINDOW_RES
#else
#define WINDOW_RES_X 1024
#endif
#endif

#ifndef WINDOW_RES_Y
#ifdef WINDOW_RES
#define WINDOW_RES_Y WINDOW_RES
#else
#define WINDOW_RES_Y 1024
#endif
#endif

#ifndef RAYTRACE_RES_X
#ifdef RAYTRACE_RES
#define RAYTRACE_RES_X RAYTRACE_RES
#else
#define RAYTRACE_RES_X 1024
#endif
#endif

#ifndef RAYTRACE_RES_Y
#ifdef RAYTRACE_RES
#define RAYTRACE_RES_Y RAYTRACE_RES
#else
#define RAYTRACE_RES_Y 1024
#endif
#endif

#ifndef MAX_TRACE_DEPTH
#define MAX_TRACE_DEPTH 200
#endif

#define X 0
#define Y 1
#define Z 2
