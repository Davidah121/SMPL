#include "InternalCompressionHeader.h"

namespace smpl
{
	#pragma region ARITHMETIC

	double Compression::compressArithmetic(std::vector<unsigned char> data, std::vector<double>& percentages)
	{
		//generate percentages
		percentages.clear();
		percentages = std::vector<double>(256);

		int total = (int)data.size();

		std::vector<double> percentageRangeVals = std::vector<double>();
		
		for(size_t i=0; i<data.size(); i++)
		{
			percentages[data[i]] += 1;
		}

		percentageRangeVals.push_back(0);
		for(int i=0; i<256; i++)
		{
			percentages[i] /= total;
			percentageRangeVals.push_back(percentages[i] + percentageRangeVals.back());
		}

		double oldMinPer = 0.0;
		double oldMaxPer = 1.0;
		
		for(size_t i=0; i<data.size(); i++)
		{
			int index = data[i];
			double minPercentage = percentageRangeVals[index];
			double maxPercentage = percentageRangeVals[index+1];

			double nMinPer = oldMinPer + (oldMaxPer-oldMinPer) * minPercentage;
			double nMaxPer = oldMinPer + (oldMaxPer-oldMinPer) * maxPercentage;

			StringTools::println("%c : range %f to %f", data[i], minPercentage, maxPercentage);
			StringTools::println("%c : new range %f to %f", data[i], nMinPer, nMaxPer);

			oldMinPer = nMinPer;
			oldMaxPer = nMaxPer;
		}

		double finalValue = (oldMinPer + oldMaxPer)/2.0;

		StringTools::println("FinalValue: %f", finalValue);
		return finalValue;
	}

	std::vector<unsigned char> Compression::decompressArithmetic(double data, size_t messageSize, std::vector<double> percentages)
	{
		//generate percentageRanges

		std::vector<double> percentageRangeVals = std::vector<double>();
		percentageRangeVals.push_back(0);
		for(int i=0; i<256; i++)
		{
			percentageRangeVals.push_back(percentages[i] + percentageRangeVals.back());
		}

		double oldMinPer = 0.0;
		double oldMaxPer = 1.0;

		std::vector<unsigned char> message = std::vector<unsigned char>();
		while(message.size() < (size_t)messageSize)
		{
			bool valid = false;
			for(size_t i=1; i<percentageRangeVals.size(); i++)
			{
				double minPercentage = percentageRangeVals[i-1];
				double maxPercentage = percentageRangeVals[i];

				double nMinPer = oldMinPer + (oldMaxPer-oldMinPer) * minPercentage;
				double nMaxPer = oldMinPer + (oldMaxPer-oldMinPer) * maxPercentage;
				
				if(data >= nMinPer && data < nMaxPer)
				{
					oldMinPer = nMinPer;
					oldMaxPer = nMaxPer;
					message.push_back((unsigned char)(i-1));
					valid = true;
					break;
				}
			}

			if(valid==false)
			{
				StringTools::println("Error finding data");
				break;
			}
		}
		
		return message;
	}

	#pragma endregion

} //NAMESPACE glib END