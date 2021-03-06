/*******************************************************************************
* Copyright 2014-2019 Intel Corporation
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
//     Cryptography Primitive.
//     SMS4 ECB encryption/decryption
// 
//  Contents:
//     cpSMS4_SetRoundKeys_aesni()
// 
// 
*/

#include "owndefs.h"
#include "owncp.h"
#include "pcpsms4.h"
#include "pcptool.h"

#if (_IPP>=_IPP_P8) || (_IPP32E>=_IPP32E_Y8)

#include "pcpsms4_y8cn.h"

__INLINE __m128i Ltag(__m128i x)
{
   __m128i T = _mm_slli_epi32(x, 13);
   T = _mm_xor_si128(T, _mm_srli_epi32 (x,19));
   T = _mm_xor_si128(T, _mm_slli_epi32 (x,23));
   T = _mm_xor_si128(T, _mm_srli_epi32 (x, 9));
   return T;
}

/*
// compute round keys
*/

#define cpSMS4_SetRoundKeys_aesni OWNAPI(cpSMS4_SetRoundKeys_aesni)
void cpSMS4_SetRoundKeys_aesni(Ipp32u* pRoundKey, const Ipp8u* pSecretKey)
{
   __ALIGN16 __m128i TMP[5];
   /*
      TMP[0] = T
      TMP[1] = K0
      TMP[2] = K1
      TMP[3] = K2
      TMP[4] = K3
   */
   TMP[1] = _mm_cvtsi32_si128(ENDIANNESS32(((Ipp32u*)pSecretKey)[0]) ^ SMS4_FK[0]);
   TMP[2] = _mm_cvtsi32_si128(ENDIANNESS32(((Ipp32u*)pSecretKey)[1]) ^ SMS4_FK[1]);
   TMP[3] = _mm_cvtsi32_si128(ENDIANNESS32(((Ipp32u*)pSecretKey)[2]) ^ SMS4_FK[2]);
   TMP[4] = _mm_cvtsi32_si128(ENDIANNESS32(((Ipp32u*)pSecretKey)[3]) ^ SMS4_FK[3]);

   const Ipp32u* pCK = SMS4_CK;

   int itr;
   for(itr=0; itr<8; itr++) {
      /* initial xors */
      TMP[0] = _mm_cvtsi32_si128(pCK[0]);
      TMP[0] = _mm_xor_si128(TMP[0], TMP[2]);
      TMP[0] = _mm_xor_si128(TMP[0], TMP[3]);
      TMP[0] = _mm_xor_si128(TMP[0], TMP[4]);
      /* Sbox */
      TMP[0] = sBox(TMP[0]);
      /* Sbox done, now Ltag */
      TMP[1] = _mm_xor_si128(_mm_xor_si128(TMP[1], TMP[0]), Ltag(TMP[0]));
      pRoundKey[0] = _mm_cvtsi128_si32(TMP[1]);

      /* initial xors */
      TMP[0] = _mm_cvtsi32_si128(pCK[1]);
      TMP[0] = _mm_xor_si128(TMP[0], TMP[3]);
      TMP[0] = _mm_xor_si128(TMP[0], TMP[4]);
      TMP[0] = _mm_xor_si128(TMP[0], TMP[1]);
      /* Sbox */
      TMP[0] = sBox(TMP[0]);
      /* Sbox done, now Ltag */
      TMP[2] = _mm_xor_si128(_mm_xor_si128(TMP[2], TMP[0]), Ltag(TMP[0]));
      pRoundKey[1] = _mm_cvtsi128_si32(TMP[2]);

      /* initial xors */
      TMP[0] = _mm_cvtsi32_si128(pCK[2]);
      TMP[0] = _mm_xor_si128(TMP[0], TMP[4]);
      TMP[0] = _mm_xor_si128(TMP[0], TMP[1]);
      TMP[0] = _mm_xor_si128(TMP[0], TMP[2]);
      /* Sbox */
      TMP[0] = sBox(TMP[0]);
      /* Sbox done, now Ltag */
      TMP[3] = _mm_xor_si128(_mm_xor_si128(TMP[3], TMP[0]), Ltag(TMP[0]));
      pRoundKey[2] = _mm_cvtsi128_si32(TMP[3]);

      /* initial xors */
      TMP[0] = _mm_cvtsi32_si128(pCK[3]);
      TMP[0] = _mm_xor_si128(TMP[0], TMP[1]);
      TMP[0] = _mm_xor_si128(TMP[0], TMP[2]);
      TMP[0] = _mm_xor_si128(TMP[0], TMP[3]);
      /* Sbox */
      TMP[0] = sBox(TMP[0]);
      /* Sbox done, now Ltag */
      TMP[4] = _mm_xor_si128(_mm_xor_si128(TMP[4], TMP[0]), Ltag(TMP[0]));
      pRoundKey[3] = _mm_cvtsi128_si32(TMP[4]);

      pCK += 4;
      pRoundKey += 4;
   }

   /* clear secret data */
   for(int i = 0; i < sizeof(TMP)/sizeof(TMP[0]); i++){
      TMP[i] = _mm_xor_si128(TMP[i],TMP[i]);
   }
}

#endif /* _IPP_P8, _IPP32E_Y8 */
