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

#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

FILE *out;

void output_mesh_down(int startbottom, int starttop, int points)
{
	int a, b, c, tricount;

	a = starttop;
	b = startbottom;
	c = startbottom + 1;
	tricount = points;
	while (tricount--)
	{
		fprintf(out, "    {{%d, %d, %d}},\n", a, b, c);
		a++;
		b++;
		c++;
		if (a == starttop + points)
			a = starttop;
		if (b == startbottom + points)
			b = startbottom;
		if (c == startbottom + points)
			c = startbottom;
	}

	a = starttop;
	b = startbottom + 1;
	c = starttop + 1;
	tricount = points;
	while (tricount--)
	{
		fprintf(out, "    {{%d, %d, %d}},\n", a, b, c);
		a++;
		b++;
		c++;
		if (a == starttop + points)
			a = starttop;
		if (b == startbottom + points)
			b = startbottom;
		if (c == starttop + points)
			c = starttop;
	}
}

void output_mesh_up(int startbottom, int starttop, int points)
{
	int a, b, c, tricount;

	a = startbottom;
	b = startbottom + 1;
	c = starttop;
	tricount = points;
	while (tricount--)
	{
		fprintf(out, "    {{%d, %d, %d}},\n", a, b, c);
		a++;
		b++;
		c++;
		if (a == startbottom + points)
			a = startbottom;
		if (b == startbottom + points)
			b = startbottom;
		if (c == starttop + points)
			c = starttop;
	}

	a = starttop;
	b = startbottom + 1;
	c = starttop + 1;
	tricount = points;
	while (tricount--)
	{
		fprintf(out, "    {{%d, %d, %d}},\n", a, b, c);
		a++;
		b++;
		c++;
		if (a == starttop + points)
			a = starttop;
		if (b == startbottom + points)
			b = startbottom;
		if (c == starttop + points)
			c = starttop;
	}
}

void output_mesh_top(int startbottom, int top, int points)
{
	int a, b, c, tricount;

	a = startbottom;
	b = startbottom + 1;
	c = top;
	tricount = points;

	while (tricount--)
	{
		fprintf(out, "    {{%d, %d, %d}},\n", a, b, c);
		a++;
		b++;
		if (a == startbottom + points)
			a = startbottom;
		if (b == startbottom + points)
			b = startbottom;
	}
}

void output_mesh_bottom(int bottom, int starttop, int points)
{
	int a, b, c, tricount;

	a = bottom;
	b = starttop + 1;
	c = starttop;
	tricount = points;

	while (tricount--)
	{
		fprintf(out, "    {{%d, %d, %d}},\n", a, b, c);
		b++;
		c++;
		if (b == starttop + points)
			b = starttop;
		if (c == starttop + points)
			c = starttop;
	}
}

int main(int argc, char *argv[])
{
	int points, circles;
	float x, y, z, alfa, beta, alfad, betad;
	float steps;
	int countp;
	int counter = 0;
	time_t now = time(NULL);

	if (argc < 4)
	{
		printf("Command line: %s Meridians Parallels <filename>\n", argv[0]);
		return -1;
	}
	printf("Generating a sphere...\n\n======================\n\n");

	points = atoi(argv[1]);
	circles = atoi(argv[2]);
	out = fopen(argv[3], "w");
	if (points & 1)
		points++;
	if ((circles & 1) == 0)
		circles++;
	if (!out)
	{
		printf("Invalid file %s\n", argv[2]);
		return -1;
	}
	if (points < 3)
	{
		printf("Too few Meridians %d!\n", points);
		fclose(out);
		return -1;
	}
	if (circles < 3)
	{
		printf("Too few Parallels %d!\n", circles);
		fclose(out);
		return -1;
	}
	alfad = -2.0f * (float)M_PI / (float)(points);
	betad = 0.5f * (float)M_PI / (float)((circles + 1) / 2);

	alfa = beta = 0.0f;
	steps = 0.0f;

	fprintf(out, "/* SPHERE */\n\n/*\n * Auto generated by %s on %s\n", argv[0], ctime(&now));
	fprintf(out, " * Meridians %d\n * Parallels %d\n */\n", points, circles);
	fprintf(out, "%s\n", "struct m3d_input_point sphere[] {");
	/* First circle not in a loop */
	fprintf(out, "%s\n", "    /* Diameter */");
	countp = 0;
	y = 0.0f;
	while (countp++ < points)
	{
		x = 100.0f * (float)cos(alfa);
		z = 100.0f * (float)sin(alfa);
		fprintf(out, "    {{%ff, %ff, %ff, 1.0f}}, // %d\n", x, y, z, counter++);
		alfa += alfad;
	}

	steps += 0.5f;
	beta += betad;
	while (beta < M_PI / 2)
	{
		alfa = steps * alfad;
		alfa = 0.0f;
		countp = 0;
		fprintf(out, "%s\n", "    /* Upper circle */");
		while (countp++ < points)
		{
			x = 100.0f * (float)cos(beta) * (float)cos(alfa);
			y = 100.0f * (float)sin(beta);
			z = 100.0f * (float)cos(beta) * (float)sin(alfa);
			fprintf(out, "    {{%ff, %ff, %ff, 1.0f}}, //%d\n", x, y, z, counter++);
			alfa += alfad;
		}
		fprintf(out, "%s\n", "    /* Lower circle */");
		alfa = steps * alfad;
		alfa = 0.0f;
		countp = 0;
		while (countp++ < points)
		{
			x = 100.0f * (float)cos(beta) * (float)cos(alfa);
			y = -100.0f * (float)sin(beta);
			z = 100.0f * (float)cos(beta) * (float)sin(alfa);
			fprintf(out, "    {{%ff, %ff, %ff, 1.0f}}, //%d\n", x, y, z, counter++);
			alfa += alfad;
		}
		steps += 0.5f;
		beta += betad;
	}

	fprintf(out, "%s\n", "    /* Top */");
	fprintf(out, "    {{0.0f, 100.0f, 0.0f, 1.0f}}, //%d\n", counter++);
	fprintf(out, "%s\n", "    /* Bottom */");
	fprintf(out, "    {{0.0f, -100.0f, 0.0f, 1.0f}} //%d\n", counter++);
	fprintf(out, "%s\n", "};\n");

	/*
	 *
	 *                     5
	 *                     3
	 *                     1
	 *                     0
	 *                     2
	 *                     4
	 *                     6
	 *
	 *
	 */

	fprintf(out, "%s\n", "struct m3d_input_trimesh spheremesh[] = {");
	countp = 0;
	fprintf(out, "%s\n", "    /* Circle */");
	output_mesh_up(countp, countp + points, points);
	fprintf(out, "%s\n", "    /* Circle */");
	output_mesh_down(countp + 2 * points, countp, points);
	countp += points;
	counter -= 3 * points;
	while (counter != 2)
	{
		fprintf(out, "%s\n", "    /* Circle */");
		output_mesh_up(countp, countp + 2 * points, points);
		fprintf(out, "%s\n", "    /* Circle */");
		output_mesh_down(countp + 3 * points, countp + points, points);
		countp += 2 * points;
		counter -= 2 * points;
	}

	fprintf(out, "%s\n", "    /* Top */");
	output_mesh_top(countp, countp + 2 * points, points);
	fprintf(out, "%s\n", "    /* Bottom */");
	output_mesh_bottom(countp + 2 * points + 1, countp + points, points);
	fprintf(out, "%s\n", "};\n");
	fclose(out);

	return 0;
}