/*
 * ObjectValueCalculation.h
 *
 *  Created on: Oct 19, 2015
 *      Author: u99319
 */

void InitialCheckDelayOK(FullSol* InitialFullSolution, CommonList* CurCommonList, float LinksDelay){
	int Obj = 0;

	for(int a=0; a<InitialFullSolution->VectorOfPartialSols.size(); a++){

		float delay = 0;
		float Deadline = CurCommonList->PolicyList[a].Deadline;
		int PolicyID = InitialFullSolution->VectorOfPartialSols[a].PolicyID;

		float PathDelay = 0;

		if(InitialFullSolution->VectorOfPartialSols[a].HaveResources == true){
			for(int b=0; b<InitialFullSolution->VectorOfPartialSols[a].VMsForAPolicy.size(); b++){
				NFAllocation NF = InitialFullSolution->VectorOfPartialSols[a].VMsForAPolicy[b];
				int NFID = NF.NFID;

				if(NF.path.Path.size() > 1){
					PathDelay = ((NF.path.Path.size()-1) * LinksDelay);
					delay = delay + PathDelay;
				}

				float NumberOfPackets = 0; // calculate NumberOfPackets in the traffic, assuming BW is in Mbpb and Packet size 744 Bytes
				float TrafficInBytes = 0;
				float DelayOfPackets = 0;

				NFType MyNF = CurCommonList->NFInfo[NFID];
				int TotalCap = MyNF.cpu;
				TrafficInBytes = (TotalCap * 1000000) / 8;
				NumberOfPackets = TrafficInBytes / 1500;
				DelayOfPackets = (1/NumberOfPackets);
				delay = delay + DelayOfPackets;
			}

			if (Deadline > delay){
				InitialFullSolution->VectorOfPartialSols[a].accepted = true;
				InitialFullSolution->VectorOfPartialSols[a].Delay = delay ;
				InitialFullSolution->Accepted.insert(std::pair<short int,PartialSol>(PolicyID,InitialFullSolution->VectorOfPartialSols[a]));
			}
			else{
				RemovePolicy(InitialFullSolution, CurCommonList, PolicyID);
				PartialSol EmptyVMMap;
				EmptyVMMap.PolicyID = PolicyID;
				EmptyVMMap.Splits = 0;
				EmptyVMMap.accepted = false;
				EmptyVMMap.Delay = 0;
				EmptyVMMap.HaveResources = false;
				InitialFullSolution->VectorOfPartialSols[a] = EmptyVMMap;
				InitialFullSolution->NotAccepted.insert(std::pair<short int,PartialSol>(PolicyID,InitialFullSolution->VectorOfPartialSols[a]));
			}
		}
		else{
			InitialFullSolution->NotAccepted.insert(std::pair<short int,PartialSol>(PolicyID,InitialFullSolution->VectorOfPartialSols[a]));
		}
	}

}

ReturnDelay LocalSearchDelayOK(PartialSol* SelectedPolicyTemp, CommonList* CurCommonList, float LinksDelay){

	ReturnDelay RObj;
	RObj.DeadlineOK = false;
	RObj.delay = 0;

	int PolicyChanged = SelectedPolicyTemp->PolicyID;

	float delay = 0;
	float Deadline = CurCommonList->PolicyList[PolicyChanged].Deadline;

	float PathDelay = 0;

	for(int b=0; b<SelectedPolicyTemp->VMsForAPolicy.size(); b++){
		NFAllocation NF = SelectedPolicyTemp->VMsForAPolicy[b];	
		int NFID = NF.NFID;

		PathDelay = ((NF.path.Path.size()-1) * LinksDelay);
		delay = delay + PathDelay;
		
		float NumberOfPackets = 0; // calculate NumberOfPackets in the traffic, assuming BW is in Mbpb and Packet size 744 Bytes
		float TrafficInBytes = 0;
		float DelayOfPackets = 0;

		NFType MyNF = CurCommonList->NFInfo[NFID];
		int TotalCap = MyNF.cpu;
		TrafficInBytes = (TotalCap * 1000000) / 8;
		NumberOfPackets = TrafficInBytes / 1500;
		DelayOfPackets = (1/NumberOfPackets);
		delay = delay + DelayOfPackets;
	}

	if (Deadline >= delay){
		RObj.DeadlineOK = true;
		RObj.delay = delay;
	}

	return RObj;
}

void GlobalObjectValueCalculation(FullSol* InitialFullSolution, CommonList* CurCommonList){
	int Obj = 0;

	for (auto local_it = InitialFullSolution->Accepted.begin(); local_it!= InitialFullSolution->Accepted.end(); ++local_it){
		int PolicyID = local_it->first;
		int BW = CurCommonList->PolicyList[PolicyID].OriginalBW;

		for(int b=0; b<local_it->second.VMsForAPolicy.size(); b++){
			NFAllocation NF = local_it->second.VMsForAPolicy[b];
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

	int ServerCapLeft = 0;

	for (std::map<int,short int>::iterator itlow=  InitialFullSolution->CurLists.ServerCapMap.begin(); itlow!=InitialFullSolution->CurLists.ServerCapMap.end(); ++itlow){
		int FoundCap = itlow->first;
		auto it2 = InitialFullSolution->CurLists.ServerCapHashTable.find(FoundCap);

		if(FoundCap > 0){
			for (std::set<int>::iterator itv = it2->second.ServerIndex.begin(); itv != it2->second.ServerIndex.end(); ++itv){
				ServerCapLeft = ServerCapLeft + FoundCap;
			}
		}
	}

	InitialFullSolution->CPULeft = ServerCapLeft;
	InitialFullSolution->ObjVal = Obj;
}


void GlobalObjectValueUpdateForIncrease(FullSol* InitialFullSolution, int PolicyID, CommonList* CurCommonList){

	int Obj = 0;
	int BW = CurCommonList->PolicyList[PolicyID].OriginalBW;

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

void GlobalObjectValueUpdateForDecrease(FullSol* InitialFullSolution, int PolicyID, CommonList* CurCommonList){

	int Obj = 0;
	int BW = CurCommonList->PolicyList[PolicyID].OriginalBW;

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