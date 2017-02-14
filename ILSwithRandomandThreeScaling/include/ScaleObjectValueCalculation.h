/*
 * ObjectValueCalculation.h
 *
 *  Created on: Oct 19, 2015
 *      Author: u99319
 */

void ScaleGlobalObjectValueCalculation(FullSol* InitialFullSolution, CommonList* CurCommonList){
	int Obj = 0;

	for (int r=0; r<InitialFullSolution->VectorOfPartialSols.size(); r++){
		int BW = InitialFullSolution->VectorOfPartialSols[r].AllocatedBW;

		for(int b=0; b<InitialFullSolution->VectorOfPartialSols[r].VMsForAPolicy.size(); b++){
			NFAllocation NF = InitialFullSolution->VectorOfPartialSols[r].VMsForAPolicy[b];
			short int NFID = NF.NFID;
			NFType NFTypeCur = CurCommonList->NFInfo[NFID];
			int NFBasicCap = NFTypeCur.cpu;

			int NFCapacity = (BW / NFBasicCap);
			if( BW % NFBasicCap != 0){
				NFCapacity++;
			}

			Obj = Obj + NFCapacity;

			BW = BW * NFTypeCur.scale;
		}
	}

	int k = Obj;
}


void ScaleObjectValueUpdateForDecrease(FullSol* InitialFullSolution, int PolicyID, CommonList* CurCommonList, int BW){

	int Obj = 0;

	for(int b=0; b<CurCommonList->PolicyList[PolicyID].NFIDs.size(); b++){
		short int NFID = CurCommonList->PolicyList[PolicyID].NFIDs[b];
		NFType NFTypeCur = CurCommonList->NFInfo[NFID];
		int NFBasicCap = NFTypeCur.cpu;

		int NFCapacity = (BW / NFBasicCap);
		if( BW % NFBasicCap != 0){
			NFCapacity++;
		}

		Obj = Obj + NFCapacity;

		BW = BW * NFTypeCur.scale;
	}

	InitialFullSolution->CPULeft = InitialFullSolution->CPULeft + Obj;
	InitialFullSolution->ObjVal = InitialFullSolution->ObjVal - Obj;
}

void ScaleObjectValueUpdateForIncrease(FullSol* InitialFullSolution, int PolicyID, CommonList* CurCommonList, int BW){

	int Obj = 0;

	for(int b=0; b<CurCommonList->PolicyList[PolicyID].NFIDs.size(); b++){
		short int NFID = CurCommonList->PolicyList[PolicyID].NFIDs[b];
		NFType NFTypeCur = CurCommonList->NFInfo[NFID];
		int NFBasicCap = NFTypeCur.cpu;

		int NFCapacity = (BW / NFBasicCap);
		if( BW % NFBasicCap != 0){
			NFCapacity++;
		}

		Obj = Obj + NFCapacity;

		BW = BW * NFTypeCur.scale;
	}

	InitialFullSolution->CPULeft = InitialFullSolution->CPULeft - Obj;
	InitialFullSolution->ObjVal = InitialFullSolution->ObjVal + Obj;
}