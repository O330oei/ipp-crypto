/*******************************************************************************
* Copyright 2019 Intel Corporation
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
//  Purpose: MB RSA.
//
*/

#ifdef IFMA_IPPCP_BUILD
#include "owndefs.h"
#endif /* IFMA_IPPCP_BUILD */

#if !defined(IFMA_IPPCP_BUILD) || (_IPP32E>=_IPP32E_K0)

#ifndef IFMA_METHODS_H
#define IFMA_METHODS_H

//#include "ifma_internal_method.h"

typedef struct _ifma_rsa_method_rsa ifma_RSA_Method;

/*
// cp methods
*/

/* rsa public key opertaion */
EXTERN_C const ifma_RSA_Method* ifma_cp_RSA1K_pub65537_Method(void);
EXTERN_C const ifma_RSA_Method* ifma_cp_RSA2K_pub65537_Method(void);
EXTERN_C const ifma_RSA_Method* ifma_cp_RSA3K_pub65537_Method(void);
EXTERN_C const ifma_RSA_Method* ifma_cp_RSA4K_pub65537_Method(void);
EXTERN_C const ifma_RSA_Method* ifma_cp_RSA_pub65537_Method(int rsaBitsize);

/* rsa private key opertaion */
EXTERN_C const ifma_RSA_Method* ifma_cp_RSA1K_private_Method(void);
EXTERN_C const ifma_RSA_Method* ifma_cp_RSA2K_private_Method(void);
EXTERN_C const ifma_RSA_Method* ifma_cp_RSA3K_private_Method(void);
EXTERN_C const ifma_RSA_Method* ifma_cp_RSA4K_private_Method(void);
EXTERN_C const ifma_RSA_Method* ifma_cp_RSA_private_Method(int rsaBitsize);

/* rsa private key opertaion (ctr) */
EXTERN_C const ifma_RSA_Method* ifma_cp_RSA1K_private_ctr_Method(void);
EXTERN_C const ifma_RSA_Method* ifma_cp_RSA2K_private_ctr_Method(void);
EXTERN_C const ifma_RSA_Method* ifma_cp_RSA3K_private_ctr_Method(void);
EXTERN_C const ifma_RSA_Method* ifma_cp_RSA4K_private_ctr_Method(void);
EXTERN_C const ifma_RSA_Method* ifma_cp_RSA_private_ctr_Method(int rsaBitsize);

EXTERN_C int ifma_RSA_Method_BufSize(const ifma_RSA_Method* m);

#endif /* IFMA_METHODS_H */
#endif
