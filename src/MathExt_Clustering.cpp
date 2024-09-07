#include "MathExt.h"
#include "Sort.h"
#include "LCG.h"
#include "System.h"

namespace smpl
{

	#pragma region VEC2F_CLUSTERING

	std::vector<std::vector<Vec2f>> MathExt::meanCut(std::vector<Vec2f> arr, int clusters, bool meansOnly)
	{
		struct BoxInfo
		{
			std::vector<Vec2f> arr = std::vector<Vec2f>();
			Vec2f error;
			Vec2f averageVal;
			bool beenSet = false;
		};

		std::vector< BoxInfo > boxes = std::vector< BoxInfo >();
		boxes.push_back( BoxInfo() );

		boxes[0].arr = arr;
		boxes[0].error = Vec2f();
		boxes[0].averageVal = Vec2f();

		while(true)
		{
			for(BoxInfo& f : boxes)
			{
				if(!f.beenSet)
				{
					//first, find mean value
					Vec2f meanVal = Vec2f();
					float meanMult = 1.0/f.arr.size();
					for(size_t i=0; i<f.arr.size(); i++)
					{
						meanVal += f.arr[i] * meanMult;
					}

					//then find error^2 for all dimensions
					Vec2f error = Vec2f();
					for(size_t i=0; i<f.arr.size(); i++)
					{
						error.x += MathExt::sqr(meanVal.x - f.arr[i].x);
						error.y += MathExt::sqr(meanVal.y - f.arr[i].y);
					}

					f.averageVal = meanVal;
					f.error = error;
					f.beenSet = true;
				}
			}

			if(boxes.size() >= (size_t)clusters)
			{
				break;
			}

			int indexOfMostError = 0;
			float mostError = 0;

			for(size_t i=0; i<boxes.size(); i++)
			{
				Vec2f errVec = boxes[i].error;
				float sumError = errVec.x + errVec.y;

				if(sumError > mostError)
				{
					mostError = sumError;
					indexOfMostError = i;
				}
			}
			
			if(boxes[indexOfMostError].arr.size() <= 1)
			{
				//can't split
				break;
			}

			BoxInfo box = boxes[indexOfMostError];

			Vec2f avg = box.averageVal;
			
			std::vector<Vec2f> split1 = std::vector<Vec2f>();
			std::vector<Vec2f> split2 = std::vector<Vec2f>();
			
			//split the box along the part with the most error
			if(box.error.x > box.error.y)
			{
				//split by x
				for(size_t i=0; i<box.arr.size(); i++)
				{
					if(box.arr[i].x < avg.x)
					{
						split1.push_back( box.arr[i] );
					}
					else
					{
						split2.push_back( box.arr[i] );
					}
				}
			}
			else
			{
				//split by y
				for(size_t i=0; i<box.arr.size(); i++)
				{
					if(box.arr[i].y < avg.y)
					{
						split1.push_back( box.arr[i] );
					}
					else
					{
						split2.push_back( box.arr[i] );
					}
				}
			}

			boxes[indexOfMostError].arr = split1;
			boxes[indexOfMostError].beenSet = false;
			
			boxes.push_back( BoxInfo() );
			boxes.back().arr = split2;
			boxes.back().beenSet = false;

			//after split, you have 2 boxes. repeat algorithm on all boxes.
			//Split on the box with the most error which is the sum of all of the boxes x,y,z error
			//Insures that you end up with the amount of boxes you need to get the appropriate amount of clusters
			//Average the clusters on each box to get the final paletteArr.
		}

		
		std::vector< std::vector<Vec2f>> finalGroups = std::vector<std::vector<Vec2f>>();
		if(meansOnly)
		{
			for(BoxInfo b : boxes)
			{
				//push averages as groups
				finalGroups.push_back( {b.averageVal} );
			}
		}
		else
		{
			for(BoxInfo b : boxes)
			{
				//push groups
				finalGroups.push_back( b.arr );
			}
		}

		return finalGroups;
	}

	std::vector<std::vector<Vec2f>> MathExt::medianCut(std::vector<Vec2f> arr, int clusters, bool meansOnly)
	{
		std::vector<int> endPos = std::vector<int>();
		std::vector<Vec2f> sortArray = std::vector<Vec2f>(arr);

		endPos.push_back(arr.size());

		while(endPos.size() < clusters)
		{
			size_t currentSize = endPos.size();
			for(size_t i=0; i<currentSize; i++)
			{
				int start = 0;
				int end = endPos[i];
				if(i!=0)
				{
					start = endPos[i-1];
				}

				//sort by most range
				
				Vec2f minVals = sortArray[start];
				Vec2f maxVals = sortArray[start];
				Vec2f ranges = Vec2f();
				int dimensionToSortBy = 0;
				
				for(int i=start; i<end; i++)
				{
					if(sortArray[i].x < minVals.x)
					{
						minVals.x = sortArray[i].x;
					}
					if(sortArray[i].x > maxVals.x)
					{
						maxVals.x = sortArray[i].x;
					}

					if(sortArray[i].y < minVals.y)
					{
						minVals.y = sortArray[i].y;
					}
					if(sortArray[i].y > maxVals.y)
					{
						maxVals.y = sortArray[i].y;
					}
				}

				ranges = maxVals - minVals;
				if(ranges.x > ranges.y)
				{
					dimensionToSortBy = 0;
				}
				else
				{
					dimensionToSortBy = 1;
				}

				Sort::mergeSort<Vec2f>(sortArray.data()+start, end-start, [dimensionToSortBy](Vec2f a, Vec2f b) -> bool{
					switch(dimensionToSortBy)
					{
						case 0:
							return a.x < b.x;
						case 1:
							return a.y < b.y;
						default:
							return false;
					}
				});
			}

			std::vector<int> newEndPos = std::vector<int>();

			newEndPos.push_back(endPos[0]/2);
			newEndPos.push_back(endPos[0]);

			for(size_t i=1; i<currentSize; i++)
			{
				int midIndex = (endPos[i-1] + endPos[i])/2;
				
				newEndPos.push_back(midIndex);
				newEndPos.push_back(endPos[i]);
			}

			endPos = newEndPos;
		}


		//average out each sections for the final set of clusters
		std::vector<std::vector<Vec2f>> finalclusters = std::vector<std::vector<Vec2f>>();

		if(meansOnly)
		{
			for(size_t i=0; i<endPos.size(); i++)
			{
				int start = 0;
				int end = endPos[i];
				if(i!=0)
				{
					start = endPos[i-1];
				}

				Vec2f avgVal = Vec2f();
				int divVal = end-start;
				for(int k=start; k<end; k++)
				{
					Vec2f c = sortArray[k];
					avgVal += c;
				}

				avgVal/=divVal;

				finalclusters.push_back( {avgVal} );
			}
		}
		else
		{
			for(size_t i=0; i<endPos.size(); i++)
			{
				int start = 0;
				int end = endPos[i];
				if(i!=0)
				{
					start = endPos[i-1];
				}

				std::vector<Vec2f> group = std::vector<Vec2f>();
				for(int k=start; k<end; k++)
				{
					group.push_back(sortArray[k]);
				}

				finalclusters.push_back( group );
			}
		}

		return finalclusters;
	}

	std::vector<std::vector<Vec2f>> MathExt::kMeans(std::vector<Vec2f> arr, int clusters, int maxIterations, bool meansOnly)
	{
		struct BoxInfo
		{
			std::vector<Vec2f> clusters = std::vector<Vec2f>();
			Vec2f averageVal = Vec2f();
		};

		//pick k means
		std::vector< BoxInfo > groups = std::vector< BoxInfo >();
		
		//pick randomly
		unsigned int currentAmount = 0;
		LCG lcg = LCG( (unsigned int)System::getCurrentTimeNano(), 12354, 0, arr.size());

		for(int i=0; i<clusters; i++)
		{
			int v = lcg.get();
			BoxInfo b = BoxInfo();
			b.averageVal = arr[v];

			groups.push_back( b );
		}

		//do k-means
		
		for(int i=0; i<maxIterations; i++)
		{
			//clear all groups
			for(int k=0; k<groups.size(); k++)
			{
				groups[k].clusters.clear();
			}

			//group into k groups
			for(size_t k=0; k<arr.size(); k++)
			{
				int minIndex = -1;
				float minDis = std::numeric_limits<float>::max();

				//measure distance from all means
				for(size_t j=0; j<groups.size(); j++)
				{
					float thisDis = 0;
					Vec2f lengthVec = Vec2f();
					lengthVec = groups[j].averageVal - arr[k];

					thisDis = MathExt::vecLength(lengthVec);

					if(thisDis < minDis)
					{
						minIndex = j;
						minDis = thisDis;
					}
				}

				if(minIndex >= 0)
				{
					groups[minIndex].clusters.push_back( arr[k] );
				}
			}

			//recompute average
			bool same = true;
			for(size_t j=0; j<groups.size(); j++)
			{
				Vec2f avg = Vec2f();
				float divVal = 1.0 / groups[j].clusters.size();

				for(Vec2f c : groups[j].clusters)
				{
					avg += c * divVal;
				}

				if(groups[j].averageVal != avg)
				{
					same = false;
				}

				groups[j].averageVal = avg;
			}

			if(same)
			{
				break;
			}
		}

		std::vector<std::vector<Vec2f>> finalGroups = std::vector<std::vector<Vec2f>>();

		if(meansOnly)
		{
			for(BoxInfo k : groups)
			{
				finalGroups.push_back( {k.averageVal} );
			}
		}
		else
		{
			for(BoxInfo k : groups)
			{
				finalGroups.push_back( k.clusters );
			}
		}

		return finalGroups;
	}

	#pragma endregion

	#pragma region VEC3F_CLUSTERING

	std::vector<std::vector<Vec3f>> MathExt::meanCut(std::vector<Vec3f> arr, int clusters, bool meansOnly)
	{
		struct BoxInfo
		{
			std::vector<Vec3f> arr = std::vector<Vec3f>();
			Vec3f error;
			Vec3f averageVal;
			bool beenSet = false;
		};

		std::vector< BoxInfo > boxes = std::vector< BoxInfo >();
		boxes.push_back( BoxInfo() );

		boxes[0].arr = arr;
		boxes[0].error = Vec3f();
		boxes[0].averageVal = Vec3f();

		while(true)
		{
			for(BoxInfo& f : boxes)
			{
				if(!f.beenSet)
				{
					//first, find mean value
					Vec3f meanVal = Vec3f();
					float meanMult = 1.0/f.arr.size();
					for(size_t i=0; i<f.arr.size(); i++)
					{
						meanVal += f.arr[i] * meanMult;
					}

					//then find error^2 for all dimensions
					Vec3f error = Vec3f();
					for(size_t i=0; i<f.arr.size(); i++)
					{
						error.x += MathExt::sqr(meanVal.x - f.arr[i].x);
						error.y += MathExt::sqr(meanVal.y - f.arr[i].y);
						error.z += MathExt::sqr(meanVal.z - f.arr[i].z);
					}

					f.averageVal = meanVal;
					f.error = error;
					f.beenSet = true;
				}
			}

			if(boxes.size() >= (size_t)clusters)
			{
				break;
			}

			int indexOfMostError = 0;
			float mostError = 0;

			for(size_t i=0; i<boxes.size(); i++)
			{
				Vec3f errVec = boxes[i].error;
				float sumError = errVec.x + errVec.y + errVec.z;

				if(sumError > mostError)
				{
					mostError = sumError;
					indexOfMostError = i;
				}
			}
			
			if(boxes[indexOfMostError].arr.size() <= 1)
			{
				//can't split
				break;
			}

			BoxInfo box = boxes[indexOfMostError];

			Vec3f avg = box.averageVal;
			
			std::vector<Vec3f> split1 = std::vector<Vec3f>();
			std::vector<Vec3f> split2 = std::vector<Vec3f>();
			
			//split the box along the part with the most error
			if(box.error.x > box.error.y && box.error.x > box.error.z)
			{
				//split by x
				for(size_t i=0; i<box.arr.size(); i++)
				{
					if(box.arr[i].x < avg.x)
					{
						split1.push_back( box.arr[i] );
					}
					else
					{
						split2.push_back( box.arr[i] );
					}
				}
			}
			else if(box.error.y > box.error.x && box.error.y > box.error.z)
			{
				//split by y
				for(size_t i=0; i<box.arr.size(); i++)
				{
					if(box.arr[i].y < avg.y)
					{
						split1.push_back( box.arr[i] );
					}
					else
					{
						split2.push_back( box.arr[i] );
					}
				}
			}
			else
			{
				//split by z
				for(size_t i=0; i<box.arr.size(); i++)
				{
					if(box.arr[i].z < avg.z)
					{
						split1.push_back( box.arr[i] );
					}
					else
					{
						split2.push_back( box.arr[i] );
					}
				}
			}

			boxes[indexOfMostError].arr = split1;
			boxes[indexOfMostError].beenSet = false;
			
			boxes.push_back( BoxInfo() );
			boxes.back().arr = split2;
			boxes.back().beenSet = false;

			//after split, you have 2 boxes. repeat algorithm on all boxes.
			//Split on the box with the most error which is the sum of all of the boxes x,y,z error
			//Insures that you end up with the amount of boxes you need to get the appropriate amount of clusters
			//Average the clusters on each box to get the final paletteArr.
		}

		
		std::vector< std::vector<Vec3f>> finalGroups = std::vector<std::vector<Vec3f>>();
		if(meansOnly)
		{
			for(BoxInfo b : boxes)
			{
				//push averages as groups
				finalGroups.push_back( {b.averageVal} );
			}
		}
		else
		{
			for(BoxInfo b : boxes)
			{
				//push groups
				finalGroups.push_back( b.arr );
			}
		}

		return finalGroups;
	}

	std::vector<std::vector<Vec3f>> MathExt::medianCut(std::vector<Vec3f> arr, int clusters, bool meansOnly)
	{
		std::vector<int> endPos = std::vector<int>();
		std::vector<Vec3f> sortArray = std::vector<Vec3f>(arr);

		endPos.push_back(arr.size());

		while(endPos.size() < clusters)
		{
			size_t currentSize = endPos.size();
			for(size_t i=0; i<currentSize; i++)
			{
				int start = 0;
				int end = endPos[i];
				if(i!=0)
				{
					start = endPos[i-1];
				}

				//sort by most range
				
				Vec3f minVals = sortArray[start];
				Vec3f maxVals = sortArray[start];
				Vec3f ranges = Vec3f();
				int dimensionToSortBy = 0;
				
				for(int i=start; i<end; i++)
				{
					if(sortArray[i].x < minVals.x)
					{
						minVals.x = sortArray[i].x;
					}
					if(sortArray[i].x > maxVals.x)
					{
						maxVals.x = sortArray[i].x;
					}

					if(sortArray[i].y < minVals.y)
					{
						minVals.y = sortArray[i].y;
					}
					if(sortArray[i].y > maxVals.y)
					{
						maxVals.y = sortArray[i].y;
					}

					if(sortArray[i].z < minVals.z)
					{
						minVals.z = sortArray[i].z;
					}
					if(sortArray[i].z > maxVals.z)
					{
						maxVals.z = sortArray[i].z;
					}
				}

				ranges = maxVals - minVals;
				if(ranges.x > ranges.y && ranges.x > ranges.z)
				{
					dimensionToSortBy = 0;
				}
				else if(ranges.y > ranges.x && ranges.y > ranges.z)
				{
					dimensionToSortBy = 1;
				}
				else
				{
					dimensionToSortBy = 2;
				}

				Sort::mergeSort<Vec3f>(sortArray.data()+start, end-start, [dimensionToSortBy](Vec3f a, Vec3f b) -> bool{
					switch(dimensionToSortBy)
					{
						case 0:
							return a.x < b.x;
						case 1:
							return a.y < b.y;
						case 2:
							return a.z < b.z;
						default:
							return false;
					}
				});
			}

			std::vector<int> newEndPos = std::vector<int>();

			newEndPos.push_back(endPos[0]/2);
			newEndPos.push_back(endPos[0]);

			for(size_t i=1; i<currentSize; i++)
			{
				int midIndex = (endPos[i-1] + endPos[i])/2;
				
				newEndPos.push_back(midIndex);
				newEndPos.push_back(endPos[i]);
			}

			endPos = newEndPos;
		}


		//average out each sections for the final set of clusters
		std::vector<std::vector<Vec3f>> finalclusters = std::vector<std::vector<Vec3f>>();

		if(meansOnly)
		{
			for(size_t i=0; i<endPos.size(); i++)
			{
				int start = 0;
				int end = endPos[i];
				if(i!=0)
				{
					start = endPos[i-1];
				}

				Vec3f avgVal = Vec3f();
				int divVal = end-start;
				for(int k=start; k<end; k++)
				{
					Vec3f c = sortArray[k];
					avgVal += c;
				}

				avgVal/=divVal;

				finalclusters.push_back( {avgVal} );
			}
		}
		else
		{
			for(size_t i=0; i<endPos.size(); i++)
			{
				int start = 0;
				int end = endPos[i];
				if(i!=0)
				{
					start = endPos[i-1];
				}

				std::vector<Vec3f> group = std::vector<Vec3f>();
				for(int k=start; k<end; k++)
				{
					group.push_back(sortArray[k]);
				}

				finalclusters.push_back( group );
			}
		}

		return finalclusters;
	}

	std::vector<std::vector<Vec3f>> MathExt::kMeans(std::vector<Vec3f> arr, int clusters, int maxIterations, bool meansOnly)
	{
		struct BoxInfo
		{
			std::vector<Vec3f> clusters = std::vector<Vec3f>();
			Vec3f averageVal = Vec3f();
		};

		//pick k means
		std::vector< BoxInfo > groups = std::vector< BoxInfo >();
		
		//pick randomly
		unsigned int currentAmount = 0;
		LCG lcg = LCG( (unsigned int)System::getCurrentTimeNano(), 12354, 0, arr.size());

		for(int i=0; i<clusters; i++)
		{
			int v = lcg.get();
			BoxInfo b = BoxInfo();
			b.averageVal = arr[v];

			groups.push_back( b );
		}

		//do k-means
		
		for(int i=0; i<maxIterations; i++)
		{
			//clear all groups
			for(size_t k=0; k<groups.size(); k++)
			{
				groups[k].clusters.clear();
			}

			//group into k groups
			for(size_t k=0; k<arr.size(); k++)
			{
				int minIndex = -1;
				float minDis = std::numeric_limits<float>::max();

				//measure distance from all means
				for(size_t j=0; j<groups.size(); j++)
				{
					float thisDis = 0;
					Vec3f lengthVec = Vec3f();
					lengthVec = groups[j].averageVal - arr[k];

					thisDis = MathExt::vecLength(lengthVec);

					if(thisDis < minDis)
					{
						minIndex = j;
						minDis = thisDis;
					}
				}

				if(minIndex >= 0)
				{
					groups[minIndex].clusters.push_back( arr[k] );
				}
			}

			//recompute average
			bool same = true;
			for(size_t j=0; j<groups.size(); j++)
			{
				Vec3f avg = Vec3f();
				float divVal = 1.0 / groups[j].clusters.size();

				for(Vec3f c : groups[j].clusters)
				{
					avg += c * divVal;
				}

				if(groups[j].averageVal != avg)
				{
					same = false;
				}

				groups[j].averageVal = avg;
			}

			if(same)
			{
				break;
			}
		}

		std::vector<std::vector<Vec3f>> finalGroups = std::vector<std::vector<Vec3f>>();

		if(meansOnly)
		{
			for(BoxInfo k : groups)
			{
				finalGroups.push_back( {k.averageVal} );
			}
		}
		else
		{
			for(BoxInfo k : groups)
			{
				finalGroups.push_back( k.clusters );
			}
		}

		return finalGroups;
	}

	#pragma endregion

	#pragma region VEC4F_CLUSTERING

	std::vector<std::vector<Vec4f>> MathExt::meanCut(std::vector<Vec4f> arr, int clusters, bool meansOnly)
	{
		struct BoxInfo
		{
			std::vector<Vec4f> arr = std::vector<Vec4f>();
			Vec4f error;
			Vec4f averageVal;
			bool beenSet = false;
		};

		std::vector< BoxInfo > boxes = std::vector< BoxInfo >();
		boxes.push_back( BoxInfo() );

		boxes[0].arr = arr;
		boxes[0].error = Vec4f();
		boxes[0].averageVal = Vec4f();

		while(true)
		{
			for(BoxInfo& f : boxes)
			{
				if(!f.beenSet)
				{
					//first, find mean value
					Vec4f meanVal = Vec4f();
					float meanMult = 1.0/f.arr.size();
					for(size_t i=0; i<f.arr.size(); i++)
					{
						meanVal += f.arr[i] * meanMult;
					}

					//then find error^2 for all dimensions
					Vec4f error = Vec4f();
					for(size_t i=0; i<f.arr.size(); i++)
					{
						error.x += MathExt::sqr(meanVal.x - f.arr[i].x);
						error.y += MathExt::sqr(meanVal.y - f.arr[i].y);
						error.z += MathExt::sqr(meanVal.z - f.arr[i].z);
						error.w += MathExt::sqr(meanVal.w - f.arr[i].w);
					}

					f.averageVal = meanVal;
					f.error = error;
					f.beenSet = true;
				}
			}

			if(boxes.size() >= (size_t)clusters)
			{
				break;
			}

			int indexOfMostError = 0;
			float mostError = 0;

			for(size_t i=0; i<boxes.size(); i++)
			{
				Vec4f errVec = boxes[i].error;
				float sumError = errVec.x + errVec.y + errVec.z + errVec.w;

				if(sumError > mostError)
				{
					mostError = sumError;
					indexOfMostError = i;
				}
			}
			
			if(boxes[indexOfMostError].arr.size() <= 1)
			{
				//can't split
				break;
			}

			BoxInfo box = boxes[indexOfMostError];

			Vec4f avg = box.averageVal;
			
			std::vector<Vec4f> split1 = std::vector<Vec4f>();
			std::vector<Vec4f> split2 = std::vector<Vec4f>();
			
			//split the box along the part with the most error
			if(box.error.x > box.error.y && box.error.x > box.error.z && box.error.x > box.error.w)
			{
				//split by x
				for(size_t i=0; i<box.arr.size(); i++)
				{
					if(box.arr[i].x < avg.x)
					{
						split1.push_back( box.arr[i] );
					}
					else
					{
						split2.push_back( box.arr[i] );
					}
				}
			}
			else if(box.error.y > box.error.x && box.error.y > box.error.z && box.error.y > box.error.w)
			{
				//split by y
				for(size_t i=0; i<box.arr.size(); i++)
				{
					if(box.arr[i].y < avg.y)
					{
						split1.push_back( box.arr[i] );
					}
					else
					{
						split2.push_back( box.arr[i] );
					}
				}
			}
			else if(box.error.z > box.error.x && box.error.z > box.error.y && box.error.z > box.error.w)
			{
				//split by z
				for(size_t i=0; i<box.arr.size(); i++)
				{
					if(box.arr[i].z < avg.z)
					{
						split1.push_back( box.arr[i] );
					}
					else
					{
						split2.push_back( box.arr[i] );
					}
				}
			}
			else
			{
				//split by w
				for(size_t i=0; i<box.arr.size(); i++)
				{
					if(box.arr[i].w < avg.w)
					{
						split1.push_back( box.arr[i] );
					}
					else
					{
						split2.push_back( box.arr[i] );
					}
				}
			}

			boxes[indexOfMostError].arr = split1;
			boxes[indexOfMostError].beenSet = false;
			
			boxes.push_back( BoxInfo() );
			boxes.back().arr = split2;
			boxes.back().beenSet = false;

			//after split, you have 2 boxes. repeat algorithm on all boxes.
			//Split on the box with the most error which is the sum of all of the boxes x,y,z error
			//Insures that you end up with the amount of boxes you need to get the appropriate amount of clusters
			//Average the clusters on each box to get the final paletteArr.
		}

		
		std::vector< std::vector<Vec4f>> finalGroups = std::vector<std::vector<Vec4f>>();
		if(meansOnly)
		{
			for(BoxInfo b : boxes)
			{
				//push averages as groups
				finalGroups.push_back( {b.averageVal} );
			}
		}
		else
		{
			for(BoxInfo b : boxes)
			{
				//push groups
				finalGroups.push_back( b.arr );
			}
		}

		return finalGroups;
	}

	std::vector<std::vector<Vec4f>> MathExt::medianCut(std::vector<Vec4f> arr, int clusters, bool meansOnly)
	{
		std::vector<int> endPos = std::vector<int>();
		std::vector<Vec4f> sortArray = std::vector<Vec4f>(arr);

		endPos.push_back(arr.size());

		while(endPos.size() < (size_t)clusters)
		{
			size_t currentSize = endPos.size();
			for(size_t i=0; i<currentSize; i++)
			{
				int start = 0;
				int end = endPos[i];
				if(i!=0)
				{
					start = endPos[i-1];
				}

				//sort by most range
				
				Vec4f minVals = sortArray[start];
				Vec4f maxVals = sortArray[start];
				Vec4f ranges = Vec4f();
				int dimensionToSortBy = 0;
				
				for(int i=start; i<end; i++)
				{
					if(sortArray[i].x < minVals.x)
					{
						minVals.x = sortArray[i].x;
					}
					if(sortArray[i].x > maxVals.x)
					{
						maxVals.x = sortArray[i].x;
					}

					if(sortArray[i].y < minVals.y)
					{
						minVals.y = sortArray[i].y;
					}
					if(sortArray[i].y > maxVals.y)
					{
						maxVals.y = sortArray[i].y;
					}

					if(sortArray[i].z < minVals.z)
					{
						minVals.z = sortArray[i].z;
					}
					if(sortArray[i].z > maxVals.z)
					{
						maxVals.z = sortArray[i].z;
					}

					if(sortArray[i].w < minVals.w)
					{
						minVals.w = sortArray[i].w;
					}
					if(sortArray[i].w > maxVals.w)
					{
						maxVals.w = sortArray[i].w;
					}
				}

				ranges = maxVals - minVals;
				if(ranges.x > ranges.y && ranges.x > ranges.z && ranges.x > ranges.w)
				{
					dimensionToSortBy = 0;
				}
				else if(ranges.y > ranges.x && ranges.y > ranges.z && ranges.y > ranges.w)
				{
					dimensionToSortBy = 1;
				}
				else if(ranges.z > ranges.x && ranges.z > ranges.y && ranges.z > ranges.w)
				{
					dimensionToSortBy = 2;
				}
				else
				{
					dimensionToSortBy = 3;
				}

				Sort::mergeSort<Vec4f>(sortArray.data()+start, end-start, [dimensionToSortBy](Vec4f a, Vec4f b) -> bool{
					switch(dimensionToSortBy)
					{
						case 0:
							return a.x < b.x;
						case 1:
							return a.y < b.y;
						case 2:
							return a.z < b.z;
						case 3:
							return a.w < b.w;
						default:
							return false;
					}
				});
			}

			std::vector<int> newEndPos = std::vector<int>();

			newEndPos.push_back(endPos[0]/2);
			newEndPos.push_back(endPos[0]);

			for(size_t i=1; i<currentSize; i++)
			{
				int midIndex = (endPos[i-1] + endPos[i])/2;
				
				newEndPos.push_back(midIndex);
				newEndPos.push_back(endPos[i]);
			}

			endPos = newEndPos;
		}


		//average out each sections for the final set of clusters
		std::vector<std::vector<Vec4f>> finalclusters = std::vector<std::vector<Vec4f>>();

		if(meansOnly)
		{
			for(size_t i=0; i<endPos.size(); i++)
			{
				int start = 0;
				int end = endPos[i];
				if(i!=0)
				{
					start = endPos[i-1];
				}

				Vec4f avgVal = Vec4f();
				int divVal = end-start;
				for(int k=start; k<end; k++)
				{
					Vec4f c = sortArray[k];
					avgVal += c;
				}

				avgVal/=divVal;

				finalclusters.push_back( {avgVal} );
			}
		}
		else
		{
			for(size_t i=0; i<endPos.size(); i++)
			{
				int start = 0;
				int end = endPos[i];
				if(i!=0)
				{
					start = endPos[i-1];
				}

				std::vector<Vec4f> group = std::vector<Vec4f>();
				for(int k=start; k<end; k++)
				{
					group.push_back(sortArray[k]);
				}

				finalclusters.push_back( group );
			}
		}

		return finalclusters;
	}

	std::vector<std::vector<Vec4f>> MathExt::kMeans(std::vector<Vec4f> arr, int clusters, int maxIterations, bool meansOnly)
	{
		struct BoxInfo
		{
			std::vector<Vec4f> clusters = std::vector<Vec4f>();
			Vec4f averageVal = Vec4f();
		};

		//pick k means
		std::vector< BoxInfo > groups = std::vector< BoxInfo >();
		
		//pick randomly
		unsigned int currentAmount = 0;
		LCG lcg = LCG( (unsigned int)System::getCurrentTimeNano(), 12354, 0, arr.size());

		for(int i=0; i<clusters; i++)
		{
			int v = lcg.get();
			BoxInfo b = BoxInfo();
			b.averageVal = arr[v];

			groups.push_back( b );
		}

		//do k-means
		
		for(int i=0; i<maxIterations; i++)
		{
			//clear all groups
			for(size_t k=0; k<groups.size(); k++)
			{
				groups[k].clusters.clear();
			}

			//group into k groups
			for(size_t k=0; k<arr.size(); k++)
			{
				int minIndex = -1;
				float minDis = std::numeric_limits<float>::max();

				//measure distance from all means
				for(size_t j=0; j<groups.size(); j++)
				{
					float thisDis = 0;
					Vec4f lengthVec = Vec4f();
					lengthVec = groups[j].averageVal - arr[k];

					thisDis = MathExt::vecLength(lengthVec);

					if(thisDis < minDis)
					{
						minIndex = j;
						minDis = thisDis;
					}
				}

				if(minIndex >= 0)
				{
					groups[minIndex].clusters.push_back( arr[k] );
				}
			}

			//recompute average
			bool same = true;
			for(size_t j=0; j<groups.size(); j++)
			{
				Vec4f avg = Vec4f();
				float divVal = 1.0 / groups[j].clusters.size();

				for(Vec4f c : groups[j].clusters)
				{
					avg += c * divVal;
				}

				if(groups[j].averageVal != avg)
				{
					same = false;
				}

				groups[j].averageVal = avg;
			}

			if(same)
			{
				break;
			}
		}

		std::vector<std::vector<Vec4f>> finalGroups = std::vector<std::vector<Vec4f>>();

		if(meansOnly)
		{
			for(BoxInfo k : groups)
			{
				finalGroups.push_back( {k.averageVal} );
			}
		}
		else
		{
			for(BoxInfo k : groups)
			{
				finalGroups.push_back( k.clusters );
			}
		}

		return finalGroups;
	}

	#pragma endregion

	#pragma region GENERAL_VECTOR_CLUSTERING

	std::vector<std::vector<GeneralVector>> MathExt::meanCut(std::vector<GeneralVector> arr, int clusters, bool meansOnly)
	{
		int dimensions = arr[0].getSize();
		struct BoxInfo
		{
			std::vector<GeneralVector> arr = std::vector<GeneralVector>();
			GeneralVector error;
			GeneralVector averageVal;
			bool beenSet = false;
		};

		std::vector< BoxInfo > boxes = std::vector< BoxInfo >();
		boxes.push_back( BoxInfo() );

		boxes[0].arr = arr;
		boxes[0].error = GeneralVector(dimensions);
		boxes[0].averageVal = GeneralVector(dimensions);

		while(true)
		{
			for(BoxInfo& f : boxes)
			{
				if(!f.beenSet)
				{
					//first, find mean value
					GeneralVector meanVal = GeneralVector(dimensions);
					float meanMult = 1.0/f.arr.size();
					for(size_t i=0; i<f.arr.size(); i++)
					{
						GeneralVector v = f.arr[i];
						for(int k=0; k<dimensions; k++)
						{
							meanVal[k] += v[k] * meanMult;
						}
					}

					//then find error^2 for all dimensions
					GeneralVector error = GeneralVector(dimensions);
					for(size_t i=0; i<f.arr.size(); i++)
					{
						GeneralVector v = f.arr[i];
						for(int k=0; k<dimensions; k++)
						{
							error[k] += MathExt::sqr(meanVal[k] - v[k]);
						}
					}

					f.averageVal = meanVal;
					f.error = error;
					f.beenSet = true;
				}
			}

			if(boxes.size() >= clusters)
			{
				break;
			}

			int indexOfMostError = 0;
			float mostError = 0;

			for(size_t i=0; i<boxes.size(); i++)
			{
				GeneralVector errVec = boxes[i].error;
				float sumError = 0;
				for(int k=0; k<dimensions; k++)
				{
					sumError += errVec[k];
				}

				if(sumError > mostError)
				{
					mostError = sumError;
					indexOfMostError = i;
				}
			}
			
			if(boxes[indexOfMostError].arr.size() <= 1)
			{
				//can't split
				break;
			}

			BoxInfo box = boxes[indexOfMostError];

			GeneralVector avg = box.averageVal;
			
			std::vector<GeneralVector> split1 = std::vector<GeneralVector>();
			std::vector<GeneralVector> split2 = std::vector<GeneralVector>();
			
			//split the box along the part with the most error
			int dimensionWithMostError = 0;
			float dimensionMostError = 0;
			for(int i=0; i<dimensions; i++)
			{
				if(box.error[i] > dimensionMostError)
				{
					dimensionWithMostError = i;
					dimensionMostError = box.error[i];
				}
			}

			for(size_t i=0; i<box.arr.size(); i++)
			{
				GeneralVector c = box.arr[i];
				if(c[dimensionWithMostError] < avg[dimensionWithMostError])
				{
					split1.push_back(c);
				}
				else
				{
					split2.push_back(c);
				}
			}

			boxes[indexOfMostError].arr = split1;
			boxes[indexOfMostError].beenSet = false;
			
			boxes.push_back( BoxInfo() );
			boxes.back().arr = split2;
			boxes.back().beenSet = false;

			//after split, you have 2 boxes. repeat algorithm on all boxes.
			//Split on the box with the most error which is the sum of all of the boxes x,y,z error
			//Insures that you end up with the amount of boxes you need to get the appropriate amount of clusters
			//Average the clusters on each box to get the final paletteArr.
		}

		
		std::vector< std::vector<GeneralVector>> finalGroups = std::vector<std::vector<GeneralVector>>();
		if(meansOnly)
		{
			for(BoxInfo b : boxes)
			{
				//push averages as groups
				finalGroups.push_back( {b.averageVal} );
			}
		}
		else
		{
			for(BoxInfo b : boxes)
			{
				//push groups
				finalGroups.push_back( b.arr );
			}
		}

		return finalGroups;
	}

	std::vector<std::vector<GeneralVector>> MathExt::medianCut(std::vector<GeneralVector> arr, int clusters, bool meansOnly)
	{
		int dimensions = arr[0].getSize();
		std::vector<int> endPos = std::vector<int>();
		std::vector<GeneralVector> sortArray = std::vector<GeneralVector>(arr);

		endPos.push_back(arr.size());

		while(endPos.size() < (size_t)clusters)
		{
			size_t currentSize = endPos.size();
			for(size_t i=0; i<currentSize; i++)
			{
				int start = 0;
				int end = endPos[i];
				if(i!=0)
				{
					start = endPos[i-1];
				}

				//sort by most range
				
				GeneralVector minVals = sortArray[start];
				GeneralVector maxVals = sortArray[start];
				GeneralVector ranges = GeneralVector(dimensions);
				int dimensionToSortBy = 0;
				
				for(int i=start; i<end; i++)
				{
					for(int k=0; k<dimensions; k++)
					{
						if(sortArray[i][k] < minVals[k])
						{
							minVals[k] = sortArray[i][k];
						}
						if(sortArray[i][k] > maxVals[k])
						{
							maxVals[k] = sortArray[i][k];
						}
					}
				}

				ranges[0] = maxVals[0] - minVals[0];
				float maxRange = ranges[0];
				for(int k=1; k<dimensions; k++)
				{
					ranges[k] = maxVals[k] - minVals[k];
					if(ranges[k] > maxRange)
					{
						maxRange = ranges[k];
						dimensionToSortBy = k;
					}
				}

				Sort::mergeSort<GeneralVector>(sortArray.data()+start, end-start, [dimensionToSortBy](GeneralVector a, GeneralVector b) -> bool{
					return a[dimensionToSortBy] < b[dimensionToSortBy];
				});
			}

			std::vector<int> newEndPos = std::vector<int>();

			newEndPos.push_back(endPos[0]/2);
			newEndPos.push_back(endPos[0]);

			for(size_t i=1; i<currentSize; i++)
			{
				int midIndex = (endPos[i-1] + endPos[i])/2;
				
				newEndPos.push_back(midIndex);
				newEndPos.push_back(endPos[i]);
			}

			endPos = newEndPos;
		}


		//average out each sections for the final set of clusters
		std::vector<std::vector<GeneralVector>> finalclusters = std::vector<std::vector<GeneralVector>>();

		if(meansOnly)
		{
			for(size_t i=0; i<endPos.size(); i++)
			{
				int start = 0;
				int end = endPos[i];
				if(i!=0)
				{
					start = endPos[i-1];
				}

				GeneralVector avgVal = GeneralVector(dimensions);
				int divVal = end-start;
				for(int k=start; k<end; k++)
				{
					GeneralVector c = sortArray[k];
					avgVal += c;
				}

				avgVal/=divVal;


				finalclusters.push_back( {avgVal} );
			}
		}
		else
		{
			for(size_t i=0; i<endPos.size(); i++)
			{
				int start = 0;
				int end = endPos[i];
				if(i!=0)
				{
					start = endPos[i-1];
				}

				std::vector<GeneralVector> group = std::vector<GeneralVector>();
				for(int k=start; k<end; k++)
				{
					group.push_back(sortArray[k]);
				}

				finalclusters.push_back( group );
			}
		}

		return finalclusters;
	}

	std::vector<std::vector<GeneralVector>> MathExt::kMeans(std::vector<GeneralVector> arr, int clusters, int maxIterations, bool meansOnly)
	{
		int dimensions = arr[0].getSize();
		struct BoxInfo
		{
			std::vector<GeneralVector> clusters = std::vector<GeneralVector>();
			GeneralVector averageVal = GeneralVector();
		};

		//pick k means
		std::vector< BoxInfo > groups = std::vector< BoxInfo >();
		
		//pick randomly
		unsigned int currentAmount = 0;
		LCG lcg = LCG( (unsigned int)System::getCurrentTimeNano(), 12354, 0, arr.size());

		for(int i=0; i<clusters; i++)
		{
			int v = lcg.get();
			BoxInfo b = BoxInfo();
			b.averageVal = arr[v];

			groups.push_back( b );
		}

		//do k-means
		
		for(int i=0; i<maxIterations; i++)
		{
			//clear all groups
			for(size_t k=0; k<groups.size(); k++)
			{
				groups[k].clusters.clear();
			}

			//group into k groups
			for(size_t k=0; k<arr.size(); k++)
			{
				int minIndex = -1;
				float minDis = std::numeric_limits<float>::max();

				//measure distance from all means
				for(size_t j=0; j<groups.size(); j++)
				{
					float thisDis = 0;
					GeneralVector lengthVec = GeneralVector(dimensions);
					lengthVec = groups[j].averageVal - arr[k];

					thisDis = MathExt::vecLength(lengthVec);

					if(thisDis < minDis)
					{
						minIndex = j;
						minDis = thisDis;
					}
				}

				if(minIndex >= 0)
				{
					groups[minIndex].clusters.push_back( arr[k] );
				}
			}

			//recompute average
			bool same = true;
			for(size_t j=0; j<groups.size(); j++)
			{
				GeneralVector avg = GeneralVector(dimensions);
				float divVal = 1.0 / groups[j].clusters.size();

				for(GeneralVector c : groups[j].clusters)
				{
					avg += c * divVal;
				}

				if(groups[j].averageVal != avg)
				{
					same = false;
				}

				groups[j].averageVal = avg;
			}

			if(same)
			{
				break;
			}
		}

		std::vector<std::vector<GeneralVector>> finalGroups = std::vector<std::vector<GeneralVector>>();

		if(meansOnly)
		{
			for(BoxInfo k : groups)
			{
				finalGroups.push_back( {k.averageVal} );
			}
		}
		else
		{
			for(BoxInfo k : groups)
			{
				finalGroups.push_back( k.clusters );
			}
		}

		return finalGroups;
	}

	#pragma endregion

} //NAMESPACE glib END