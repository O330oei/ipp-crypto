/*******************************************************************************
* Copyright 2016-2018 Intel Corporation
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
//        ippsHMACInit_rmf()
//
*/

#include "owndefs.h"
#include "owncp.h"
#include "pcphmac.h"
#include "pcphmac_rmf.h"
#include "pcptool.h"

/*F*
//    Name: ippsHMACInit_rmf
//
// Purpose: Init HMAC state.
//
// Returns:                Reason:
//    ippStsNullPtrErr           pKey == NULL
//                               pState == NULL
//    ippStsLengthErr            keyLen <0
//    ippStsNoErr                no errors
//
// Parameters:
//    pKey        pointer to the secret key
//    keyLen      length (bytes) of the secret key
//    pState      pointer to the HMAC state
//    pMethod     hash method
//
*F*/
IPPFUN(IppStatus, ippsHMACInit_rmf,(const Ipp8u* pKey, int keyLen,
                                    IppsHMACState_rmf* pCtx,
                                    const IppsHashMethod* pMethod))
{
   /* test pointer */
   IPP_BAD_PTR2_RET(pCtx, pMethod);
   pCtx = (IppsHMACState_rmf*)( IPP_ALIGNED_PTR(pCtx, HASH_ALIGNMENT) );

   /* test key pointer and key length */
   IPP_BAD_PTR1_RET(pKey);
   IPP_BADARG_RET(0>keyLen, ippStsLengthErr);

   /* set state ID */
   HMAC_CTX_ID(pCtx) = idCtxHMAC;

   /* init hash context */
   ippsHashInit_rmf(&HASH_CTX(pCtx), pMethod);

   {
      int n;

      /* hash specific */
      IppsHashState_rmf* pHashCtx = &HASH_CTX(pCtx);
      int mbs = pMethod->msgBlkSize;
      int hashSize = pMethod->hashLen;

      /* copyMask = keyLen>mbs? 0xFF : 0x00 */
      int copyMask = (mbs-keyLen) >>(BITSIZE(int)-1);

      /* actualKeyLen = keyLen>mbs? hashSize:keyLen */
      int actualKeyLen = (hashSize & copyMask) | (keyLen & ~copyMask);

      /* compute hash(key, keyLen) just in case */
      ippsHashUpdate_rmf(pKey, keyLen, pHashCtx);
      ippsHashFinal_rmf(HASH_BUFF(pHashCtx), pHashCtx);

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
      ippsHashUpdate_rmf(pCtx->ipadKey, mbs, pHashCtx);

      return ippStsNoErr;
   }
}
