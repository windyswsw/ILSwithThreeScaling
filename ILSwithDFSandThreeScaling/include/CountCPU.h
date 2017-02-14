/*
 * ObjectValueCalculation.h
 *
 *  Created on: Oct 19, 2015
 *      Author: u99319
 */


int NextBW(FullSol* InitialFullSolution, int PolicyID, CommonList* CurCommonList, int BW){

	int maxBW = 0;

	for(int b=0; b<CurCommonList->PolicyList[PolicyID].NFIDs.size(); b++){
		short int NFID = CurCommonList->PolicyList[PolicyID].NFIDs[b];
		NFType NFTypeCur = CurCommonList->NFInfo[NFID];
		int NFBasicCap = NFTypeCur.cpu;

		int NFCapacity = (BW / NFBasicCap);
		if( BW % NFBasicCap != 0){
			NFCapacity++;
		}

		int NFCapacityNeededOneLess = NFCapacity - 1;
		int BWthatCanHandleByNewCapacity = NFCapacityNeededOneLess * NFBasicCap;

		if( maxBW < BWthatCanHandleByNewCapacity){
			maxBW = BWthatCanHandleByNewCapacity;
		}

		BW = BW * NFTypeCur.scale;
	}
	return maxBW;
}

void MaxEnteringBW(CommonList* CurCommonList){

	for(int k=0; k<CurCommonList->PolicyList.size(); k++){

		int GivenBW = CurCommonList->PolicyList[k].OriginalBW;

		short int NFID = CurCommonList->PolicyList[k].NFIDs[0];
		NFType NFTypeCur = CurCommonList->NFInfo[NFID];
		int NFBasicCap = NFTypeCur.cpu;

		int NFCapacity = (GivenBW / NFBasicCap);
		if( GivenBW % NFBasicCap != 0){
			NFCapacity++;
		}

		int BWthatCanHandleByNewCapacity = NFCapacity * NFBasicCap;

		CurCommonList->PolicyList[k].OriginalBW = BWthatCanHandleByNewCapacity;
		CurCommonList->PolicyList[k].CurrentBW = BWthatCanHandleByNewCapacity;
		CurCommonList->PolicyList[k].NextBW = BWthatCanHandleByNewCapacity;
	}
}

void NextMaxEnteringBWForAPolicy(CommonList* CurCommonList, int PolicyID){

	int GivenBW = CurCommonList->PolicyList[PolicyID].NextBW;

	short int NFID = CurCommonList->PolicyList[PolicyID].NFIDs[0];
	NFType NFTypeCur = CurCommonList->NFInfo[NFID];
	int NFBasicCap = NFTypeCur.cpu;

	int NFCapacity = (GivenBW / NFBasicCap);
	if( GivenBW % NFBasicCap != 0){
		NFCapacity++;
	}

	int BWthatCanHandleByNewCapacity = NFCapacity * NFBasicCap;
	CurCommonList->PolicyList[PolicyID].NextBW = BWthatCanHandleByNewCapacity;
}

int ReturnMaxEnteringBWForAPolicy(CommonList* CurCommonList, int PolicyID, int GivenBW){

	short int NFID = CurCommonList->PolicyList[PolicyID].NFIDs[0];
	NFType NFTypeCur = CurCommonList->NFInfo[NFID];
	int NFBasicCap = NFTypeCur.cpu;

	int NFCapacity = (GivenBW / NFBasicCap);
	if( GivenBW % NFBasicCap != 0){
		NFCapacity++;
	}

	int BWthatCanHandleByNewCapacity = NFCapacity * NFBasicCap;
	
	return BWthatCanHandleByNewCapacity;
}

void CountTotalCPUs(FullSol* InitialFullSolution, CommonList* CurCommonList){

	int CPU = 0;

	for(int p=0; p< CurCommonList->PolicyList.size(); p++){
		int BWO = CurCommonList->PolicyList[p].NextBW;
		int BW = ReturnMaxEnteringBWForAPolicy(CurCommonList, p,BWO);
		
		if(CurCommonList->PolicyList[p].Parent == CurCommonList->PolicyList[p].PolicyID){
			for(int b=0; b<CurCommonList->PolicyList[p].NFIDs.size(); b++){
				short int NFID = CurCommonList->PolicyList[p].NFIDs[b];
				NFType NFTypeCur = CurCommonList->NFInfo[NFID];
				int NFBasicCap = NFTypeCur.cpu;

				int NFCapacity = (BW / NFBasicCap);
				if( BW % NFBasicCap != 0){
					NFCapacity++;
				}

				CPU = CPU + NFCapacity;

				BW = BW * NFTypeCur.scale;
			}
		}
	}

	InitialFullSolution->MaxCPUs = CPU;
}
