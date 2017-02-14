void RemovePolicy(FullSol* FullSolution, CommonList* CurCommonList, int policyIndex){

	int BW = CurCommonList->PolicyList[policyIndex].CurrentBW;
	short int NoOfNFs = CurCommonList->PolicyList[policyIndex].NFIDs.size();

	for(int loop=0; loop<NoOfNFs; loop++){
		NFAllocation NF = FullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy[loop];
		int NFSvr = NF.ServerID;	
		short int NFID = NF.NFID;
		SinglePath CurPath = NF.path;

		NFType MyNF = CurCommonList->NFInfo[NFID];
		int NFBasicCap = MyNF.cpu;
		int NFCapacity = (BW / NFBasicCap);
		if( BW % NFBasicCap != 0){
			NFCapacity++;
		}

		int OldServerCap = FullSolution->CurLists.ServerCapArray[NFSvr];
		auto  it5Again = FullSolution->CurLists.ServerCapHashTable.find(OldServerCap);
		it5Again->second.ServerIndex.erase(NFSvr);
			
		if(it5Again->second.ServerIndex.size() <= 0){
			FullSolution->CurLists.ServerCapHashTable.erase(OldServerCap);
			FullSolution->CurLists.ServerCapMap.erase(OldServerCap);
		}
						
		int OldServerNewCap = FullSolution->CurLists.ServerCapArray[NFSvr] + NFCapacity;
		FullSolution->CurLists.ServerCapArray[NFSvr] = OldServerNewCap;
		FullSolution->CurLists.ServerUsedList[NFSvr] = FullSolution->CurLists.ServerUsedList[NFSvr] - 1;
						
		std::map<int,short int>::iterator  it6Again = FullSolution->CurLists.ServerCapMap.find(OldServerNewCap);
			if( it6Again != FullSolution->CurLists.ServerCapMap.end()){
				auto it7Again = FullSolution->CurLists.ServerCapHashTable.find(OldServerNewCap);
				it7Again->second.ServerIndex.insert(NFSvr);
			}
		else{
			FullSolution->CurLists.ServerCapMap.insert(std::pair<int,short int>(OldServerNewCap,1));
			set<int> ValsNew;
			ValsNew.insert(NFSvr);
			VecForHashTable NewVecForHashTable;
			NewVecForHashTable.ServerIndex = ValsNew;
			FullSolution->CurLists.ServerCapHashTable.insert(std::pair<int,VecForHashTable>(OldServerNewCap,NewVecForHashTable));
		}

		// remove the old in path / path

		if(CurPath.Path.size() > 1){
			Update_LinksMatrix(FullSolution, CurPath, BW,"A"); // remove old assigned path by adding BW from relavant links
		}

		// remove the old out path
		if(loop == (NoOfNFs-1)){
			SinglePath OutPathOld = Check_OutPath(NFSvr,CurCommonList);
			Update_LinksMatrix(FullSolution, OutPathOld, BW,"A"); // remove old assigned path by adding BW from relavant links
		}

		BW = BW * MyNF.scale;
	}
}

void AddPolicy(FullSol* FullSolution, CommonList* CurCommonList, int policyIndex){

	int BW = CurCommonList->PolicyList[policyIndex].CurrentBW;
	short int NoOfNFs = CurCommonList->PolicyList[policyIndex].NFIDs.size();

	for(int loop=0; loop<NoOfNFs; loop++){
		NFAllocation NF = FullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy[loop];
		int NFSvr = NF.ServerID;	
		short int NFID = NF.NFID;
		SinglePath CurPath = NF.path;

		NFType MyNF = CurCommonList->NFInfo[NFID];
		int NFBasicCap = MyNF.cpu;
		int NFCapacity = (BW / NFBasicCap);
		if( BW % NFBasicCap != 0){
			NFCapacity++;
		}

		int OldServerCap = FullSolution->CurLists.ServerCapArray[NFSvr];
		auto  it5Again = FullSolution->CurLists.ServerCapHashTable.find(OldServerCap);
		it5Again->second.ServerIndex.erase(NFSvr);
			
		if(it5Again->second.ServerIndex.size() <= 0){
			FullSolution->CurLists.ServerCapHashTable.erase(OldServerCap);
			FullSolution->CurLists.ServerCapMap.erase(OldServerCap);
		}
						
		int OldServerNewCap = FullSolution->CurLists.ServerCapArray[NFSvr] - NFCapacity;
		FullSolution->CurLists.ServerCapArray[NFSvr] = OldServerNewCap;
		FullSolution->CurLists.ServerUsedList[NFSvr] = FullSolution->CurLists.ServerUsedList[NFSvr] + 1;
						
		std::map<int,short int>::iterator  it6Again = FullSolution->CurLists.ServerCapMap.find(OldServerNewCap);
			if( it6Again != FullSolution->CurLists.ServerCapMap.end()){
				auto it7Again = FullSolution->CurLists.ServerCapHashTable.find(OldServerNewCap);
				it7Again->second.ServerIndex.insert(NFSvr);
			}
		else{
			FullSolution->CurLists.ServerCapMap.insert(std::pair<int,short int>(OldServerNewCap,1));
			set<int> ValsNew;
			ValsNew.insert(NFSvr);
			VecForHashTable NewVecForHashTable;
			NewVecForHashTable.ServerIndex = ValsNew;
			FullSolution->CurLists.ServerCapHashTable.insert(std::pair<int,VecForHashTable>(OldServerNewCap,NewVecForHashTable));
		}

		// remove the old in path / path

		if(CurPath.Path.size() > 1){
			Update_LinksMatrix(FullSolution, CurPath, BW,"D"); // assign old  path by deleting BW from relavant links
		}

		// remove the old out path
		if(loop == (NoOfNFs-1)){
			SinglePath OutPathOld = Check_OutPath(NFSvr,CurCommonList);
			Update_LinksMatrix(FullSolution, OutPathOld, BW,"D"); // assign old path by deleting BW from relavant links
		}

		BW = BW * MyNF.scale;
	}
}

void RemovePolicyInRepeatedprocedure(FullSol* FullSolution, CommonList* CurCommonList, int policyIndex){

	int BW = FullSolution->VectorOfPartialSols[policyIndex].AllocatedBW;
	short int NoOfNFs = CurCommonList->PolicyList[policyIndex].NFIDs.size();

	if(FullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy.size() > 0){
		for(int loop=0; loop<NoOfNFs; loop++){
			NFAllocation NF = FullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy[loop];
			int NFSvr = NF.ServerID;	
			short int NFID = NF.NFID;
			SinglePath CurPath = NF.path;

			NFType MyNF = CurCommonList->NFInfo[NFID];
			int NFBasicCap = MyNF.cpu;
			int NFCapacity = (BW / NFBasicCap);
			if( BW % NFBasicCap != 0){
				NFCapacity++;
			}

			int OldServerCap = FullSolution->CurLists.ServerCapArray[NFSvr];
			auto  it5Again = FullSolution->CurLists.ServerCapHashTable.find(OldServerCap);
			it5Again->second.ServerIndex.erase(NFSvr);
			
			if(it5Again->second.ServerIndex.size() <= 0){
				FullSolution->CurLists.ServerCapHashTable.erase(OldServerCap);
				FullSolution->CurLists.ServerCapMap.erase(OldServerCap);
			}
						
			int OldServerNewCap = FullSolution->CurLists.ServerCapArray[NFSvr] + NFCapacity;
			FullSolution->CurLists.ServerCapArray[NFSvr] = OldServerNewCap;
			FullSolution->CurLists.ServerUsedList[NFSvr] = FullSolution->CurLists.ServerUsedList[NFSvr] - 1;
						
			std::map<int,short int>::iterator  it6Again = FullSolution->CurLists.ServerCapMap.find(OldServerNewCap);
				if( it6Again != FullSolution->CurLists.ServerCapMap.end()){
					auto it7Again = FullSolution->CurLists.ServerCapHashTable.find(OldServerNewCap);
					it7Again->second.ServerIndex.insert(NFSvr);
				}
			else{
				FullSolution->CurLists.ServerCapMap.insert(std::pair<int,short int>(OldServerNewCap,1));
				set<int> ValsNew;
				ValsNew.insert(NFSvr);
				VecForHashTable NewVecForHashTable;
				NewVecForHashTable.ServerIndex = ValsNew;
				FullSolution->CurLists.ServerCapHashTable.insert(std::pair<int,VecForHashTable>(OldServerNewCap,NewVecForHashTable));
			}

			// remove the old in path / path

			if(CurPath.Path.size() > 1){
				Update_LinksMatrix(FullSolution, CurPath, BW,"A"); // remove old assigned path by adding BW from relavant links
			}

			// remove the old out path
			if(loop == (NoOfNFs-1)){
				SinglePath OutPathOld = Check_OutPath(NFSvr,CurCommonList);
				Update_LinksMatrix(FullSolution, OutPathOld, BW,"A"); // remove old assigned path by adding BW from relavant links
			}

			BW = BW * MyNF.scale;
		}
	}
}

void AddPolicyInRepeatedprocedure(FullSol* FullSolution, CommonList* CurCommonList, int policyIndex){

	int BW = FullSolution->VectorOfPartialSols[policyIndex].AllocatedBW;
	short int NoOfNFs = CurCommonList->PolicyList[policyIndex].NFIDs.size();

	if(FullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy.size() > 0){
		for(int loop=0; loop<NoOfNFs; loop++){
			NFAllocation NF = FullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy[loop];
			int NFSvr = NF.ServerID;	
			short int NFID = NF.NFID;
			SinglePath CurPath = NF.path;

			NFType MyNF = CurCommonList->NFInfo[NFID];
			int NFBasicCap = MyNF.cpu;
			int NFCapacity = (BW / NFBasicCap);
			if( BW % NFBasicCap != 0){
				NFCapacity++;
			}

			int OldServerCap = FullSolution->CurLists.ServerCapArray[NFSvr];
			auto  it5Again = FullSolution->CurLists.ServerCapHashTable.find(OldServerCap);
			it5Again->second.ServerIndex.erase(NFSvr);
			
			if(it5Again->second.ServerIndex.size() <= 0){
				FullSolution->CurLists.ServerCapHashTable.erase(OldServerCap);
				FullSolution->CurLists.ServerCapMap.erase(OldServerCap);
			}
						
			int OldServerNewCap = FullSolution->CurLists.ServerCapArray[NFSvr] - NFCapacity;
			FullSolution->CurLists.ServerCapArray[NFSvr] = OldServerNewCap;
			FullSolution->CurLists.ServerUsedList[NFSvr] = FullSolution->CurLists.ServerUsedList[NFSvr] + 1;
						
			std::map<int,short int>::iterator  it6Again = FullSolution->CurLists.ServerCapMap.find(OldServerNewCap);
				if( it6Again != FullSolution->CurLists.ServerCapMap.end()){
					auto it7Again = FullSolution->CurLists.ServerCapHashTable.find(OldServerNewCap);
					it7Again->second.ServerIndex.insert(NFSvr);
				}
			else{
				FullSolution->CurLists.ServerCapMap.insert(std::pair<int,short int>(OldServerNewCap,1));
				set<int> ValsNew;
				ValsNew.insert(NFSvr);
				VecForHashTable NewVecForHashTable;
				NewVecForHashTable.ServerIndex = ValsNew;
				FullSolution->CurLists.ServerCapHashTable.insert(std::pair<int,VecForHashTable>(OldServerNewCap,NewVecForHashTable));
			}

			// remove the old in path / path

			if(CurPath.Path.size() > 1){
				Update_LinksMatrix(FullSolution, CurPath, BW,"D"); // assign old  path by deleting BW from relavant links
			}

			// remove the old out path
			if(loop == (NoOfNFs-1)){
				SinglePath OutPathOld = Check_OutPath(NFSvr,CurCommonList);
				Update_LinksMatrix(FullSolution, OutPathOld, BW,"D"); // assign old path by deleting BW from relavant links
			}

			BW = BW * MyNF.scale;
		}
	}
}

