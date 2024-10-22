#pragma once
#include "BuildOptions.h"
#include "SimpleGraphics.h"
#include "ColorSpaceConverter.h"

namespace smpl
{
	class DLL_OPTION ComputerVision
	{
	public:
		static const unsigned char RED_CHANNEL = 0;
		static const unsigned char GREEN_CHANNEL = 1;
		static const unsigned char BLUE_CHANNEL = 2;
		static const unsigned char ALPHA_CHANNEL = 3;

		static const unsigned char THRESH_ADAPTIVE_MEAN = 0;
		static const unsigned char THRESH_ADAPTIVE_GUASSIAN = 1;
		

		/**
		 * @brief Calculates the gradient of the specified color channel using
		 * 		central differences.
		 * @param img
		 * 		The image to operate on.
		 * @param type
		 * 		Must be one of the following types.
		 * 			RED_CHANNEL
		 * 			GREEN_CHANNEL
		 * 			BLUE_CHANNEL
		 * 			ALPHA_CHANNEL
		 * @return std::vector<Vec2f>
		 * 		Returns a vector of Vec2f where each Vec2f stores the gradient at that point.
		 * 		It has the size width*height and the vectors are stored scanline by scanline.
		 * 			The same way pixels are stored.
		 */
		static std::vector<Vec2f> calculateGradient(Image* img, unsigned char type);
		
		/**
		 * @brief Calculates an image hash using Average hash.
		 * 		2 images can be compared by determining how many of booleans match (hamming distance) assuming that the vectors are the same size.
		 * 		The resulting vector will have a size of 64 (8x8) or a uint64_t
		 * 		If the image is larger or smaller than the required size (8x8), it is scaled with no respect to its aspect ratio.
		 * 		
		 * 		This is calculated as such:
		 * 			scale
		 * 			convert to grayscale
		 * 			calculate average lumniance
		 * 			for each pixel, check if above or below average
		 * 			return hash
		 * 		
		 * 		(https://www.hackerfactor.com/blog/index.php?/archives/432-Looks-Like-It.html)
		 * 
		 * @param img 
		 * @return uint64_t
		 */
		static uint64_t imageHash(Image* img);

		/**
		 * @brief Calculates an image hash using pHash (perceptual hash)
		 * 		2 images can be compared by determining how many of booleans match (hamming distance).
		 * 		The resulting vector will have a size of 64 bits (8x8) or a uint64_t
		 * 		If the image is larger or smaller than the required size (32x32), it is scaled with no respect to its aspect ratio.
		 * 		
		 * 		This is calculated as such:
		 * 			scale
		 * 			convert to grayscale
		 * 			compute DCT
		 * 			calculate average of top-left 8x8 (minus the first term)
		 * 			for dct value in the top left 8x8, check if above or below average
		 * 			return hash
		 * 
		 * 		Good for image with modifications that are minor (25% or less) and gamma corrections but slower than average hash.
		 * 		(https://www.hackerfactor.com/blog/index.php?/archives/432-Looks-Like-It.html)
		 * 
		 * @param img 
		 * @return uint64_t
		 */
		static uint64_t pHash(Image* img);

		/**
		 * @brief Calculates an image hash using dHash (diferences hash)
		 * 		2 images can be compared by determining how many of booleans match (hamming distance).
		 * 		The resulting vector will have a size of 64 (8x8) or a uint64_t
		 * 		If the image is larger or smaller than the required size (8x8), it is scaled with no respect to its aspect ratio.
		 * 		
		 * 		This is calculated as such:
		 * 			scale
		 * 			convert to grayscale
		 * 			calculate x gradient
		 * 			for each gradient, check if positive or not
		 * 			return hash
		 * 
		 * 		Fast and better than average hash. Almost as good as pHash.
		 * 		(https://www.hackerfactor.com/blog/index.php?/archives/529-Kind-of-Like-That.html)
		 * @param img 
		 * @return uint64_t
		 */
		static uint64_t dHash(Image* img);

		/**
		 * @brief Calculates the hamming distance between 2 image hashes.
		 * 		Assumes that they were computed using the same hash function otherwise it is likely to result in very large distances when it should not.
		 * 		The value returned represents how many bits are different. The smaller the value, the more similar the images are. 
		 * 			An image is likely to be the same if it has around 95% of the bits the same or around 5% are different though this depends on the hash as well.
		 * 			For average hash:
		 * 				Much lower distances may be required to avoid false positives but may result in false negatives. 90% may be good.
		 * 			For perceptual hash:
		 * 				85% same is good.
		 * 			For difference hash:
		 * 				85% or a little higher is good. May result in false positives but very few.
		 * 
		 * 		Returns a negative value if an error occurs. (hash sizes are not the same)
		 * @param img1 
		 * @param img2 
		 * @return int 
		 */
		static int hammingDistanceImageHash(uint64_t img1, uint64_t img2);

		/**
		 * @brief Generates a guassian kernel that can be used in convolution
		 * 		to do guassian blur.
		 * 		A separable kernel but not directly.
		 * 
		 * @param kernelRadius 
		 * @param sigma 
		 * @return Matrix 
		 */
		static Matrix guassianKernel(int kernelRadius, float sigma);
		
		/**
		 * @brief Returns the identity kernel. That is a 3x3 kernel with all 0s except in the middle
		 * 		which is 1.
		 * 		Not a separable kernel
		 * 
		 * @return Matrix 
		 */
		static Matrix identityKernel();

		/**
		 * @brief Returns a sharpen kernel. A 3x3 kernel where the center is the intensity value.
		 * 		Not a separable kernel
		 * 
		 * @param intensity 
		 * @return Matrix 
		 */
		static Matrix sharpenKernel(float intensity);

		/**
		 * @brief Returns a box blur kernel. 
		 * 		Its a uniform kernel and not directly separable.
		 * 			Box blur can be separable by multiplying the separated 1D kernels by the radius
		 * 
		 * @param kernelRadius 
		 * @return Matrix 
		 */
		static Matrix blurKernel(int kernelRadius);

		/**
		 * @brief Converts the image to a matrix only keeping 1 color channel
		 * 
		 * @param img 
		 * @param colorChannel 
		 * @return Matrix 
		 */
		static Matrix imageToMatrix(Image* img, unsigned char colorChannel);

		/**
		 * @brief Converts the matrix to a grayscale image.
		 * 
		 * @param mat 
		 * @return Image* 
		 */
		static Image* matrixToImage(const Matrix& mat);

		/**
		 * @brief Converts the matrix back to an image. Only modifies a single color channel.
		 * 		Allows multiple matrices that represent a different color channel to be combined.
		 * 
		 * @param mat 
		 * @param img 
		 * @param colorChannel 
		 */
		static void matrixToImage(const Matrix& mat, Image* img, unsigned char colorChannel);

		/**
		 * @brief Converts the image to a matrix while keeping all values above some threshold.
		 * 		Converts only 1 color channel
		 * 		Values above the threshold are set to 1. Otherwise, they are set to 0.
		 * 
		 * @param img 
		 * @param threshold 
		 * @param colorChannel 
		 * @param inverse
		 * 		If its inverse, values below the threshold are 1.
		 * @return Matrix 
		 */
		static Matrix thresholding(Image* img, unsigned char threshold, unsigned char colorChannel, bool inverse);

		/**
		 * @brief Converts the image to a matrix while keeping all values above some threshold.
		 * 		Values above the threshold are set to 1. Otherwise, they are set to 0.
		 * 
		 * @param img 
		 * @param threshold 
		 * @param inverse
		 * 		If its inverse, values below the threshold are 1.
		 * @return Matrix 
		 */
		static Matrix thresholding(const Matrix& img, float threshold, bool inverse);

		/**
		 * @brief Converts the image to a matrix while keeping all values above some threshold.
		 * 		Converts only 1 color channel
		 * 		Values above the threshold are set to 1. Otherwise, they are set to 0.
		 * 
		 * @param img 
		 * @param adjustment
		 * @param radius 
		 * @param colorChannel 
		 * @param mode
		 * 		The type of adaptive thresholding
		 * @param inverse
		 * 		If its inverse, values below the threshold are 1.
		 * @return Matrix 
		 */
		static Matrix adaptiveThresholding(Image* img, unsigned char adjustment, unsigned char radius, unsigned char colorChannel, unsigned char mode, bool inverse);

		/**
		 * @brief Converts the image to a matrix while keeping all values above some threshold.
		 * 		Values above the threshold are set to 1. Otherwise, they are set to 0.
		 * 
		 * @param img 
		 * @param threshold 
		 * @param mode
		 * 		The type of adaptive thresholding
		 * @param inverse
		 * 		If its inverse, values below the threshold are 1.
		 * @return Matrix 
		 */
		static Matrix adaptiveThresholding(const Matrix& img, unsigned char mode, bool inverse);

		/**
		 * @brief Readjusts the matrix values so that they fit between the min and max intensities but
		 * 		also maintain relative scaling between the values.
		 * 
		 * 		Useful for converting back into an image where the matrix values may be larger or smaller
		 * 			than what can be represented with an 8bit value.
		 * 
		 * @param baseImg 
		 * @param minIntensity 
		 * @param maxIntensity 
		 */
		static Matrix readjustIntensity(const Matrix& baseImg, double minIntensity, double maxIntensity);

		/**
		 * @brief Computes the convolution of a image and a kernel (which is a matrix).
		 * 		The convolution is the sum of the kernel applied centered on a point for each point in the base image.
		 * 			Meaning (f*g)(x) = SUM( f[i]*g[x-i] ) from -INF to INF
		 * 			which for the discrete case, the sum is limited by the size of the kernel.
		 * 
		 * 		Computed in O(N^2) operations but it is possible to do it in O(NLogN) with a FFT.
		 * 		Returns the baseImage convolved with the kernel.
		 * @param baseImage 
		 * @param kernel 
		 * @param normalize
		 * @return Matrix 
		 */
		static Matrix convolution(const Matrix& baseImage, const Matrix& kernel);
		
		/**
		 * @brief Computes the cross correlation of a image and another image.
		 * 		The cross correlation is the sum of the kernel applied centered on a point for each point in the base image.
		 * 			Meaning (f*g)(x) = SUM( f[i]*g[x+i] ) from -INF to INF
		 * 			which for the discrete case, the sum is limited by the size of the kernel.
		 * 
		 * 		Similar to convolution, but effectively computes the similarity of the baseImage to the kernel at a given point.
		 * 			Note that it is not communitive like a convolution
		 * 
		 * 		Computed in O(N^2) operations but it is possible to do it in O(NLogN) with a FFT.
		 * 		Returns the baseImage convolved with the kernel.
		 * @param baseImage 
		 * @param kernel 
		 * @param normalize
		 * @return Matrix 
		 */
		static Matrix crossCorrelation(const Matrix& baseImage, const Matrix& kernel, bool normalized);

		/**
		 * @brief Computes the vertical histogram of an image. Returns a column vector that is the sum
		 * 		of each column.
		 * 
		 * @param baseImg 
		 * @return Matrix 
		 */
		static Matrix verticalHistogram(const Matrix& baseImg);

		/**
		 * @brief Computes the horizontal histogram of an image. Returns a row vector that is the sum
		 * 		of each row.
		 * 
		 * @param baseImg 
		 * @return Matrix 
		 */
		static Matrix horizontalHistogram(const Matrix& baseImg);

		/**
		 * @brief Computes a color histogram filling x number of bins per color channel
		 * 		where x is an input.
		 * 		Returns a list of bins in the order R, G, B
		 * 			meaning all of the Red bins then Green bins then Blue bins.
		 * 
		 * @param img 
		 * @param bins
		 * 		The total number of bins per channel.
		 * 		Total range (256) is divided by the total number of bins.
		 * @return std::vector<int> 
		 */
		static std::vector<unsigned int> colorHistogram(Image* img, unsigned char bins);
		

		/**
		 * @brief Attempts to find a contour from some starting point x,y that is on the contour using
		 * 		the direction it entered from defined by preX,preY. Due to the exit condition, it may not
		 * 		trace the entire contour. It will not find holes in the shape.
		 * 
		 * @param m 
		 * @param x 
		 * @param y 
		 * @param preX 
		 * @param preY 
		 * @param points 
		 */
		static void mooreNeighborTracing(const Matrix& m, int x, int y, int preX, int preY, std::vector<Vec2f>& points);

		/**
		 * @brief Attempts to find all contours using moore's neighbor tracing.
		 * 		Finds contours that are 8 way connected.
		 * 		Due to the exit condition, it currently may not trace the entire contour.
		 * 
		 * @param img 
		 * @return std::vector<std::vector<Vec2f>> 
		 */
		static std::vector<std::vector<Vec2f>> findContours(Image* img);
		
		/**
		 * @brief Attempts to find all contours using moore's neighbor tracing.
		 * 		Finds contours that are 8 way connected.
		 * 		Due to the exit condition, it currently may not trace the entire contour.
		 * 
		 * @param img 
		 * @return std::vector<std::vector<Vec2f>> 
		 */
		static std::vector<std::vector<Vec2f>> findContours2(Image* img);

	private:
	};
}