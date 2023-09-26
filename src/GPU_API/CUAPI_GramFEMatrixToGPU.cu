#include "CUAPI.h"
#include "CUFLU.h"


#if ( defined(GPU) && GRAMFE_SCHEME == GRAMFE_MATMUL )

extern gramfe_matmul_float (*d_Flu_TimeEvo)[2 * FLU_NXT];

//-------------------------------------------------------------------------------------------------------
// Function    :  CUAPI_ELBDM_GramFE_MatrixToGPU
// Description :  Transfer time evolution matrix used in ELBDM GramFE matrix multiplication scheme to GPU 
//                Uses synchronous copy to ensure matrix is on GPU when solver starts 
//
// Note        :  1. Prefix "d" : for pointers pointing to the "Device" memory space
//                   Prefix "h" : for pointers pointing to the "Host"   memory space
//
// Parameter   :  h_GramFE_TimeEvo : Host array storing the GramFE time evolution matrix prepared by ELBDM_GramFE_ComputeTimeEvolutionMatrix
//-------------------------------------------------------------------------------------------------------
void CUAPI_ELBDM_GramFE_MatrixToGPU( gramfe_matmul_float (*h_GramFE_TimeEvo)[2 * FLU_NXT] )
{
   size_t h_FluTimeEvo_MemSize = 2 * FLU_NXT * PS2 * sizeof(gramfe_matmul_float);
   CUDA_CHECK_ERROR( cudaMemcpy( d_Flu_TimeEvo, h_GramFE_TimeEvo, h_FluTimeEvo_MemSize, cudaMemcpyHostToDevice) );
} // FUNCTION : CUAPI_ELBDM_GramFE_MatrixToGPU

#endif // #if ( defined(GPU) && GRAMFE_SCHEME == GRAMFE_MATMUL )