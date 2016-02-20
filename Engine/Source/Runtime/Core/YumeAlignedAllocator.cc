///////////////////////////////////////////////////////////////////////////////////
/// Yume Engine MIT License (MIT)

/// Copyright (c) 2015 Alperen Gezer

/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
/// 
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
/// 
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
/// 
/// File : YumeMemoryAllocatorNedPooling.h
/// Date : 8.28.2015
/// Comments : 
///
///////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------
#include "YumeHeaders.h"
#include "YumeAlignedAllocator.h"
#include "YumeBitPattern.h"

namespace YumeEngine
{
	//---------------------------------------------------------------------
	void* AlignedMemory::allocate(size_t size,size_t alignment)
	{
		assert(0 < alignment && alignment <= 128 && Bitwise::isPO2(alignment));

		unsigned char* p = new unsigned char[size + alignment];
		size_t offset = alignment - (size_t(p) & (alignment-1));

		unsigned char* result = p + offset;
		result[-1] = (unsigned char)offset;

		return result;
	}
	//---------------------------------------------------------------------
	void* AlignedMemory::allocate(size_t size)
	{
		return allocate(size,YUME_SIMD_ALIGNMENT);
	}
	//---------------------------------------------------------------------
	void AlignedMemory::deallocate(void* p)
	{
		if(p)
		{
			unsigned char* mem = (unsigned char*)p;
			mem = mem - mem[-1];
			delete[] mem;
		}
	}
}
