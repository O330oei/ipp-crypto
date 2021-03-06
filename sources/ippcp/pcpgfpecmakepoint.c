/*******************************************************************************
* Copyright 2010-2019 Intel Corporation
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
//     Intel(R) Integrated Performance Primitives. Cryptography Primitives.
//     EC over GF(p) Operations
//
//     Context:
//        ippsGFpECMakePoint()
//
*/

#include "owndefs.h"
#include "owncp.h"
#include "pcpgfpecstuff.h"
#include "pcphash.h"
#include "pcphash_rmf.h"


/*
   computes for given x- computes y-coodinates of P(x,y) point belonging EC if existing
*/

/*F*
// Name: ippsGFpECMakePoint
//
// Purpose: Constructs the coordinates of a point on an elliptic curve based on the X-coordinate
//
// Returns:                   Reason:
//    ippStsNullPtrErr               pPoint == NULL
//                                   pEC == NULL
//                                   pX == NULL
//
//    ippStsContextMatchErr          invalid pEC->idCtx
//                                   invalid pPoint->idCtx
//                                   invalid pX->idCtx
//
//    ippStsBadArgErr                !GFP_IS_BASIC(GFP_PMA(ECP_GFP(pEC)))
//
//    ippStsOutOfRangeErr            ECP_POINT_FELEN(pPoint)!=GFP_FELEN()
//                                   GFPE_ROOM(pX)!=GFP_FELEN()
//
//    ippStsQuadraticNonResidueErr   square of the Y-coordinate of
//                                   the pPoint is a quadratic non-residue modulo
//
//
//    ippStsNoErr                    no error
//
// Parameters:
//    pPoint      Pointer to the IppsGFpECPoint context
//    pEC         Pointer to the context of the elliptic curve
//    pX          Pointer to the X-coordinate of the point on the elliptic curve
//
// Note:
//    Is not a fact that computed point belongs to BP-related subgroup BP
//
*F*/

IPPFUN(IppStatus, ippsGFpECMakePoint,(const IppsGFpElement* pX, IppsGFpECPoint* pPoint, IppsGFpECState* pEC))
{
   IPP_BAD_PTR3_RET(pX, pPoint, pEC);
   pEC = (IppsGFpECState*)( IPP_ALIGNED_PTR(pEC, ECGFP_ALIGNMENT) );
   IPP_BADARG_RET( !ECP_TEST_ID(pEC), ippStsContextMatchErr );
   IPP_BADARG_RET( !GFP_IS_BASIC(GFP_PMA(ECP_GFP(pEC))), ippStsBadArgErr );
   IPP_BADARG_RET( !GFPE_TEST_ID(pX), ippStsContextMatchErr );
   IPP_BADARG_RET( !ECP_POINT_TEST_ID(pPoint), ippStsContextMatchErr );

   IPP_BADARG_RET( GFPE_ROOM(pX)!=GFP_FELEN(GFP_PMA(ECP_GFP(pEC))), ippStsOutOfRangeErr);
   IPP_BADARG_RET( ECP_POINT_FELEN(pPoint)!=GFP_FELEN(GFP_PMA(ECP_GFP(pEC))), ippStsOutOfRangeErr);

   return gfec_MakePoint(pPoint, GFPE_DATA(pX), pEC)? ippStsNoErr : ippStsQuadraticNonResidueErr;
}
