/*******************************************************************************
* Copyright 2017-2019 Intel Corporation
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
// 
//  Purpose:
//     Cryptography Primitive. Modular Arithmetic Engine. General Functionality
// 
//  Contents:
//        alm_mont_inv_ct()
//
*/

#include "owndefs.h"
#include "owncp.h"
#include "pcpbnumisc.h"
#include "pcpbnuarith.h"
#include "gsmodstuff.h"
#include "pcpmask_ct.h"

/*
// almost Montgomery Inverse
//
// returns (k,r), r = (1/a)*(2^k) mod m
//
// (constant-execution-time version)
*/
int alm_mont_inv_ct(BNU_CHUNK_T* pr, const BNU_CHUNK_T* pa, gsModEngine* pME)
{
   const BNU_CHUNK_T* pm = MOD_MODULUS(pME);
   int mLen = MOD_LEN(pME);
   int modulusBitSize = MOD_BITSIZE(pME);

   const int polLength  = 6;
   BNU_CHUNK_T* pBuffer = gsModPoolAlloc(pME, polLength);

   BNU_CHUNK_T* pu = pBuffer;
   BNU_CHUNK_T* ps = pu+mLen;
   BNU_CHUNK_T* pv = ps+mLen;
   BNU_CHUNK_T* pt = pv+mLen;

   BNU_CHUNK_T* px = pt+mLen;
   BNU_CHUNK_T* py = px+mLen;

   int k = 0;
   int i;
   BNU_CHUNK_T ext = 0;

   //tbcd: temporary excluded: assert(NULL!=pBuffer);

   // u=modulus, s=1 and v=a, t=0,
   COPY_BNU(pu, pm, mLen);
   ZEXPAND_BNU(ps, 0, mLen); ps[0] = 1;
   COPY_BNU(pv, pa, mLen);
   ZEXPAND_BNU(pt, 0, mLen);

   for(i=0; i<2*modulusBitSize; i++) {
      /* update mask - update = (v==0)? 0xFF : 0 */
      BNU_CHUNK_T update = ~cpIsGFpElemEquChunk_ct(pv, mLen, 0);
      /* temporary masks */
      BNU_CHUNK_T m, mm;

      /* compute in advance r = s+t */
      cpAdd_BNU(pr, ps, pt, mLen);

      /*
      // update or keep current u, s, v, t
      */

      /* if(isEven(u)) { u=u/2; s=2*s; } 1-st branch */
      m = update & cpIsEven_ct(pu[0]);
      cpLSR_BNU(px, pu, mLen, 1);
      cpAdd_BNU(py, ps, ps, mLen);
      cpMaskedReplace_ct(pu, px, mLen, m);
      cpMaskedReplace_ct(ps, py, mLen, m);

      /* else if(isEven(v)) { v=v/2; t=2*t; } 2-nd branch */
      mm = update & ~m & cpIsEven_ct(pv[0]);
      cpLSR_BNU(px, pv, mLen, 1);
      cpAdd_BNU(py, pt, pt, mLen);
      cpMaskedReplace_ct(pv, px, mLen, mm);
      cpMaskedReplace_ct(pt, py, mLen, mm);

      m |= mm; /* if fall in the 1-st of 2-nf branches m=FF.. else m=0 */

      /* else if(v>=u) { v=(v-u)/2; s=s+t; t=2*t;} 3-st branch */
      mm = cpSub_BNU(px, pv, pu, mLen);
      mm = cpIsZero_ct(mm);
      mm = update & ~m & mm;
      cpLSR_BNU(px, px, mLen, 1);
      ext += cpAdd_BNU(py, pt, pt, mLen) & mm;
      cpMaskedReplace_ct(pv, px, mLen, mm);
      cpMaskedReplace_ct(ps, pr, mLen, mm);
      cpMaskedReplace_ct(pt, py, mLen, mm);

      /* else { u=(u-v)/2; t= t+s; s=2*s; } 4-rd branch*/
      cpSub_BNU(px, pu, pv, mLen);
      mm = update & ~m & ~mm;
      cpLSR_BNU(px, px, mLen, 1);
      cpAdd_BNU(py, ps, ps, mLen);
      cpMaskedReplace_ct(pu, px, mLen, mm);
      cpMaskedReplace_ct(pt, pr, mLen, mm);
      cpMaskedReplace_ct(ps, py, mLen, mm);

      /* update or keep current k */
      k = ((k+1) & update) | (k & ~update);
   }

   /*
   // r = (t>mod)? t-mod : t;
   // r = mod - t;
   */
   ext -= cpSub_BNU(pr, pt, pm, mLen);
   cpMaskedReplace_ct(pr, pt, mLen, ~cpIsZero_ct(ext));
   cpSub_BNU(pr, pm, pr, mLen);

   /* test if inversion not found (k=0) */
   k &= cpIsGFpElemEquChunk_ct(pu, mLen, 1);

   gsModPoolFree(pME, polLength);
   return k;
}
