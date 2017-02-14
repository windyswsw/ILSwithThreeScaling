void ScaleAcceptanceCriteria(FullSol* FullSolution, CommonList* CurCommonLists, float LinksDelay){

	int InitialObj = FullSolution->InitialObjVal;
	int CurObj = FullSolution->ObjVal;

	if(InitialObj > CurObj){ // Initial Solution is better

		int NoOfChanges = FullSolution->VectorOfChangedSols.size() - 1;

		for(int i=NoOfChanges; i>=0; i--){

			int policyIndex = FullSolution->VectorOfChangedSols[i].PolicyID;
			short int NoOfNFs = CurCommonLists->PolicyList[policyIndex].NFIDs.size();

			int BWCur = FullSolution->VectorOfOriginalSols[i].AllocatedBW;
			int BWNew = FullSolution->VectorOfChangedSols[i].AllocatedBW;

			for(int loop=0; loop<NoOfNFs; loop++){

				//////////////////////////////////////////////////////////////
				// undo the changed NF
				if(FullSolution->VectorOfChangedSols[i].VMsForAPolicy.size() > 0){
					NFAllocation NFO = FullSolution->VectorOfChangedSols[i].VMsForAPolicy[loop];
					int NFSvrO = NFO.ServerID;	
					short int NFIDO = NFO.NFID;
					SinglePath CurPathO = NFO.path;

					NFType NFTypeC = CurCommonLists->NFInfo[NFIDO];
					int NFBasicCap = NFTypeC.cpu;
					int NFCapacityO = (BWNew / NFBasicCap);
					if( BWNew % NFBasicCap != 0){
						NFCapacityO++;
					}

					int OldServerCapO = FullSolution->CurLists.ServerCapArray[NFSvrO];
					auto  it5AgainO = FullSolution->CurLists.ServerCapHashTable.find(OldServerCapO);
					it5AgainO->second.ServerIndex.erase(NFSvrO);
			
					if(it5AgainO->second.ServerIndex.size() <= 0){
						FullSolution->CurLists.ServerCapHashTable.erase(OldServerCapO);
						FullSolution->CurLists.ServerCapMap.erase(OldServerCapO);
					}
						
					int OldServerNewCapO = FullSolution->CurLists.ServerCapArray[NFSvrO] + NFCapacityO;
					FullSolution->CurLists.ServerCapArray[NFSvrO] = OldServerNewCapO;
					FullSolution->CurLists.ServerUsedList[NFSvrO] = FullSolution->CurLists.ServerUsedList[NFSvrO] - 1;
						
					std::map<int,short int>::iterator  it6AgainO = FullSolution->CurLists.ServerCapMap.find(OldServerNewCapO);
						if( it6AgainO != FullSolution->CurLists.ServerCapMap.end()){
							auto it7AgainO = FullSolution->CurLists.ServerCapHashTable.find(OldServerNewCapO);
							it7AgainO->second.ServerIndex.insert(NFSvrO);
						}
					else{
						FullSolution->CurLists.ServerCapMap.insert(std::pair<int,short int>(OldServerNewCapO,1));
						set<int> ValsNewO;
						ValsNewO.insert(NFSvrO);
						VecForHashTable NewVecForHashTableO;
						NewVecForHashTableO.ServerIndex = ValsNewO;
						FullSolution->CurLists.ServerCapHashTable.insert(std::pair<int,VecForHashTable>(OldServerNewCapO,NewVecForHashTableO));
					}

					// remove the old in path / path

					if(CurPathO.Path.size() > 1){
						Update_LinksMatrix(FullSolution, CurPathO, BWNew,"A"); // remove old assigned path by adding BW from relavant links
					}

					// remove the old out path
					if(loop == (NoOfNFs-1)){
						SinglePath OutPathOldO = Check_OutPath(NFSvrO,CurCommonLists);
						Update_LinksMatrix(FullSolution, OutPathOldO, BWNew,"A"); // remove old assigned out path by adding BW from relavant links
					}
				}
				////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// redo the original NF

				if(FullSolution->VectorOfOriginalSols[i].VMsForAPolicy.size() > 0){
					NFAllocation NF = FullSolution->VectorOfOriginalSols[i].VMsForAPolicy[loop];
					int NFSvr = NF.ServerID;	
					short int NFID = NF.NFID;
					SinglePath CurPath = NF.path;

					NFType NFTypeC1 = CurCommonLists->NFInfo[NFID];
					int NFBasicCap11 = NFTypeC1.cpu;
					int NFCapacity = (BWCur/ NFBasicCap11);
					if( BWCur % NFBasicCap11 != 0){
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

					// add the old in path / path

					if(CurPath.Path.size() > 1){
						Update_LinksMatrix(FullSolution, CurPath, BWCur,"D"); // add old assigned path by deleting BW from relavant links
					}

					// add the old out path
					if(loop == (NoOfNFs-1)){
						SinglePath OutPathOld = Check_OutPath(NFSvr,CurCommonLists);
						Update_LinksMatrix(FullSolution, OutPathOld, BWCur,"D"); // add old assigned out path by deliting BW from relavant links
					}
				}

				int ThisNFID = CurCommonLists->PolicyList[policyIndex].NFIDs[loop];
				NFType ThisNF = CurCommonLists->NFInfo[ThisNFID];
				BWNew = BWNew * ThisNF.scale;
				BWCur = BWCur * ThisNF.scale;
			}

			// update the full solution with old solution for policy
			FullSolution->VectorOfPartialSols[policyIndex] = FullSolution->VectorOfOriginalSols[i];

			int GivenRequiredBW = CurCommonLists->PolicyList[policyIndex].NextBW;
			int RequiredBW = ReturnMaxEnteringBWForAPolicy(CurCommonLists, policyIndex,GivenRequiredBW);
			int AllocatedBW = FullSolution->VectorOfPartialSols[policyIndex].AllocatedBW;

			if( RequiredBW > AllocatedBW){
				ScaleInfo ChangeReject;
				ChangeReject.AR = "A";
				ChangeReject.pol = policyIndex;
				ChangeReject.BWChange = RequiredBW - AllocatedBW;

				auto it1 = FullSolution->ScaleRejected.find(policyIndex);
				if (it1 != FullSolution->ScaleRejected.end()){ // if its already in scale rejected, update with new info
					it1->second = ChangeReject;
				}
				else{ // if not, insert
					FullSolution->ScaleRejected.insert(std::pair<short int,ScaleInfo>(policyIndex,ChangeReject));
				}
			}
			else{
				bool AlreadyAccepted = false; // check if its already in scale accepted
				for(int j=0; j<FullSolution->ScaleAccepted.size(); j++){
					if(FullSolution->ScaleAccepted[j] == policyIndex){
						AlreadyAccepted = true;
						break;
					}
				}
				if( AlreadyAccepted == false){
					FullSolution->ScaleAccepted.push_back(policyIndex);
				}
			}

		}

		FullSolution->ObjVal = InitialObj;

	}

}
