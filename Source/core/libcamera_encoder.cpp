//
// Created by Daniel Kowalski on 25/01/2023.
//

#include "libcamera_encoder.hpp"

void LibcameraEncoder::transformByCurve(void *mem, uint size)
{
	unsigned char* ptr = (unsigned char*)(mem);

	for (uint i = 0; i < size; i++)
	{
		ptr[i] = ptr[i] >> 1;
	}
}
#if defined(USE_USHORT)
void LibcameraEncoder::transform(void *mem, const ushort transformArray[256 * 256], uint size)
{
	memcpy(transformed, mem, size);

	uint length = size / 2;
	for (uint i = 0; i < length; i++)
	{
		transformed[i] = transformArray[transformed[i]];
	}

	memcpy(mem, transformed, size);
}

#else
void LibcameraEncoder::transform(void *mem, const uchar transformArray[256], uint size)
{
	memcpy(transformed, mem, size);

	for (uint i = 0; i < size; i++)
	{
		transformed[i] = transformArray[transformed[i]];
	}

	memcpy(mem, transformed, size);
}
#endif