#pragma once
#include "BuildOptions.h"
#include <vector>
#include "Object.h"
#include "KDTree.h"
#include "MathExt.h"
#include "GeneralExceptions.h"

namespace smpl
{

	struct Color
	{
		unsigned char red = 0;
		unsigned char green = 0;
		unsigned char blue = 0;
		unsigned char alpha = 0;

		bool const operator==(const Color& o)
		{
			return memcmp(this, &o, sizeof(Color)) == 0;
		}
		bool const operator!=(const Color& o)
		{
			return memcmp(this, &o, sizeof(Color)) != 0;
		}
	};

	struct Color4f
	{
		UFP16 red;
		UFP16 green;
		UFP16 blue;
		UFP16 alpha;
		
		Color4f operator-(Color4f other) { return Color4f{red - other.red, green - other.green, blue - other.blue, alpha - other.alpha}; }
		Color4f operator+(Color4f other) { return Color4f{red + other.red, green + other.green, blue + other.blue, alpha + other.alpha}; }

		Color4f operator-() { return Color4f{-red, -green, -blue, -alpha}; }

		friend Color4f operator*(double a, Color4f other) { return Color4f{a*other.red, a*other.green, a*other.blue, a*other.alpha}; }
		void operator+=(Color4f other)
		{ 
			red += other.red; 
			green += other.green;
			blue += other.blue;
			alpha += other.alpha;
		}
		void operator-=(Color4f other)
		{ 
			red -= other.red; 
			green -= other.green;
			blue -= other.blue;
			alpha -= other.alpha;
		}

		void operator*=(double other)
		{ 
			red *= other; 
			green *= other;
			blue *= other;
			alpha *= other;
		}
		void operator/=(double other)
		{ 
			red /= other; 
			green /= other;
			blue /= other;
			alpha /= other;
		}

		Color4f operator*(double other) { return Color4f{red * other, green * other, blue * other, alpha * other}; }
		Color4f operator/(double other) { return Color4f{red / other, green / other, blue / other, alpha / other}; }

		bool const operator==(const Color4f& o)
		{
			return memcmp(this, &o, sizeof(Color4f)) == 0;
		}
		bool const operator!=(const Color4f& o)
		{
			return memcmp(this, &o, sizeof(Color4f)) != 0;
		}
	};

	class DLL_OPTION ColorPalette : public Object
	{
	public:

		/**
		 * @brief Create a ColorPalette Object.
		 * 		It is a list of colors that can also be efficiently search over for
		 * 		the closest color by using KD-Trees for any given image.
		 */
		ColorPalette();

		/**
		 * @brief Copies a valid ColorPalette object
		 */
		ColorPalette(const ColorPalette& other);

		/**
		 * @brief Copies a valid ColorPalette object
		 */
		void operator=(const ColorPalette& other);

		// ColorPalette(ColorPalette&& other);

		/**
		 * @brief Destroys a ColorPalette Object.
		 */
		~ColorPalette();

		/**
		 * @brief Disposes data for the ColorPalette Object.
		 * 
		 */
		void dispose();

		//Object and RootClass Stuff
		static const RootClass globalClass;
		virtual const RootClass* getClass();
		
		/**
		 * @brief Adds a new color to the palette.
		 * 		Duplicates may be allowed if palette mode has been set to false
		 * @param c
		 * 		The color to add.
		 */
		void addNewColor(Color c);

		/**
		 * @brief Gets a color by its index in the palette.
		 * @param i
		 * 		The index to check.
		 * @return Color
		 * 		returns a valid color if index is valid. If index is invalid and USE_EXCEPTIONS is defined, an
		 * 		OutOfBounds error is thrown. Otherwise, the default Color is returned.
		 */
		Color getColor(size_t index);

		/**
		 * @brief Gets a color by its index in the palette. Returns the reference to the color
		 * @param i
		 * 		The index to check.
		 * @return Color
		 * 		returns a valid color pointer if index is valid. If index is invalid and USE_EXCEPTIONS is defined, an
		 * 		OutOfBounds error is thrown. Otherwise, a nullptr is returned.
		 */
		Color* getColorRef(size_t index);

		/**
		 * @brief Gets the index of the color in the palette if it exists.
		 * @param c
		 * 		The color to check.
		 * @return size_t
		 * 		returns a valid index if the color was found. -1 is returned otherwise.
		 */
		size_t getColorIndex(Color c);
		
		/**
		 * @brief Returns the color in the palette closest to the desired color.
		 * 		It uses the Euclidean distance as the measure.
		 * 		KD-Trees are used to speed this operation up and performs with O(Log(N)) on average
		 * @param c
		 * 		The desired color
		 * @return Color
		 * 		The closest color found in the palette.
		 */
		Color getClosestColor(Color c);

		/**
		 * @brief Returns the index of the color in the palette closest to the desired color.
		 * 		It uses the Euclidean distance as the measure.
		 * 		KD-Trees are used to speed this operation up and performs with O(Log(N)) on average
		 * @param c
		 * 		The desired color
		 * @return size_t
		 * 		The index of the closest color found in the palette.
		 */
		size_t getClosestColorIndex(Color c);

		/**
		 * @brief Returns the amount of colors in the palette.
		 * @return size_t
		 */
		size_t getSize();

		/**
		 * @brief Returns the palette used as a vector. It is a reference to avoid copying data.
		 * @return std::vector<Color>&
		 */
		std::vector<Color>& getPalette();

		/**
		 * @brief Sets the palette to the specified list of colors.
		 * @param p
		 * 		The list of colors.
		 */
		void setPalette(std::vector<Color> p);

		/**
		 * @brief Clears the palette.
		 */
		void clearPalette();

		/**
		 * @brief Sets the palette mode to be either unique colors only or all colors
		 * @param uniqueOnly
		 * 		false = Duplicates allowed | true = Only unique colors
		 */
		void setPaletteMode(bool uniqueOnly);

		/**
		 * @brief Creates a color palette using a specified amount of red, green and blue colors.
		 * 		The total size of the palette is equal to:
		 * 			reds*greens*blues
		 * 		Each color channel is divided evenly by the number of colors in it.
		 * @param reds
		 * 		The amount of red colors in the red color channel.
		 * @param greens
		 * 		The amount of green colors in the green color channel.
		 * @param blue
		 * 		The amount of blue colors in the blue color channel.
		 * @return ColorPalette
		 * 		returns the created color palette.
		 */
		static ColorPalette createColorPalette(int reds, int greens, int blues);

		static const unsigned char MEAN_CUT = 0;
		static const unsigned char MEDIAN_CUT = 1;
		static const unsigned char K_MEANS = 2;

		/**
		 * @brief Creates an optimal color palette using 1 of 3 possible algorithms.
		 * 		MEAN_CUT:
		 * 			It is a custom created method that produces results close to K_MEANS but is the fastest of the algorithms available.
		 * 		MEDIAN_CUT:
		 * 			A slower less accurate method. 2nd fastest algorithm.
		 * 		K_MEANS:
		 * 			A simple implementation of KMeans is used. It is the most accurate of the 3 methods but is slower and its results are a bit random.
		 * 			10 iterations are used.
		 * @param colorArray
		 * 		The array of colors to generate the palette over. Generally just the images pixels.
		 * 		Note that the expected color space is the RGB color space however, any color space can be used.
		 * @param size
		 * 		The size the array.
		 * @param colors
		 * 		The amount of colors to generate for the palette.
		 * @param type
		 * 		Must be MEAN_CUT, MEDIAN_CUT, or K_MEANS
		 * @param convertToLab
		 * 		Whether to convert from RGB color space to LAB color space for generating the palette.
		 * 		May not work properly if the colorArray is not in the RGB color space.
		 * 		By default, it is set to false.
		 * @param uniqueOnly
		 * 		Whether to use only unique colors from the array or allow duplicate colors from the array when generating the palette.
		 * 		By default, it is set to false.
		 * @return ColorPalette
		 * 		Returns a color palette in the same color space as the input if successful.
		 * 		Otherwise, returns an empty color palette.
		 */
		static ColorPalette generateOptimalPalette(Color* colorArray, size_t size, int colors, unsigned char type, bool convertToLab = false, bool uniqueOnly = false);

		/**
		 * @brief Rebalances the KD-Tree for color searching.
		 * 		Best called after adding all colors to the palette or when large changes are made and color searching is needed
		 * 		to maintain the speed of the color searching.
		 */
		void reBalance();

	private:
		void copy(const ColorPalette& other);

		static std::vector<Color> meanCut(std::vector<Color> colorArray, int colors);
		static std::vector<Color> medianCut(std::vector<Color> colorArray, int colors);
		static std::vector<Color> kMeans(std::vector<Color> colorArray, int colors, size_t maxIterations);
		
		std::vector<Color> paletteArr = std::vector<Color>();
		KDTree<unsigned char>* paletteTree = new KDTree<unsigned char>(3);
		bool uniquePalette = true;
	};

}