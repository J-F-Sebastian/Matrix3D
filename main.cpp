/*
 * Matrix3D
 *
 * Copyright (C) 1995 - 2025 Diego Gallizioli
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define NELEMENTS(x) (sizeof(x) / sizeof(x[0]))

#include "m3d_renderer_wireframe.hh"
#include "m3d_renderer_flat.hh"
#include "m3d_renderer_gouraud.hh"
#include "m3d_renderer_phong.hh"
#include "cubeobject.mes"
#include "sphereobject.mes"

static struct m3d_input_point madcube[] = {/* first square, y coords positive */
					   {{100.0, 100.0, 100.0, 1.0}},
					   {{100.0, 100.0, -100.0, 1.0}},
					   {{-100.0, 100.0, -100.0, 1.0}},
					   {{-100.0, 100.0, 100.0, 1.0}},
					   /* second square, y coords negative */
					   {{100.0, -100.0, 100.0, 1.0}},
					   {{100.0, -100.0, -100.0, 1.0}},
					   {{-100.0, -100.0, -100.0, 1.0}},
					   {{-100.0, -100.0, 100.0, 1.0}}};

static struct m3d_input_point viewpoint = {{1000.0f, 1000.0f, 1000.0f, 1.0f}};
static struct m3d_input_point viewpointat = {{0.0f, 0.0f, 0.0f, 1.0f}};
static struct m3d_input_point lightpos = {{500.0f, 500.0f, 500.0f, 1.0f}};
static struct m3d_input_point lightat = {{50.0f, 50.0f, 50.0f, 1.0f}};

static m3d_render_object cubeo, cubeo2, cubeo3, sphereo;
static m3d_renderer *renderer[6];
static m3d_display *display;
static m3d_world *world;

#if 1
#include <windows.h>
#include <wingdi.h>
#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include "m3d_display_wingdi.hh"

using namespace std;

static HWND m_hwnd;

static LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static m3d_render_object *objcmd = &cubeo;
	static int renderer_index = 0;
	static float stepping = 1.0f;

	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0); // Exit message loop
		return 0;
	case WM_PAINT:
		renderer[renderer_index]->render(*world);
		return 0;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			return 0;
		case '1': // 1
			objcmd = &cubeo;
			return 0;
		case '2': // 2
			objcmd = &cubeo2;
			return 0;
		case '3': // 3
			objcmd = &cubeo3;
			return 0;
		case '4': // 4
			objcmd = &sphereo;
			return 0;
		case VK_RIGHT:
			objcmd->yaw(stepping);
			break;
		case VK_LEFT:
			objcmd->yaw(-stepping);
			break;
		case VK_UP:
			objcmd->pitch(stepping);
			break;
		case VK_DOWN:
			objcmd->pitch(-stepping);
			break;
		case 'Z': // Z
			objcmd->roll(stepping);
			break;
		case 'X': // X
			objcmd->roll(-stepping);
			break;
		case VK_ADD:
		/* FALLTHRU */
		case VK_OEM_PLUS:
			std::cout << "Stepping+: " << stepping << std::endl;
			if (stepping < 179.0f)
				stepping += 1.25f;
			return 0;
		case VK_SUBTRACT:
		/* FALLTHRU */
		case VK_OEM_MINUS:
			std::cout << "Stepping-: " << stepping << std::endl;
			if (stepping > -179.f)
				stepping -= 1.25f;
			return 0;
		case VK_SPACE:
			renderer_index = (renderer_index + 1) % 5;
			break;
		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
		}
		InvalidateRect(hwnd, NULL, FALSE);
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

INT WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, INT nCmdShow)
{
	MSG msg;

	std::cout << "Matrix3D" << std::endl;

	// Ignore the return value because we want to continue running even in the
	// unlikely event that HeapSetInformation fails.
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	// Register the window class.
	WNDCLASSEX wcex = {sizeof(WNDCLASSEX)};
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = wndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInst;
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.lpszClassName = "Matrix3D";

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL, "Failed RegisterClassEx", "ERROR", MB_ICONERROR);
		return 1;
	}

	// Create the application window.
	int xres = 1024;
	int yres = 768;

	// Create the application window.
	m_hwnd = CreateWindow("Matrix3D",
			      "Matrix3D demo",
			      WS_OVERLAPPEDWINDOW,
			      CW_USEDEFAULT,
			      CW_USEDEFAULT,
			      xres,
			      yres,
			      NULL,
			      NULL,
			      wcex.hInstance,
			      NULL);

	m3d_color cubecolor(255, 1, 1, 0);
	m3d_color cubecolor2(1, 255, 1, 0);
	m3d_color cubecolor3(1, 1, 255, 0);
	m3d_color spherecolor(64, 128, 255, 0);
	m3d_color ambientcolor(255, 255, 255, 0);

	m3d_point_light_source lsource(lightpos, ambientcolor, 100.0f, 1.0f, 0.1f, 20000.0f);
	// m3d_spot_light_source lsource(lightat, lightpos, ambientcolor, 100.0f, 1.0f, 1.0f, 20000.0f);
	m3d_ambient_light ambient(ambientcolor, 0.4f);
	m3d_camera camera(viewpoint, viewpointat, xres, yres);
	world = new m3d_world(ambient, camera);
	display = new m3d_display_wingdi(m_hwnd, xres, yres);
	renderer[0] = new m3d_renderer_wireframe(display);
	renderer[1] = new m3d_renderer_flat(display);
	renderer[2] = new m3d_renderer_shaded(display);
	renderer[3] = new m3d_renderer_shaded_gouraud(display);
	renderer[4] = new m3d_renderer_shaded_phong(display);
	cubeo.create(cube, NELEMENTS(cube), cubemesh, NELEMENTS(cubemesh), cubecolor);
	cubeo2.create(cube, NELEMENTS(cube), cubemesh, NELEMENTS(cubemesh), cubecolor2);
	cubeo3.create(cube, NELEMENTS(cube), cubemesh, NELEMENTS(cubemesh), cubecolor3);
	sphereo.create(sphere, NELEMENTS(sphere), spheremesh, NELEMENTS(spheremesh), spherecolor);
	m3d_vector newpos(300.0f, 0.0f, 0.0f);
	m3d_vector newpos2(0.0f, 300.0f, 0.0f);
	m3d_vector newpos3(0.0f, 0.0f, 300.0f);
	m3d_vector newpos4(200.0f, 300.0f, -300.0f);
	cubeo.move(newpos);
	cubeo2.move(newpos2);
	cubeo3.move(newpos3);
	sphereo.move(newpos4);
	world->add_light_source(lsource);
	world->add_object(cubeo);
	world->add_object(cubeo2);
	world->add_object(cubeo3);
	world->add_object(sphereo);
	// world.print();

	(void)ShowWindow(m_hwnd, nCmdShow);
	(void)UpdateWindow(m_hwnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	delete world;
	delete display;
	delete[] &renderer;
	return 0;
}

#else

#define SDL_MAIN_HANDLED
#include <iostream>
#include <SDL.h>
#include <ctime>

using namespace std;

int main(int argc, char *argv[])
{
	m3d_color cubecolor(255, 1, 1, 0);
	m3d_color cubecolor2(1, 255, 1, 0);
	m3d_color cubecolor3(1, 1, 255, 0);
	m3d_color spherecolor(64, 128, 255, 0);
	m3d_color ambientcolor(255, 255, 255, 0);
	m3d_render_object *objcmd;
	m3d_ambient_light ambient(ambientcolor, 0.4f);
	m3d_point_light_source lsource(lightpos, ambientcolor, 100.0f, 1.0f, 0.1f, 20000.0f);
	// m3d_spot_light_source lsource(lightat, lightpos, ambientcolor, 100.0f, 1.0f, 1.0f, 20000.0f);
	m3d_camera camera(viewpoint, viewpointat, 1024, 768);
	world = new m3d_world(ambient, camera);
	display = new m3d_display_wingdi(m_hwnd, xres, yres);
	m3d_renderer *renderer[6];
	SDL_Event event;
	int goon = 1;
	int renderer_index = 0;
	float stepping = 1.0f;
	time_t chronograph = time(NULL);

	std::cout << "Matrix3D" << std::endl;

#if !defined(_MSC_VER)
	__builtin_cpu_init();
	if (__builtin_cpu_is("intel"))
		std::cout << "running on a Intel CPU." << std::endl;
	if (__builtin_cpu_is("amd"))
		std::cout << "running on a AMD CPU." << std::endl;
	if (__builtin_cpu_supports("mmx"))
		std::cout << "with MMX instructions." << std::endl;
	if (__builtin_cpu_supports("sse"))
		std::cout << "with SSE instructions." << std::endl;
	if (__builtin_cpu_supports("sse2"))
		std::cout << "with SSE2 instructions." << std::endl;
	if (__builtin_cpu_supports("sse3"))
		std::cout << "with SSE3 instructions." << std::endl;
	if (__builtin_cpu_supports("ssse3"))
		std::cout << "with SSSE3 instructions." << std::endl;
	if (__builtin_cpu_supports("sse4.1"))
		std::cout << "with SSE4.1 instructions." << std::endl;
	if (__builtin_cpu_supports("sse4.2"))
		std::cout << "with SSE4.2 instructions." << std::endl;
	if (__builtin_cpu_supports("sse4a"))
		std::cout << "with SSE4A instructions." << std::endl;
#endif

	renderer[0] = new m3d_renderer_wireframe(display);
	renderer[1] = new m3d_renderer_flat(display);
	renderer[2] = new m3d_renderer_shaded(display);
	renderer[3] = new m3d_renderer_shaded_gouraud(display);
	renderer[4] = new m3d_renderer_shaded_phong(display);
	cubeo.create(cube, NELEMENTS(cube), cubemesh, NELEMENTS(cubemesh), cubecolor);
	cubeo2.create(cube, NELEMENTS(cube), cubemesh, NELEMENTS(cubemesh), cubecolor2);
	cubeo3.create(cube, NELEMENTS(cube), cubemesh, NELEMENTS(cubemesh), cubecolor3);
	sphereo.create(sphere, NELEMENTS(sphere), spheremesh, NELEMENTS(spheremesh), spherecolor);
	m3d_vector newpos(300.0f, 0.0f, 0.0f);
	m3d_vector newpos2(0.0f, 300.0f, 0.0f);
	m3d_vector newpos3(0.0f, 0.0f, 300.0f);
	m3d_vector newpos4(200.0f, 300.0f, -300.0f);
	cubeo.move(newpos);
	cubeo2.move(newpos2);
	cubeo3.move(newpos3);
	sphereo.move(newpos4);
	world.add_light_source(lsource);
	world.add_object(cubeo);
	world.add_object(cubeo2);
	world.add_object(cubeo3);
	world.add_object(sphereo);
	// world.print();

	time_t chronograph2 = time(NULL);
	std::cout << "SETUP [" << renderer_index << "]: " << chronograph2 - chronograph << std::endl;
	renderer[renderer_index]->render(*world);
	objcmd = &cubeo;
#if 0
	chronograph = time(NULL);
	while (goon)
	{
		objcmd = &cubeo;
		objcmd->yaw(stepping);
		objcmd->pitch(stepping);
		objcmd->roll(stepping);

		objcmd = &cubeo2;
		objcmd->yaw(stepping);
		objcmd->pitch(stepping);
		objcmd->roll(stepping);

		objcmd = &cubeo3;
		objcmd->yaw(stepping);
		objcmd->pitch(stepping);
		objcmd->roll(stepping);

		objcmd = &sphereo;
		objcmd->yaw(stepping);
		objcmd->pitch(stepping);
		objcmd->roll(stepping);
		if (stepping < 359.0f)
		{
			stepping += 0.07f;
			// SDL_Delay(50);
		}
		else
		{
			stepping = 1.0f;
			if (renderer_index > 1)
				goon = 0;
			else
			{
				chronograph2 = time(NULL);
				std::cout << "RENDERING [" << renderer_index << "]: " << chronograph2 - chronograph << std::endl;
				chronograph = chronograph2;
				renderer_index++;
			}
		}

		renderer[renderer_index]->render(*world);
		SDL_PollEvent(&event);
	}
	chronograph2 = time(NULL);
	std::cout << "RENDERING [" << renderer_index << "]: " << chronograph2 - chronograph << std::endl;

	delete world;
	delete display;
	delete[] &renderer;
	return 0;

#endif
	renderer[renderer_index]->render(world);

	while (goon && SDL_WaitEvent(&event))
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_1:
				objcmd = &cubeo;
				break;
			case SDLK_2:
				objcmd = &cubeo2;
				break;
			case SDLK_3:
				objcmd = &cubeo3;
				break;
			case SDLK_4:
				objcmd = &sphereo;
				break;
			case SDLK_RIGHT:
				objcmd->yaw(stepping);
				break;
			case SDLK_LEFT:
				objcmd->yaw(-stepping);
				break;
			case SDLK_UP:
				objcmd->pitch(stepping);
				break;
			case SDLK_DOWN:
				objcmd->pitch(-stepping);
				break;
			case SDLK_z:
				objcmd->roll(stepping);
				break;
			case SDLK_x:
				objcmd->roll(-stepping);
				break;
			case SDLK_r:
				renderer_index++;
				if (renderer_index > 4)
					renderer_index = 0;
				break;
			case SDLK_ESCAPE:
				goon = 0;
				break;
			case SDLK_PLUS:
				if (stepping < 179.0f)
					stepping += 1.25f;
				break;
			case SDLK_MINUS:
				if (stepping > -179.f)
					stepping -= 1.25f;
				break;
			}
			break;
		default:
			continue;
		}
		// world.print();
		renderer[renderer_index]->render(world);
	}

	delete world;
	delete display;
	delete[] &renderer;
	return 0;
}

#endif
