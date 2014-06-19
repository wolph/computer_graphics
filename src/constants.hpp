/*
 * constants.hpp
 *
 *  Created on: 18 Jun 2014
 *      Author: rick
 */

#ifndef CONSTANTS_HPP_
#define CONSTANTS_HPP_

#ifndef THREADS
#define THREADS 4
#endif

#ifndef MSAA
#define MSAA 1
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
#define RAYTRACE_RES_X 512
#endif
#endif

#ifndef RAYTRACE_RES_Y
#ifdef RAYTRACE_RES
#define RAYTRACE_RES_Y RAYTRACE_RES
#else
#define RAYTRACE_RES_Y 512
#endif
#endif

#endif /* CONSTANTS_HPP_ */