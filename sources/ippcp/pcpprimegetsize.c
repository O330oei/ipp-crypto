/*******************************************************************************
* Copyright 2004-2019 Intel Corporation
* All Rights Reserved.
*
* If this  software was obtained  under the  Intel Simplified  Software License,
* the following terms apply:
*
* The source code,  information  and material  ("Material") contained  herein is
* owned by Intel Corporation or its  suppliers or licensors,  and  title to such
* Material remains with Intel  Corporation or its  suppliers or  licensors.  The
* Material  contains  proprietary  information  of  Intel or  its suppliers  and
* licensors.  The Material is protected by  worldwide copyright  laws and treaty
* provisions.  No part  of  the  Material   may  be  used,  copied,  reproduced,
* modified, published,  uploaded, posted, transmitted,  distributed or disclosed
* in any way without Intel's prior express written permission.  No license under
* any patent,  copyright or other  intellectual property rights  in the Material
* is granted to  or  conferred  upon  you,  either   expressly,  by implication,
* inducement,  estoppel  or  otherwise.  Any  license   under such  intellectual
* property rights must be express and approved by Intel in writing.
*
* Unless otherwise agreed by Intel in writing,  you may not remove or alter this
* notice or  any  other  notice   embedded  in  Materials  by  Intel  or Intel's
* suppliers or licensors in any way.
*
*
* If this  software  was obtained  under the  Apache License,  Version  2.0 (the
* "License"), the following terms apply:
*
* You may  not use this  file except  in compliance  with  the License.  You may
* obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
*
*
* Unless  required  by   applicable  law  or  agreed  to  in  writing,  software
* distributed under the License  is distributed  on an  "AS IS"  BASIS,  WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
* See the   License  for the   specific  language   governing   permissions  and
* limitations under the License.
*******************************************************************************/

/* 
//  Purpose:
//     Intel(R) Integrated Performance Primitives. Cryptographic Primitives (ippcp)
//     Prime Number Primitives.
// 
//  Contents:
//        ippsPrimeGetSize()
//
*/

#include "owncp.h"
#include "pcpprimeg.h"
#include "pcptool.h"

/*F*
// Name: ippsPrimeGetSize
//
// Purpose: Returns size of Prime Number Generator context (bytes).
//
// Returns:                   Reason:
//    ippStsNullPtrErr           NULL == pSize
//    ippStsLengthErr            1 > nMaxBits
//    ippStsNoErr                no error
//
// Parameters:
//    nMaxBits max length of a prime number
//    pSize    pointer to the size of internal context
*F*/
IPPFUN(IppStatus, ippsPrimeGetSize, (int nMaxBits, int* pSize))
{
   IPP_BAD_PTR1_RET(pSize);
   IPP_BADARG_RET(nMaxBits<1, ippStsLengthErr);

   {
      cpSize len = BITS_BNU_CHUNK(nMaxBits);
      cpSize montSize;

      gsModEngineGetSize(nMaxBits, MONT_DEFAULT_POOL_LENGTH, &montSize);

      *pSize = sizeof(IppsPrimeState)
              +len*sizeof(BNU_CHUNK_T)
              +len*sizeof(BNU_CHUNK_T)
              +len*sizeof(BNU_CHUNK_T)
              +len*sizeof(BNU_CHUNK_T)
              +montSize
              +PRIME_ALIGNMENT-1;

      return ippStsNoErr;
   }
}
