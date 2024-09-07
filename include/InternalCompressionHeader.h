#pragma once

//ONLY FOR INTERNAL USE WITH COMPRESSION FILES

#include "Compression.h"
#include <iostream>
#include "LinkedList.h"
#include "BinarySet.h"
#include "Algorithms.h"
#include "StringTools.h"
#include "MathExt.h"
#include "System.h"
#include "Concurrency.h"
#include "SimpleFile.h"

#include "SimpleHashMap.h"

#ifndef min
	#define min(a,b) (a<b)? a:b
#endif

#ifndef max
	#define max(a,b) (a>b)? a:b
#endif