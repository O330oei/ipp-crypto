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
//     HMAC General Functionality
// 
//  Contents:
//        ippsHMAC_Init()
//
*/

#include "owndefs.h"
#include "owncp.h"
#include "pcphmac.h"
#include "pcptool.h"

/*F*
//    Name: ippsHMAC_Init
//
// Purpose: Init HMAC state.
//
// Returns:                Reason:
//    ippStsNullPtrErr           pKey == NULL
//                               pState == NULL
//    ippStsLengthErr            keyLen <0
//    ippStsNotSupportedModeErr  if algID is not match to supported hash alg
//    ippStsNoErr                no errors
//
// Parameters:
//    pKey        pointer to the secret key
//    keyLen      length (bytes) of the secret key
//    pState      pointer to the HMAC state
//    hashAlg     hash alg ID
//
*F*/
IPPFUN(IppStatus, ippsHMAC_Init,(const Ipp8u* pKey, int keyLen, IppsHMACState* pCtx, IppHashAlgId hashAlg))
{
   //int mbs;

   /* get algorithm id */
   hashAlg = cpValidHashAlg(hashAlg);
   /* test hash alg */
   IPP_BADARG_RET(ippHashAlg_Unknown==hashAlg, ippStsNotSupportedModeErr);
   //mbs = cpHashMBS(hashAlg);

   /* test pState pointer */
   IPP_BAD_PTR1_RET(pCtx);
   pCtx = (IppsHMACState*)( IPP_ALIGNED_PTR(pCtx, HASH_ALIGNMENT) );

   /* test key pointer and key length */
   IPP_BAD_PTR1_RET(pKey);
   IPP_BADARG_RET(0>keyLen, ippStsLengthErr);

   /* set state ID */
   HMAC_CTX_ID(pCtx) = idCtxHMAC;

   /* init hash context */
   ippsHashInit(&HASH_CTX(pCtx), hashAlg);

   {
      int n;

      /* hash specific */
      IppsHashState* pHashCtx = &HASH_CTX(pCtx);
      int mbs = cpHashMBS(hashAlg);
      int hashSize = cpHashSize(hashAlg);

      /* copyMask = keyLen>mbs? 0xFF : 0x00 */
      int copyMask = (mbs-keyLen) >>(BITSIZE(int)-1);

      /* actualKeyLen = keyLen>mbs? hashSize:keyLen */
      int actualKeyLen = (hashSize & copyMask) | (keyLen & ~copyMask);

      /* compute hash(key, keyLen) just in case */
      ippsHashUpdate(pKey, keyLen, pHashCtx);
      ippsHashFinal(HASH_BUFF(pHashCtx), pHashCtx);

      /* copy either key or hash(key) into ipad- and opad- buffers */
      MASKED_COPY_BNU(pCtx->ipadKey, (Ipp8u)copyMask, HASH_BUFF(pHashCtx), pKey, actualKeyLen);
      MASKED_COPY_BNU(pCtx->opadKey, (Ipp8u)copyMask, HASH_BUFF(pHashCtx), pKey, actualKeyLen);

      /* XOR-ing key */
      for(n=0; n<actualKeyLen; n++) {
         pCtx->ipadKey[n] ^= (Ipp8u)IPAD;
         pCtx->opadKey[n] ^= (Ipp8u)OPAD;
      }
      for(; n<mbs; n++) {
         pCtx->ipadKey[n] = (Ipp8u)IPAD;
         pCtx->opadKey[n] = (Ipp8u)OPAD;
      }

      /* ipad key processing */
      ippsHashUpdate(pCtx->ipadKey, mbs, pHashCtx);

      return ippStsNoErr;
   }
}
