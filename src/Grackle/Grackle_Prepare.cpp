#include "Copyright.h"
#include "GAMER.h"

#ifdef SUPPORT_GRACKLE




//-------------------------------------------------------------------------------------------------------
// Function    :  Grackle_Prepare
// Description :  Fill up the input host array "h_Che_Array" for the CPU/GPU Grackle solver
//
// Note        :  1. Prepare CHE_NPREP variables
//                   --> CHE_NPREP = 3 currently
//                   --> [mass density, specific internal energy, kinematic energy density]
//                2. This function always prepares the latest FluSg data
//
// Parameter   :  lv          : Target refinement level
//                h_Che_Array : Host array to store the prepared data
//                NPG         : Number of patch groups prepared at a time
//                PID0_List   : List recording the patch indicies with LocalID==0 to be udpated
//-------------------------------------------------------------------------------------------------------
void Grackle_Prepare( const int lv, real h_Che_Array[][CHE_NPREP][ CUBE(PS1) ], const int NPG, const int *PID0_List )
{

   const int  Idx_Dens        = 0;
   const int  Idx_sEint       = 1;
   const int  Idx_Ek          = 2;
   const real dh              = (real)amr->dh[lv];
#  ifdef DUAL_ENERGY
   const real  Gamma_m1       = GAMMA - (real)1.0;
   const real _Gamma_m1       = (real)1.0 / Gamma_m1;
   const bool CheckMinPres_No = false;
#  endif

   int  N, PID, PID0;
   real Dens, Px, Py, Pz, Etot, _Dens, Ek, sEint;

#  pragma omp parallel for private( N, PID, PID0, Dens, Px, Py, Pz, Etot, _Dens, Ek, sEint ) schedule( runtime )
   for (int TID=0; TID<NPG; TID++)
   {
      PID0 = PID0_List[TID];

      for (int LocalID=0; LocalID<8; LocalID++)
      {
         PID = PID0 + LocalID;
         N   = 8*TID + LocalID;

         for (int t=0; t<CUBE(PS1); t++)
         {
            Dens  = *( amr->patch[ amr->FluSg[lv] ][lv][PID]->fluid[DENS][0][0] + t );
            Px    = *( amr->patch[ amr->FluSg[lv] ][lv][PID]->fluid[MOMX][0][0] + t );
            Py    = *( amr->patch[ amr->FluSg[lv] ][lv][PID]->fluid[MOMY][0][0] + t );
            Pz    = *( amr->patch[ amr->FluSg[lv] ][lv][PID]->fluid[MOMZ][0][0] + t );
            Etot  = *( amr->patch[ amr->FluSg[lv] ][lv][PID]->fluid[ENGY][0][0] + t );
            _Dens = (real)1.0 / Dens;
            Ek    = (real)0.5*( SQR(Px) + SQR(Py) + SQR(Pz) )*_Dens;

//          use the dual-energy variable to calculate the internal energy if applicable
#           ifdef DUAL_ENERGY

#           if   ( DUAL_ENERGY == DE_ENPY )
            sEint = CPU_DensEntropy2Pres( Dens, *( amr->patch[ amr->FluSg[lv] ][lv][PID]->fluid[ENPY][0][0] + t ), Gamma_m1,
                                          CheckMinPres_No, NULL_REAL )*_Dens*_Gamma_m1;
#           elif ( DUAL_ENERGY == DE_EINT )
#           error : DE_EINT is NOT supported yet !!
#           endif

#           else
            sEint = ( Etot - Ek )*_Dens;
#           endif // #ifdef DUAL_ENERGY ... else

            h_Che_Array[N][Idx_Dens ][t] = Dens;
            h_Che_Array[N][Idx_sEint][t] = sEint;
            h_Che_Array[N][Idx_Ek   ][t] = Ek;
         }

         if ( GRACKLE_MODE == GRACKLE_MODE_ORI )
         {
            Che_FieldData[N].density         = h_Che_Array[N][Idx_Dens ];
            Che_FieldData[N].internal_energy = h_Che_Array[N][Idx_sEint];
            Che_FieldData[N].grid_dx         = dh;
         }
      } // for (int LocalID=0; LocalID<8; LocalID++)
   } // for (int TID=0; TID<NPG; TID++)

} // FUNCTION : Grackle_Prepare



#endif // #ifdef SUPPORT_GRACKLE