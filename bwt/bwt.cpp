/**
 * BWT Algorithm with the End Flag (source file)
 * 2019-02-02
 */

/* !!! DISCARD: STOP DEVELOPING FOLLOWING APIs */

#include <iostream>
#include <vector>
#include <cstring>
#include "bwt.h"

#define ENF -1

/* Expose some Member of STL */
/* for Inverse-BWT */
using std::vector;

/* Non-public Namespace */
namespace 
{
	/* Logic Suffix Array Definition */
	typedef struct 
	{
		int16_t* nnstr;			/* !< the cache pointer */
		uint32_t position;		/* !< the position in origin Array */
		uint32_t length;		/* !< the length of nnstr */
	}SuffixArray_TypeDef;

	/* the cache for origin Array */
	class arrayCache
	{
	public:
		/* Class Data Member */
		int16_t* cache1;		/* !< cache1 to store origin array */
		int16_t* cache2;		/* !< cache2 to store result after suffix sort */
		uint32_t length;		/* !< length of cache(sizeof(origin)+1) */

		/* Constructor and Destructor */
		arrayCache(uint32_t length);
		~arrayCache(void);

		/* Public Method */
		void copy_to_cache1(uint8_t* srcArray);
	};

	/**
	 * @brief	Compare sequences by subscript(be like memcmp)
	 */
	template<typename T> 
	int sequence_cmp(T* s1, T* s2, size_t count)
	{
		const T* seq1 = s1;
		const T* seq2 = s2;

		while(count --> 0u)
		{
			if(*seq1++ != *seq2++)
			{
				return (seq1[-1] < seq2[-1]) ? -1 : 1;
			}
		}
		return 0;
	}
}

/* Non-Public Class arrayCache Functions */

/**
 * @brief	the Constructor of arrayCache
**/
::arrayCache::arrayCache(uint32_t length)
{
	this->cache1 = new int16_t[length + 1];
	this->cache2 = new int16_t[length + 1];

	this->length = length + 1;
}

/**
 * @brief	the Destructor of arrayCache 
**/
::arrayCache::~arrayCache(void)
{
	delete cache1;
	delete cache2;

	length = 0;
}

/* Public Interface in namespace NXZIP */

/**
 * @brief	The Burrows-Wheeler Transform
 * @param 	uint8_t* srcArray: the Array to be transformed
 * @param	BWT* bwt: the result after BWT
 * @return	bool: true means no error occured
 * @note	!!! Length of srcArray must be as same as bwt->length
**/
bool NXZIP::NXZ_BWTransform(uint8_t* srcArray, BWT* bwt)
{
	/* Parameters Check */
	if(nullptr == srcArray || nullptr == bwt || nullptr == bwt->cstr || 0u == bwt->length)
	{
		return false;
	}

	/* Allocate the memory */
	::arrayCache* cache = new ::arrayCache(bwt->length);
	SuffixArray_TypeDef* suf = new SuffixArray_TypeDef[bwt->length + 1u];
	SuffixArray_TypeDef* TMP = new SuffixArray_TypeDef{nullptr, 0u, 0u};
	uint32_t tmplen = 0u;

	/* copy to cache */
	cache->copy_to_cache1(srcArray);

	/* Init the logic suffix array */
	for(uint32_t i = 0; i < cache->length; i++)
	{
		suf[i].nnstr = cache->cache1 + i;
		suf[i].position = i;
		suf[i].length = cache->length - i;
	}

	/* Bubble sort for Logic Suffix Array */
	/* Time Complicity: O(n^3) */
	/* TODO: Bubble sort --> Quick sort */
	/* Time Complexity: O(n^3) */
	for(uint32_t i = 0; i < cache->length - 1u; i++)
	{
		for(uint32_t j = 0; j < cache->length - 1u; j++)
		{
			tmplen = ((suf[j].length < suf[j+1].length) ? suf[j].length : suf[j+1].length);

			if(::sequence_cmp(suf[j].nnstr, suf[j+1].nnstr, tmplen) == 1)
			{
				memcpy((void*)TMP, (void*)(suf+j), sizeof(SuffixArray_TypeDef));
				memcpy((void*)(suf+j), (void*)(suf+j+1), sizeof(SuffixArray_TypeDef));
				memcpy((void*)(suf+j+1), (void*)TMP, sizeof(SuffixArray_TypeDef));
			}
		}
	}

	/* take out the data(with flag character) */
	for(uint32_t i = 0; i < cache->length; i++)
	{
		if((suf+i)->position == 0)
		{
			cache->cache2[i] = *(cache->cache1 + cache->length - 1u);
		}
		else
		{
			cache->cache2[i] = *(suf[i].nnstr - 1);
		}

		/* get the position of flag character */
		if(*(cache->cache2 + i ) == ENF)
		{
			bwt->index = i;
		}
	}

	/* Get the Result */
	/* before index */
	for(uint32_t i = 0; i < bwt->index; i++)
	{
		bwt->cstr[i] = (uint8_t)cache->cache2[i];
	}
	/* after index */
	for(uint32_t i = bwt->index; i < cache->length - 1u; i++)
	{
		bwt->cstr[i] = (uint8_t)cache->cache2[i + 1u];
	}
	
	/* Destroy the Memory */
	delete[] suf;
	delete TMP;
	delete cache;

	return true;
}

/**
 * @brief	Inverse-BWT
 * @param	BWT* ibwt: the BWT Sequence
 * @param	uint8_t* dstArray: Distance Array to receive the origin Array
 * @return	bool: true means no error occured
 * @note	!!! Length of dstArray must be as same as ibwt->length
**/
bool NXZIP::NXZ_BWTransform_Inverse(BWT* ibwt, uint8_t* dstArray)
{
	/* Parameters Check */
	if(nullptr == ibwt || nullptr == ibwt->cstr || nullptr == dstArray || 0u == ibwt->length)
	{
		return false;
	}

	return true;
}

/* Non-Public Method */

/**
 * @brief	copy origin Array to cache(for next operations)(Non-Public)
 * @param	uint8_t* srcArray: Array to copy
 * @return	None
 * @note	None
**/ 
void ::arrayCache::copy_to_cache1(uint8_t* srcArray)
{
	for(uint32_t i = 0; i < this->length - 1u; i++)
	{
		this->cache1[i] = srcArray[i];
	}

	/* insert the flag character */
	this->cache1[this->length - 1u] = ENF;
}

//EOF
