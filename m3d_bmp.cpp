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

#include <array>
#include <bit>
#include <cerrno>
#include <fstream>
#include <iostream>
#include <limits>
#include <new>
#include <stdexcept>
#include <utility>
#include <vector>

#include "m3d_bmp.h"

namespace
{
uint16_t read_u16(const uint8_t *data)
{
	return static_cast<uint16_t>(data[0]) |
	       (static_cast<uint16_t>(data[1]) << 8);
}

uint32_t read_u32(const uint8_t *data)
{
	return static_cast<uint32_t>(data[0]) |
	       (static_cast<uint32_t>(data[1]) << 8) |
	       (static_cast<uint32_t>(data[2]) << 16) |
	       (static_cast<uint32_t>(data[3]) << 24);
}

int unsupported_format_error()
{
#ifdef ENOTSUP
	return ENOTSUP;
#else
	return EINVAL;
#endif
}

uint8_t expand_5bit_channel(uint16_t channel)
{
	return static_cast<uint8_t>((channel * 255U + 15U) / 31U);
}
} // namespace

m3d_bmp::m3d_bmp(const std::string &_filename)
	: filename(_filename),
	  file_size(0),
	  pixel_offset(0),
	  row_stride(0),
	  pixel_count(0),
	  memory_size(0),
	  width(0),
	  height(0),
	  bits_per_pixel(0),
	  compression(0),
	  top_down(false),
	  valid(false),
	  loaded(false),
	  error(0)
{
	try
	{
		error = inspect_file();
	}
	catch (const std::bad_alloc &)
	{
		error = ENOMEM;
	}
	valid = (error == 0);
}

int m3d_bmp::inspect_file()
{
	if (filename.empty())
	{
		return EINVAL;
	}

	std::ifstream file(filename, std::ios::binary);
	if (!file)
	{
		return ENOENT;
	}

	file.seekg(0, std::ios::end);
	const std::streamoff actual_size = file.tellg();
	if (!file || actual_size < 54)
	{
		return EINVAL;
	}
	file_size = static_cast<uint64_t>(actual_size);
	file.seekg(0, std::ios::beg);

	std::array<uint8_t, 14> file_header{};
	if (!file.read(reinterpret_cast<char *>(file_header.data()), file_header.size()))
	{
		return EIO;
	}
	if (file_header[0] != 'B' || file_header[1] != 'M')
	{
		return EINVAL;
	}

	const uint32_t declared_file_size = read_u32(file_header.data() + 2);
	pixel_offset = read_u32(file_header.data() + 10);

	std::array<uint8_t, 40> dib_header{};
	if (!file.read(reinterpret_cast<char *>(dib_header.data()), dib_header.size()))
	{
		return EIO;
	}
	const uint32_t dib_size = read_u32(dib_header.data());
	if (dib_size < dib_header.size())
	{
		return unsupported_format_error();
	}
	if (static_cast<uint64_t>(14) + dib_size > file_size)
	{
		return EINVAL;
	}

	const int32_t dib_width = std::bit_cast<int32_t>(read_u32(dib_header.data() + 4));
	const int32_t dib_height = std::bit_cast<int32_t>(read_u32(dib_header.data() + 8));
	const uint16_t planes = read_u16(dib_header.data() + 12);
	bits_per_pixel = read_u16(dib_header.data() + 14);
	compression = read_u32(dib_header.data() + 16);
	const uint32_t colors_used = read_u32(dib_header.data() + 32);

	if (dib_width <= 0 || dib_height == 0 ||
	    dib_height == std::numeric_limits<int32_t>::min() || planes != 1)
	{
		return EINVAL;
	}
	width = dib_width;
	top_down = (dib_height < 0);
	height = top_down ? -dib_height : dib_height;

	if (compression != 0)
	{
		return unsupported_format_error();
	}
	switch (bits_per_pixel)
	{
	case 1:
	case 4:
	case 8:
	case 16:
	case 24:
	case 32:
		break;
	default:
		return unsupported_format_error();
	}

	const uint64_t bits_per_row = static_cast<uint64_t>(width) * bits_per_pixel;
	row_stride = ((bits_per_row + 31U) / 32U) * 4U;
	if (row_stride > std::numeric_limits<uint64_t>::max() / static_cast<uint64_t>(height))
	{
		return EOVERFLOW;
	}
	const uint64_t pixel_data_size = row_stride * static_cast<uint64_t>(height);
	if (pixel_offset > file_size || pixel_data_size > file_size - pixel_offset)
	{
		return EINVAL;
	}
	if (declared_file_size != 0 &&
	    (declared_file_size > file_size || pixel_offset + pixel_data_size > declared_file_size))
	{
		return EINVAL;
	}

	const uint64_t pixel_data_start = static_cast<uint64_t>(14) + dib_size;
	if (pixel_offset < pixel_data_start)
	{
		return EINVAL;
	}

	if (bits_per_pixel <= 8)
	{
		const uint32_t maximum_palette_size = 1U << bits_per_pixel;
		const uint32_t palette_size = (colors_used == 0) ? maximum_palette_size : colors_used;
		const uint64_t palette_end = pixel_data_start + static_cast<uint64_t>(palette_size) * 4U;
		if (palette_size == 0 || palette_size > maximum_palette_size || palette_end > pixel_offset)
		{
			return EINVAL;
		}

		palette.resize(palette_size);
		file.seekg(static_cast<std::streamoff>(pixel_data_start), std::ios::beg);
		for (auto &color : palette)
		{
			std::array<uint8_t, 4> entry{};
			if (!file.read(reinterpret_cast<char *>(entry.data()), entry.size()))
			{
				return EIO;
			}
			color.setColor(entry[2], entry[1], entry[0], 255);
		}
	}

	const uint64_t count = static_cast<uint64_t>(width) * static_cast<uint64_t>(height);
	if (count > std::numeric_limits<std::size_t>::max() / sizeof(m3d_color))
	{
		return EOVERFLOW;
	}
	pixel_count = static_cast<std::size_t>(count);
	memory_size = pixel_count * sizeof(m3d_color);
	return 0;
}

int m3d_bmp::load()
{
	if (!valid)
	{
		return error;
	}
	if (loaded)
	{
		return 0;
	}

	std::ifstream file(filename, std::ios::binary);
	if (!file)
	{
		error = ENOENT;
		return error;
	}
	file.seekg(static_cast<std::streamoff>(pixel_offset), std::ios::beg);
	if (!file)
	{
		error = EIO;
		return error;
	}

	std::vector<m3d_color> decoded;
	std::vector<uint8_t> row;
	try
	{
		decoded.resize(pixel_count);
		if (row_stride > row.max_size() ||
		    row_stride > static_cast<uint64_t>(std::numeric_limits<std::streamsize>::max()))
		{
			error = EOVERFLOW;
			return error;
		}
		row.resize(static_cast<std::size_t>(row_stride));
	}
	catch (const std::bad_alloc &)
	{
		error = ENOMEM;
		return error;
	}
	catch (const std::length_error &)
	{
		error = EOVERFLOW;
		return error;
	}

	for (int file_y = 0; file_y < height; ++file_y)
	{
		if (!file.read(reinterpret_cast<char *>(row.data()), static_cast<std::streamsize>(row_stride)))
		{
			error = EIO;
			return error;
		}
		const int y = top_down ? file_y : height - 1 - file_y;
		for (int x = 0; x < width; ++x)
		{
			m3d_color color;
			const uint8_t *source = row.data();
			switch (bits_per_pixel)
			{
			case 1:
			{
				const uint8_t index = (source[x / 8] >> (7 - (x % 8))) & 0x01;
				if (index >= palette.size())
				{
					error = EINVAL;
					return error;
				}
				color = palette[index];
				break;
			}
			case 4:
			{
				const uint8_t packed = source[x / 2];
				const uint8_t index = (x % 2 == 0) ? (packed >> 4) : (packed & 0x0f);
				if (index >= palette.size())
				{
					error = EINVAL;
					return error;
				}
				color = palette[index];
				break;
			}
			case 8:
			{
				const uint8_t index = source[x];
				if (index >= palette.size())
				{
					error = EINVAL;
					return error;
				}
				color = palette[index];
				break;
			}
			case 16:
			{
				const uint16_t value = read_u16(source + static_cast<std::size_t>(x) * 2U);
				color.setColor(expand_5bit_channel((value >> 10) & 0x1f),
					       expand_5bit_channel((value >> 5) & 0x1f),
					       expand_5bit_channel(value & 0x1f),
					       255);
				break;
			}
			case 24:
			{
				const uint8_t *pixel = source + static_cast<std::size_t>(x) * 3U;
				color.setColor(pixel[2], pixel[1], pixel[0], 255);
				break;
			}
			case 32:
			{
				const uint8_t *pixel = source + static_cast<std::size_t>(x) * 4U;
				color.setColor(pixel[2], pixel[1], pixel[0], 255);
				break;
			}
			default:
				error = unsupported_format_error();
				return error;
			}
			decoded[static_cast<std::size_t>(y) * width + x] = color;
		}
	}

	pixels.swap(decoded);
	loaded = true;
	error = 0;
	return 0;
}

m3d_color m3d_bmp::read(int x, int y) const
{
	if (!loaded)
	{
		throw std::logic_error("BMP pixels have not been loaded");
	}
	if (x < 0 || x >= width || y < 0 || y >= height)
	{
		throw std::out_of_range("BMP pixel coordinates are outside the image");
	}
	return pixels[static_cast<std::size_t>(y) * width + x];
}

void m3d_bmp::unload()
{
	std::vector<m3d_color>().swap(pixels);
	loaded = false;
}

void m3d_bmp::print() const
{
#ifdef DEBUG
	std::cout << "BMP file: " << filename << '\n'
		  << "  Valid: " << (valid ? "yes" : "no") << '\n'
		  << "  File size: " << file_size << " bytes\n"
		  << "  Resolution: " << width << " x " << height << '\n'
		  << "  Format: " << bits_per_pixel << "-bit "
		  << ((bits_per_pixel <= 8) ? "indexed" : "true-color")
		  << ", compression " << compression
		  << (top_down ? ", top-down" : ", bottom-up") << '\n'
		  << "  In-memory size: " << memory_size << " bytes\n"
		  << "  Loaded: " << (loaded ? "yes" : "no")
		  << ", status: " << error << '\n';
#endif
}
