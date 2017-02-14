bool DFSRecursive(FullSol* InitialFullSolution, CommonList* CurCommonList, int policyIndex, PartialSol newVMMap, int CurrentNF, int BW){

	bool sol = false;
	short int NoOfNFs = CurCommonList->PolicyList[policyIndex].NFIDs.size();
	short int NFID = CurCommonList->PolicyList[policyIndex].NFIDs[CurrentNF];

	NFType MyNF = CurCommonList->NFInfo[NFID];
	int NFBasicCap = MyNF.cpu;
	int NFCapacity = (BW / NFBasicCap);
	if( BW % NFBasicCap != 0){
		NFCapacity++;
	}

	int TrafficToNextNF = BW * MyNF.scale;

	vector<int> ServerArr;

	for (std::map<int,short int>::iterator itlow=  InitialFullSolution->CurLists.ServerCapMap.lower_bound(NFCapacity); itlow!=InitialFullSolution->CurLists.ServerCapMap.end(); ++itlow){
		int FoundCap = itlow->first;
		auto it2 = InitialFullSolution->CurLists.ServerCapHashTable.find(FoundCap);

		for (std::set<int>::iterator itv = it2->second.ServerIndex.begin(); itv != it2->second.ServerIndex.end(); ++itv){
			ServerArr.push_back(*itv);
		}
	}

	//if( CurrentNF != 0){
	//	NFAllocation LeftNFN = newVMMap.VMsForAPolicy[CurrentNF - 1];
	//	short int LeftNFSvrN = LeftNFN.ServerID;
	//	bool NoLeftServer = ServerCapCheck(&InitialFullSolution->CurLists,LeftNFSvrN, NFCapacity); // check whether we can put in the server where left NF is
	//	if(NoLeftServer == false){
	//		ServerArr.push_back(LeftNFSvrN);
	//	}
	//}

	//for (std::map<int,short int>::reverse_iterator itlow = InitialFullSolution->CurLists.ServerCapMap.rbegin(); itlow!=InitialFullSolution->CurLists.ServerCapMap.rend(); ++itlow){
	//	int FoundCap = itlow->first;
	//	if(FoundCap >= NFCapacity){
	//		auto it2 = InitialFullSolution->CurLists.ServerCapHashTable.find(FoundCap);

	//		for (std::set<int>::iterator itv = it2->second.ServerIndex.begin(); itv != it2->second.ServerIndex.end(); ++itv){
	//			ServerArr.push_back(*itv);
	//		}
	//	}
	//	else{
	//		break;
	//	}
	//}

	bool sucess = false;

	for(int h=0; h< ServerArr.size();h++){

		int next_pos = CurrentNF + 1;
		NFAllocation NewNF;

		bool Assigned = false;

		if(!(CurrentNF == (NoOfNFs-1)) ){ // not the last NF in the chain

			if(CurrentNF == 0 ){ // first NF in the chain

				newVMMap.VMsForAPolicy.clear();
				newVMMap.Splits = 0;
				newVMMap.accepted = false;
				newVMMap.HaveResources = false;

				short int NFSvr = ServerArr[h];

				bool NoInPath = false;

				ReturnPath RPath;
				RPath = Find_InPath(NFSvr,CurCommonList, &InitialFullSolution->CurLists, BW);
				SinglePath InPath = RPath.NewPath;
				NoInPath = RPath.NoPathFound;

				if( NoInPath == false){ // there is an InPath
					Update_LinksMatrix(InitialFullSolution, InPath, BW,"D"); // add the new out path by deleting BW from relavant links

					// update the server

					// First delete the entry for current capacity		
					int CurrentCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr];
					auto it5 = InitialFullSolution->CurLists.ServerCapHashTable.find(CurrentCapOfServer);
					it5->second.ServerIndex.erase(NFSvr);

					if(it5->second.ServerIndex.size() <= 0){
						InitialFullSolution->CurLists.ServerCapHashTable.erase(CurrentCapOfServer);
						InitialFullSolution->CurLists.ServerCapMap.erase(CurrentCapOfServer);
					}
				
					// update the server with new capacity
					int NewCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr] - NFCapacity;
					InitialFullSolution->CurLists.ServerCapArray[NFSvr] = NewCapOfServer;
					InitialFullSolution->CurLists.ServerUsedList[NFSvr] = InitialFullSolution->CurLists.ServerUsedList[NFSvr] + 1;

					std::map<int,short int>::iterator  it3 = InitialFullSolution->CurLists.ServerCapMap.find(NewCapOfServer);
					if( it3 != InitialFullSolution->CurLists.ServerCapMap.end()){
						auto it4 = InitialFullSolution->CurLists.ServerCapHashTable.find(NewCapOfServer);
						it4->second.ServerIndex.insert(NFSvr);
					}
					else{
						InitialFullSolution->CurLists.ServerCapMap.insert(std::pair<int,short int>(NewCapOfServer,1));
						set<int> ValsNew;
						ValsNew.insert(NFSvr);
						VecForHashTable NewVecForHashTable;
						NewVecForHashTable.ServerIndex = ValsNew;
						InitialFullSolution->CurLists.ServerCapHashTable.insert(std::pair<int,VecForHashTable>(NewCapOfServer,NewVecForHashTable));
					}

					NewNF.ServerID = NFSvr;
					NewNF.NFID = NFID;
					NewNF.path = InPath;
					newVMMap.VMsForAPolicy.insert((newVMMap.VMsForAPolicy.begin()+CurrentNF), NewNF);

					Assigned = true;

					sucess = DFSRecursive(InitialFullSolution, CurCommonList, policyIndex, newVMMap, next_pos, TrafficToNextNF); // call to next NF 
				}
			}

			if(CurrentNF != 0 ) { // not the first NF or not the last NF in the chain
				
				short int NFSvr = ServerArr[h];
				
				NFAllocation LeftNF = newVMMap.VMsForAPolicy[CurrentNF-1];
				short int LeftNFSvr = LeftNF.ServerID;

				SinglePath NFPath2;
				ReturnPath RPath2;
				bool NoNFPath = false;

				if(NFSvr != LeftNFSvr){
					RPath2 = Find_Shortest_Path(LeftNFSvr, NFSvr, CurCommonList, &InitialFullSolution->CurLists, BW);
					NFPath2 = RPath2.NewPath;
					NoNFPath = RPath2.NoPathFound;
				}
				else{
					NFPath2.Path.push_back(0);
				}

				if(NoNFPath == false){ // there is a path, or NF and left NF are in same server, so continue
					if(NFPath2.Path.size() > 1 ){
						Update_LinksMatrix(InitialFullSolution, NFPath2, BW,"D"); // add the new out path by deleting BW from relavant links
					}

					// update the server
					
					// First delete the entry for current capacity		
						int CurrentCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr];
						auto it5 = InitialFullSolution->CurLists.ServerCapHashTable.find(CurrentCapOfServer);
						it5->second.ServerIndex.erase(NFSvr);

						if(it5->second.ServerIndex.size() <= 0){
							InitialFullSolution->CurLists.ServerCapHashTable.erase(CurrentCapOfServer);
							InitialFullSolution->CurLists.ServerCapMap.erase(CurrentCapOfServer);
						}
				
						// update the server with new capacity
						int NewCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr] - NFCapacity;
						InitialFullSolution->CurLists.ServerCapArray[NFSvr] = NewCapOfServer;
						InitialFullSolution->CurLists.ServerUsedList[NFSvr] = InitialFullSolution->CurLists.ServerUsedList[NFSvr] + 1;

						std::map<int,short int>::iterator  it3 = InitialFullSolution->CurLists.ServerCapMap.find(NewCapOfServer);
						if( it3 != InitialFullSolution->CurLists.ServerCapMap.end()){
							auto it4 = InitialFullSolution->CurLists.ServerCapHashTable.find(NewCapOfServer);
							it4->second.ServerIndex.insert(NFSvr);
						}
						else{
							InitialFullSolution->CurLists.ServerCapMap.insert(std::pair<int,short int>(NewCapOfServer,1));
							set<int> ValsNew;
							ValsNew.insert(NFSvr);
							VecForHashTable NewVecForHashTable;
							NewVecForHashTable.ServerIndex = ValsNew;
							InitialFullSolution->CurLists.ServerCapHashTable.insert(std::pair<int,VecForHashTable>(NewCapOfServer,NewVecForHashTable));
						}

						NewNF.ServerID = NFSvr;
						NewNF.NFID = NFID;
						NewNF.path = NFPath2;
						newVMMap.VMsForAPolicy.insert((newVMMap.VMsForAPolicy.begin()+CurrentNF), NewNF);

						if(NFSvr != LeftNFSvr){
							newVMMap.Splits = newVMMap.Splits + 1;
						}

						Assigned = true;

						sucess = DFSRecursive(InitialFullSolution, CurCommonList, policyIndex, newVMMap, next_pos, TrafficToNextNF); // call to next NF
					}
				}

				if(sucess == true){
					return sucess;
				}
				else{ // revert back all

					if( Assigned == true ){
						NFAllocation NFtoRevert = newVMMap.VMsForAPolicy[CurrentNF];
						short int NFtoRevertID = NFtoRevert.NFID;
						short int NFtoRevertCurSvr = NFtoRevert.ServerID;
						SinglePath NFtoChangePath = NFtoRevert.path;

						NFType NFTypeR = CurCommonList->NFInfo[NFtoRevertID];
						int NFBasicCap = NFTypeR.cpu;
						int NFtoRevertCapacity = (BW / NFBasicCap);
						if( BW % NFBasicCap != 0){
							NFtoRevertCapacity++;
						}

						if(NFtoChangePath.Path.size() >0){
							Update_LinksMatrix(InitialFullSolution, NFtoChangePath, BW,"A"); // update the path
						}

						// update the server

						// First delete the entry for current capacity		
						int CurrentCapOfNFtoRevertServer = InitialFullSolution->CurLists.ServerCapArray[NFtoRevertCurSvr];
						auto it5 = InitialFullSolution->CurLists.ServerCapHashTable.find(CurrentCapOfNFtoRevertServer);
						it5->second.ServerIndex.erase(NFtoRevertCurSvr);

						if(it5->second.ServerIndex.size() <= 0){
							InitialFullSolution->CurLists.ServerCapHashTable.erase(CurrentCapOfNFtoRevertServer);
							InitialFullSolution->CurLists.ServerCapMap.erase(CurrentCapOfNFtoRevertServer);
						}
				
						// update the server with new capacity
						int NewCapOfNFtoRevertServer = InitialFullSolution->CurLists.ServerCapArray[NFtoRevertCurSvr] + NFtoRevertCapacity;
						InitialFullSolution->CurLists.ServerCapArray[NFtoRevertCurSvr] = NewCapOfNFtoRevertServer;
						InitialFullSolution->CurLists.ServerUsedList[NFtoRevertCurSvr] = InitialFullSolution->CurLists.ServerUsedList[NFtoRevertCurSvr] - 1;

						std::map<int,short int>::iterator  it3 = InitialFullSolution->CurLists.ServerCapMap.find(NewCapOfNFtoRevertServer);
						if( it3 != InitialFullSolution->CurLists.ServerCapMap.end()){
							auto it4 = InitialFullSolution->CurLists.ServerCapHashTable.find(NewCapOfNFtoRevertServer);
							it4->second.ServerIndex.insert(NFtoRevertCurSvr);
						}
						else{
							InitialFullSolution->CurLists.ServerCapMap.insert(std::pair<int,short int>(NewCapOfNFtoRevertServer,1));
							set<int> ValsNew;
							ValsNew.insert(NFtoRevertCurSvr);
							VecForHashTable NewVecForHashTable;
							NewVecForHashTable.ServerIndex = ValsNew;
							InitialFullSolution->CurLists.ServerCapHashTable.insert(std::pair<int,VecForHashTable>(NewCapOfNFtoRevertServer,NewVecForHashTable));
						}

						newVMMap.VMsForAPolicy.pop_back();
					}
				}
					
			}

			if(CurrentNF == (NoOfNFs-1)) { // last NF in the chain

				short int NFSvr = ServerArr[h];
				
				NFAllocation LeftNF = newVMMap.VMsForAPolicy[CurrentNF-1];
				short int LeftNFSvr = LeftNF.ServerID;

				bool NoOutPath = false;
				ReturnPath RPath3;
				RPath3 = Find_OutPath(NFSvr,CurCommonList, &InitialFullSolution->CurLists, BW);
				SinglePath OutPath = RPath3.NewPath;
				NoOutPath = RPath3.NoPathFound;

				if( NoOutPath == false){
					Update_LinksMatrix(InitialFullSolution, OutPath, BW,"D"); // add the new out path by deleting BW from relavant links
					
					bool NoNFPath = false;
					SinglePath NFPath;
					ReturnPath RPath4;

					if(NFSvr != LeftNFSvr){
						RPath4 = Find_Shortest_Path(LeftNFSvr, NFSvr, CurCommonList, &InitialFullSolution->CurLists, BW);
						NFPath = RPath4.NewPath;
						NoNFPath = RPath4.NoPathFound;
					}
					else{
						NFPath.Path.push_back(0);
					}

					if(NoNFPath == false){ // there is a path, or NF and left NF are in same server, so continue
				
						if(NFPath.Path.size() > 1 ){
							Update_LinksMatrix(InitialFullSolution, NFPath, BW,"D"); // add the new out path by deleting BW from relavant links
						}

						// update the server

						// First delete the entry for current capacity		
						int CurrentCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr];
						auto it5 = InitialFullSolution->CurLists.ServerCapHashTable.find(CurrentCapOfServer);
						it5->second.ServerIndex.erase(NFSvr);

						if(it5->second.ServerIndex.size() <= 0){
							InitialFullSolution->CurLists.ServerCapHashTable.erase(CurrentCapOfServer);
							InitialFullSolution->CurLists.ServerCapMap.erase(CurrentCapOfServer);
						}
				
						// update the server with new capacity
						int NewCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr] - NFCapacity;
						InitialFullSolution->CurLists.ServerCapArray[NFSvr] = NewCapOfServer;
						InitialFullSolution->CurLists.ServerUsedList[NFSvr] = InitialFullSolution->CurLists.ServerUsedList[NFSvr] + 1;

						std::map<int,short int>::iterator  it3 = InitialFullSolution->CurLists.ServerCapMap.find(NewCapOfServer);
						if( it3 != InitialFullSolution->CurLists.ServerCapMap.end()){
							auto it4 = InitialFullSolution->CurLists.ServerCapHashTable.find(NewCapOfServer);
							it4->second.ServerIndex.insert(NFSvr);
						}
						else{
							InitialFullSolution->CurLists.ServerCapMap.insert(std::pair<int,short int>(NewCapOfServer,1));
							set<int> ValsNew;
							ValsNew.insert(NFSvr);
							VecForHashTable NewVecForHashTable;
							NewVecForHashTable.ServerIndex = ValsNew;
							InitialFullSolution->CurLists.ServerCapHashTable.insert(std::pair<int,VecForHashTable>(NewCapOfServer,NewVecForHashTable));
						}

							NewNF.ServerID = NFSvr;
							NewNF.NFID = NFID;
							NewNF.path = NFPath;
							newVMMap.VMsForAPolicy.insert((newVMMap.VMsForAPolicy.begin()+CurrentNF), NewNF);
							newVMMap.HaveResources = true;

							if(NFSvr != LeftNFSvr){
								newVMMap.Splits = newVMMap.Splits + 1;
							}

							InitialFullSolution->VectorOfPartialSols.push_back(newVMMap); // everything went fine, so save the configurations
							sucess = true;
							return sucess;
					}
					else{ // No Path to left NF, so revert back the OutPath
						Update_LinksMatrix(InitialFullSolution, OutPath, BW,"A"); // delete the new out path by adding BW from relavant links
					}
				}
			}
		}
	sucess = false;
	return sucess;
}
