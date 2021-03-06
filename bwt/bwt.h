/**
 * BWT Algorithm (header file)
 * 2019-02-02
 */

#ifndef __BWT_H
#define __BWT_H

namespace NXZIP
{
	class BWT
	{
	public:
		uint8_t* cstr;			/* !< The Result Array in C Style */
		uint32_t index;			/* !< The position of flag character */
		uint32_t length;		/* !< The Length of Result Array */

		BWT(uint32_t size);
		~BWT(void);
	};

	bool NXZ_BWTransform2(uint8_t* srcArray, uint32_t length, BWT* bwt);
	bool NXZ_BWTransform_Inverse2(uint8_t* srcArray, uint32_t length, uint32_t index, uint8_t* dstArray);
}

#endif /*__BWT_H*/

//EOF
