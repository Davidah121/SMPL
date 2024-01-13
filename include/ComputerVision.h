#pragma once
#include "SimpleGraphics.h"

namespace glib
{
	class ComputerVision
	{
	public:
		static const unsigned char RED_CHANNEL = 0;
		static const unsigned char GREEN_CHANNEL = 1;
		static const unsigned char BLUE_CHANNEL = 2;
		static const unsigned char ALPHA_CHANNEL = 3;

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
		 * @brief Computes the convolution of a image and a kernel (which is a matrix).
		 * 		The convolution is the sum of the kernel applied centered on a point for each point in the base image.
		 * 			Meaning (f*g)(x) = SUM( f[i]*g[x-i] ) from -INF to INF
		 * 			which for the discrete case, the sum is limited by the size of the kernel.
		 * 
		 * 		Computed in O(N^2) operations but it is possible to do it in O(NLogN) with a FFT.
		 * 		Returns the baseImage convolved with the kernel.
		 * @param baseImage 
		 * @param kernel 
		 * @param colorChannel
		 * 		Possible values are
		 * 			RED_CHANNEL
		 * 			GREEN_CHANNEL
		 * 			BLUE_CHANNEL
		 * 			ALPHA_CHANNEL
		 * @return Matrix 
		 */
		static Matrix convolution(Image* baseImage, Matrix* kernel, int colorChannel);

		/**
		 * @brief Computes the normalized convolution of a image and a kernel (which is a matrix).
		 * 		The convolution is the sum of the kernel applied centered on a point for each point in the base image.
		 * 			Meaning (f*g)(x) = SUM( f[i]*g[x-i] ) from -INF to INF
		 * 			which for the discrete case, the sum is limited by the size of the kernel.
		 * 
		 * 		Computed in O(N^2) operations but it is possible to do it in O(NLogN) with a FFT.
		 * 		Returns the baseImage convolved with the kernel.
		 * @param baseImage 
		 * @param kernel 
		 * @param colorChannel
		 * 		Possible values are
		 * 			RED_CHANNEL
		 * 			GREEN_CHANNEL
		 * 			BLUE_CHANNEL
		 * 			ALPHA_CHANNEL
		 * @return Matrix 
		 */
		static Matrix convolutionNormalized(Image* baseImage, Matrix* kernel, int colorChannel);
		
		/**
		 * @brief Computes the cross correlation of a image and another image.
		 * 		Computes the normalized version of the cross correlation.
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
		 * @param colorChannel
		 * 		Possible values are
		 * 			RED_CHANNEL
		 * 			GREEN_CHANNEL
		 * 			BLUE_CHANNEL
		 * 			ALPHA_CHANNEL
		 * @return Matrix 
		 */
		static Matrix crossCorrelation(Image* baseImage, Image* kernel, int colorChannel);
	private:

	};
}