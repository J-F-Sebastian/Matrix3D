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

#ifndef M3D_BMP_H
#define M3D_BMP_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "m3d_color.hh"

class m3d_bmp
{
public:
	explicit m3d_bmp(const std::string &filename);

	int load();
	m3d_color read(int x, int y) const;
	void unload();
	void print() const;

	bool is_valid() const { return valid; }
	bool is_loaded() const { return loaded; }
	int get_error() const { return error; }
	int get_width() const { return width; }
	int get_height() const { return height; }
	uint16_t get_bits_per_pixel() const { return bits_per_pixel; }
	std::size_t get_memory_size() const { return memory_size; }

private:
	int inspect_file();

	std::string filename;
	std::vector<m3d_color> pixels;
	std::vector<m3d_color> palette;
	uint64_t file_size;
	uint64_t pixel_offset;
	uint64_t row_stride;
	std::size_t pixel_count;
	std::size_t memory_size;
	int width;
	int height;
	uint16_t bits_per_pixel;
	uint32_t compression;
	bool top_down;
	bool valid;
	bool loaded;
	int error;
};

#endif // M3D_BMP_H
