#ifndef M3D_COLOR_H
#define M3D_COLOR_H

#include <cstdint>

class m3d_color
{
public:
	enum
	{
		B_CHANNEL,
		G_CHANNEL,
		R_CHANNEL,
		A_CHANNEL
	};

	enum
	{
		B_SHIFT = 0,
		G_SHIFT = 8,
		R_SHIFT = 16,
		A_SHIFT = 24
	};

	union m3d_color_channels
	{
		uint32_t color;
		uint8_t channels[4];
	};

	/** Default constructor */
	m3d_color()
	{
		mycolor.color = 0;
	};

	/** Default destructor */
	~m3d_color(){};

	/** Copy constructor
	 *  \param other Object to copy from
	 */
	m3d_color(const m3d_color &other) : mycolor(other.mycolor){};

	m3d_color(uint32_t color)
	{
		mycolor.color = color;
	};

	m3d_color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
	{
		setColor(red, green, blue, alpha);
	}

	void operator=(const m3d_color &other)
	{
		mycolor.color = other.mycolor.color;
	}

	m3d_color operator*(const m3d_color &other)
	{
		unsigned tempc;

		m3d_color temp(*this);
		tempc = temp.mycolor.channels[R_CHANNEL] * other.mycolor.channels[R_CHANNEL];
		tempc >>= 8;
		temp.mycolor.channels[R_CHANNEL] = tempc;
		tempc = temp.mycolor.channels[G_CHANNEL] * other.mycolor.channels[G_CHANNEL];
		tempc >>= 8;
		temp.mycolor.channels[G_CHANNEL] = tempc;
		tempc = temp.mycolor.channels[B_CHANNEL] * other.mycolor.channels[B_CHANNEL];
		tempc >>= 8;
		temp.mycolor.channels[B_CHANNEL] = tempc;
		return temp;
	}

	/*
	 *  setColor fills color with the proper values
	 */
	void setColor(uint8_t red,
		      uint8_t green,
		      uint8_t blue,
		      uint8_t alpha);

	uint32_t getColor(void)
	{
		return (mycolor.color);
	}

	uint8_t getChannel(unsigned chanNum)
	{
		return mycolor.channels[chanNum];
	}

	/*
	 * void brighten applies a new intensity to color; uint32_t brighten2
	 * compute and return a copy of color, does not modify the stored value
	 */
	void brighten(float intensity);
	uint32_t brighten2(float intensity);

	/*
	 * void brighten_channels applies a new intensity to the channels in color,
	 * chansint must point to an array of 3 floats for intensities.
	 * uint32_t brighten_channels compute and return a copy of color, does not modify
	 * the stored value.
	 */
	void brighten_channels(float *chansint);
	uint32_t brighten_channels2(float *chansint);

	/*
	 * average_colors compute the average color for num items in array and
	 * store the resulting value in out.
	 * Average is not weighted, i.e. all colors contribute equally to the average.
	 */
	static void average_colors(m3d_color array[], unsigned num, m3d_color &out);

	/*
	 * add_colors compute the sum for num items in array and store the result
	 * in out.
	 * Sums for R, G, B channels are clamped to 255.
	 */
	static void add_colors(m3d_color *array[], unsigned num, m3d_color &out);

	void print(void);

protected:
private:
	inline void change_channel_int(uint8_t &chan, float intensity);
	union m3d_color_channels mycolor;
};

#endif // M3D_COLOR_H
