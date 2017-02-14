bool InitialLocalSearchWithOnePolicyMove(FullSol* InitialFullSolution, CommonList* CurCommonList, int policyIndex, int CurrentNF, int BW, float LinksDelay, PartialSol* newVMMap, PartialSol* SelectedPolicyOriginal ){

	int OriginalObj = InitialFullSolution->ObjVal;
	short int NoOfNFs = CurCommonList->PolicyList[policyIndex].NFIDs.size();
	short int NFID = CurCommonList->PolicyList[policyIndex].NFIDs[CurrentNF];

	NFType MyNF = CurCommonList->NFInfo[NFID];
	int NFBasicCap = MyNF.cpu;
	int NFCapacity = (BW / NFBasicCap);
	if( BW % NFBasicCap != 0){
		NFCapacity++;
	}
	int TrafficToNextNF = BW * MyNF.scale;

	// find a new server
	vector<int> ServerArr;

	if( CurrentNF != 0){
		NFAllocation LeftNFN = newVMMap->VMsForAPolicy[CurrentNF - 1];
		short int LeftNFSvrN = LeftNFN.ServerID;
		bool NoLeftServer = ServerCapCheck(&InitialFullSolution->CurLists,LeftNFSvrN, NFCapacity); // check whether we can put in the server where left NF is
		if(NoLeftServer == false){
			ServerArr.push_back(LeftNFSvrN);
		}
	}

	for (std::map<int,short int>::iterator itlow=  InitialFullSolution->CurLists.ServerCapMap.lower_bound(NFCapacity); itlow!=InitialFullSolution->CurLists.ServerCapMap.end(); ++itlow){
		if (itlow != InitialFullSolution->CurLists.ServerCapMap.end()){
			int FoundCap = itlow->first;
			auto it2 = InitialFullSolution->CurLists.ServerCapHashTable.find(FoundCap);

			for (std::set<int>::iterator itv = it2->second.ServerIndex.begin(); itv != it2->second.ServerIndex.end(); ++itv){
				if(itv != it2->second.ServerIndex.end()){
					ServerArr.push_back(*itv);
				}
			}
		}
	}

	int TotSvr = ServerArr.size() - 1 ;
	int RandomStart = 0;
	
	
	if( ServerArr.size() > 1){
		RandomStart = rand() % TotSvr + 1;
		if( CurrentNF != 0){
			swap(ServerArr[0], ServerArr[RandomStart]);
		}
	}

	bool sucess = false;

    for(int g=0; g< ServerArr.size();g++){
		int h = (g + RandomStart) % ServerArr.size();
		
		bool Assigned = false;

		NFAllocation NewNF;
		int next_pos = CurrentNF + 1;

		if(!(CurrentNF == (NoOfNFs-1)) ){ // not the last NF in the chain

			if(CurrentNF == 0 ){ // first NF in the chain

				short int NFSvr = ServerArr[h];

				newVMMap->VMsForAPolicy.clear();
				newVMMap->Splits = 0;
				newVMMap->accepted = false;
				newVMMap->HaveResources = true;

				bool NoInPath = false;
				bool NoRightPath = false;
				ReturnPath RPath;
				RPath = Find_InPath(NFSvr,CurCommonList, &InitialFullSolution->CurLists, BW);
				SinglePath InPath = RPath.NewPath;
				NoInPath = RPath.NoPathFound;

				if( NoInPath == false){ // there is an InPath

					PartialSol TempVMMap;
					TempVMMap.VMsForAPolicy = newVMMap->VMsForAPolicy;
					TempVMMap.PolicyID = policyIndex;

					NewNF.ServerID = NFSvr;
					NewNF.NFID = NFID;
					NewNF.path = InPath;
					TempVMMap.VMsForAPolicy.insert((TempVMMap.VMsForAPolicy.begin()+CurrentNF), NewNF);

					ReturnDelay RD;
					RD = LocalSearchDelayOK(&TempVMMap, CurCommonList, LinksDelay);
					
					bool DelayOK = RD.DeadlineOK;

					if(DelayOK == true){ // obj is better, so proceed

						newVMMap->VMsForAPolicy = TempVMMap.VMsForAPolicy;
					    
						Update_LinksMatrix(InitialFullSolution, InPath, BW,"D"); // add the new in path by deleting BW from relavant links

						// update the new server
						// First delete the entry for current capacity		
						int CurrentCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr];
						auto it55 = InitialFullSolution->CurLists.ServerCapHashTable.find(CurrentCapOfServer);
						it55->second.ServerIndex.erase(NFSvr);

						if(it55->second.ServerIndex.size() <= 0){
							InitialFullSolution->CurLists.ServerCapHashTable.erase(CurrentCapOfServer);
							InitialFullSolution->CurLists.ServerCapMap.erase(CurrentCapOfServer);
						}
				
						// update the server with new capacity
						int NewCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr] - NFCapacity;
						InitialFullSolution->CurLists.ServerCapArray[NFSvr] = NewCapOfServer;
						InitialFullSolution->CurLists.ServerUsedList[NFSvr] = InitialFullSolution->CurLists.ServerUsedList[NFSvr] + 1;

						std::map<int,short int>::iterator  it33 = InitialFullSolution->CurLists.ServerCapMap.find(NewCapOfServer);
						if( it33 != InitialFullSolution->CurLists.ServerCapMap.end()){
							auto it44 = InitialFullSolution->CurLists.ServerCapHashTable.find(NewCapOfServer);
							it44->second.ServerIndex.insert(NFSvr);
						}
						else{
							InitialFullSolution->CurLists.ServerCapMap.insert(std::pair<int,short int>(NewCapOfServer,1));
							set<int> ValsNew1;
							ValsNew1.insert(NFSvr);
							VecForHashTable NewVecForHashTable1;
							NewVecForHashTable1.ServerIndex = ValsNew1;
							InitialFullSolution->CurLists.ServerCapHashTable.insert(std::pair<int,VecForHashTable>(NewCapOfServer,NewVecForHashTable1));
						}

						Assigned = true;						
						sucess = InitialLocalSearchWithOnePolicyMove(InitialFullSolution, CurCommonList, policyIndex, next_pos, TrafficToNextNF, LinksDelay, newVMMap, SelectedPolicyOriginal); // call to next NF
					}
				}
			}

			if(CurrentNF != 0 ) { // not the first NF or not the last NF in the chain

				// proceed with new allocation
				
				short int NFSvr = ServerArr[h];

				NFAllocation LeftNF = newVMMap->VMsForAPolicy[CurrentNF - 1];
				short int LeftNFSvr = LeftNF.ServerID;

				SinglePath NFPath;
				ReturnPath RPath1;

				if(NFSvr != LeftNFSvr){
					int NoOfPaths = CurCommonList->PathsList.PathsList[LeftNFSvr][NFSvr].size();

					for (int h= (NoOfPaths-1); h>=0; h--){
						bool NoNFPath = false;
						bool AssignedInThisLoop = false;
 				
						//if(rand()%100 <=3 ){
                                                
						RPath1 = Find_Shortest_PathAll(LeftNFSvr, NFSvr, CurCommonList, &InitialFullSolution->CurLists, BW, h);
						NFPath = RPath1.NewPath;
						NoNFPath = RPath1.NoPathFound;

						if(NoNFPath == false){ // there is a path so continue

							PartialSol TempVMMap;
							TempVMMap.VMsForAPolicy = newVMMap->VMsForAPolicy;
							TempVMMap.PolicyID = policyIndex;

							NewNF.ServerID = NFSvr;
							NewNF.NFID = NFID;
							NewNF.path = NFPath;
							TempVMMap.VMsForAPolicy.insert((TempVMMap.VMsForAPolicy.begin()+CurrentNF), NewNF);

							ReturnDelay RD;
							RD = LocalSearchDelayOK(&TempVMMap, CurCommonList, LinksDelay);

							bool DelayOK = RD.DeadlineOK;

							if(DelayOK == true){ // obj is better, so proceed

								newVMMap->VMsForAPolicy = TempVMMap.VMsForAPolicy;
					    
								if(NFPath.Path.size() > 1 ){
									Update_LinksMatrix(InitialFullSolution, NFPath, BW,"D"); // add the new out path by deleting BW from relavant links
								}

									// update the new server
									// First delete the entry for current capacity		
								int CurrentCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr];
								auto it55 = InitialFullSolution->CurLists.ServerCapHashTable.find(CurrentCapOfServer);
								it55->second.ServerIndex.erase(NFSvr);

								if(it55->second.ServerIndex.size() <= 0){
									InitialFullSolution->CurLists.ServerCapHashTable.erase(CurrentCapOfServer);
									InitialFullSolution->CurLists.ServerCapMap.erase(CurrentCapOfServer);
								}
				
								// update the server with new capacity
								int NewCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr] - NFCapacity;
								InitialFullSolution->CurLists.ServerCapArray[NFSvr] = NewCapOfServer;
								InitialFullSolution->CurLists.ServerUsedList[NFSvr] = InitialFullSolution->CurLists.ServerUsedList[NFSvr] + 1;

								std::map<int,short int>::iterator  it33 = InitialFullSolution->CurLists.ServerCapMap.find(NewCapOfServer);
								if( it33 != InitialFullSolution->CurLists.ServerCapMap.end()){
									auto it44 = InitialFullSolution->CurLists.ServerCapHashTable.find(NewCapOfServer);
									it44->second.ServerIndex.insert(NFSvr);
								}
								else{
									InitialFullSolution->CurLists.ServerCapMap.insert(std::pair<int,short int>(NewCapOfServer,1));
									set<int> ValsNew1;
									ValsNew1.insert(NFSvr);
									VecForHashTable NewVecForHashTable1;
									NewVecForHashTable1.ServerIndex = ValsNew1;
									InitialFullSolution->CurLists.ServerCapHashTable.insert(std::pair<int,VecForHashTable>(NewCapOfServer,NewVecForHashTable1));
								}
								
								Assigned = true;
								AssignedInThisLoop = true;
								newVMMap->Splits = newVMMap->Splits + 1;

								sucess = InitialLocalSearchWithOnePolicyMove(InitialFullSolution, CurCommonList, policyIndex, next_pos, TrafficToNextNF, LinksDelay, newVMMap, SelectedPolicyOriginal); // call to next NF
							}
                            else{
								break;
                            }
						}
						if(sucess == true){
							break;
						}
						else if ((sucess == false) && (h != (NoOfPaths-1)) && (AssignedInThisLoop == true)){ // next NF was not sucessful, and this is not last path for current NF, before going to next path, undo currect configs

							newVMMap->Splits = newVMMap->Splits - 1;
							Update_LinksMatrix(InitialFullSolution, NFPath, BW,"A");

							// revert back the new server

							// First delete the entry for current capacity		
							int CurrentCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr];
							auto it53 = InitialFullSolution->CurLists.ServerCapHashTable.find(CurrentCapOfServer);
							it53->second.ServerIndex.erase(NFSvr);

							if(it53->second.ServerIndex.size() <= 0){
								InitialFullSolution->CurLists.ServerCapHashTable.erase(CurrentCapOfServer);
								InitialFullSolution->CurLists.ServerCapMap.erase(CurrentCapOfServer);
							}
				
							// update the server with new capacity
							int NewCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr] + NFCapacity;
							InitialFullSolution->CurLists.ServerCapArray[NFSvr] = NewCapOfServer;
							InitialFullSolution->CurLists.ServerUsedList[NFSvr] = InitialFullSolution->CurLists.ServerUsedList[NFSvr] - 1;

							std::map<int,short int>::iterator  it33 = InitialFullSolution->CurLists.ServerCapMap.find(NewCapOfServer);
							if( it33 != InitialFullSolution->CurLists.ServerCapMap.end()){
								auto it43 = InitialFullSolution->CurLists.ServerCapHashTable.find(NewCapOfServer);
								it43->second.ServerIndex.insert(NFSvr);
							}
							else{
								InitialFullSolution->CurLists.ServerCapMap.insert(std::pair<int,short int>(NewCapOfServer,1));
								set<int> ValsNew33;
								ValsNew33.insert(NFSvr);
								VecForHashTable NewVecForHashTable33;
								NewVecForHashTable33.ServerIndex = ValsNew33;
								InitialFullSolution->CurLists.ServerCapHashTable.insert(std::pair<int,VecForHashTable>(NewCapOfServer,NewVecForHashTable33));
							}

							newVMMap->VMsForAPolicy.pop_back();
						}
						//}

					}				
				}

				if(NFSvr == LeftNFSvr){ // NF and left NF in same server
					NFPath.Path.push_back(0);

					PartialSol TempVMMap;
					TempVMMap.VMsForAPolicy = newVMMap->VMsForAPolicy;
					TempVMMap.PolicyID = policyIndex;

					NewNF.ServerID = NFSvr;
					NewNF.NFID = NFID;
					NewNF.path = NFPath;
					TempVMMap.VMsForAPolicy.insert((TempVMMap.VMsForAPolicy.begin()+CurrentNF), NewNF);

					ReturnDelay RD;
					RD = LocalSearchDelayOK(&TempVMMap, CurCommonList, LinksDelay);

					bool DelayOK = RD.DeadlineOK;

					if(DelayOK == true){ // obj is better, so proceed

						newVMMap->VMsForAPolicy = TempVMMap.VMsForAPolicy;

						// update the new server
						// First delete the entry for current capacity		
						int CurrentCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr];
						auto it55 = InitialFullSolution->CurLists.ServerCapHashTable.find(CurrentCapOfServer);
						it55->second.ServerIndex.erase(NFSvr);

						if(it55->second.ServerIndex.size() <= 0){
							InitialFullSolution->CurLists.ServerCapHashTable.erase(CurrentCapOfServer);
							InitialFullSolution->CurLists.ServerCapMap.erase(CurrentCapOfServer);
						}
				
						// update the server with new capacity
						int NewCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr] - NFCapacity;
						InitialFullSolution->CurLists.ServerCapArray[NFSvr] = NewCapOfServer;
						InitialFullSolution->CurLists.ServerUsedList[NFSvr] = InitialFullSolution->CurLists.ServerUsedList[NFSvr] + 1;

						std::map<int,short int>::iterator  it33 = InitialFullSolution->CurLists.ServerCapMap.find(NewCapOfServer);
						if( it33 != InitialFullSolution->CurLists.ServerCapMap.end()){
							auto it44 = InitialFullSolution->CurLists.ServerCapHashTable.find(NewCapOfServer);
							it44->second.ServerIndex.insert(NFSvr);
						}
						else{
							InitialFullSolution->CurLists.ServerCapMap.insert(std::pair<int,short int>(NewCapOfServer,1));
							set<int> ValsNew1;
							ValsNew1.insert(NFSvr);
							VecForHashTable NewVecForHashTable1;
							NewVecForHashTable1.ServerIndex = ValsNew1;
							InitialFullSolution->CurLists.ServerCapHashTable.insert(std::pair<int,VecForHashTable>(NewCapOfServer,NewVecForHashTable1));
						}
								
						Assigned = true;
						sucess = InitialLocalSearchWithOnePolicyMove(InitialFullSolution, CurCommonList, policyIndex, next_pos,TrafficToNextNF, LinksDelay, newVMMap, SelectedPolicyOriginal); // call to next NF
					}
				}
			}

			if(sucess == true){
				return sucess;
			}
			else{ // revert back all

				if( Assigned == true ){

					NFAllocation NFtoRevert = newVMMap->VMsForAPolicy[CurrentNF];
					short int NFtoRevertID = NFtoRevert.NFID;
					short int NFtoRevertCurSvr = NFtoRevert.ServerID;
					SinglePath NFtoChangePath = NFtoRevert.path;

					NFType NFTypeR = CurCommonList->NFInfo[NFtoRevertID];
					int NFBasicCap = NFTypeR.cpu;
					int NFtoRevertCapacity = (BW / NFBasicCap);
					if( BW % NFBasicCap != 0){
						NFtoRevertCapacity++;
					}
					if(CurrentNF != 0){
						NFAllocation LeftNF = newVMMap->VMsForAPolicy[CurrentNF-1];
						short int LeftSvr = LeftNF.ServerID;

						if(LeftSvr != NFtoRevertCurSvr){
							newVMMap->Splits = newVMMap->Splits - 1;
						}
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

					newVMMap->VMsForAPolicy.pop_back();
					
				}
			}
				
		}

		if(CurrentNF == (NoOfNFs-1) ) { // last NF in the chain

			// proceed with new allocation

			short int NFSvr = ServerArr[h];
				
			NFAllocation LeftNF = newVMMap->VMsForAPolicy[CurrentNF - 1];
			short int LeftNFSvr = LeftNF.ServerID;

			bool NoOutPath = false;
			ReturnPath RPath3;
			RPath3 = Find_OutPath(NFSvr,CurCommonList, &InitialFullSolution->CurLists, BW);
			SinglePath OutPath = RPath3.NewPath;
			NoOutPath = RPath3.NoPathFound;

			if( NoOutPath == false){
				Update_LinksMatrix(InitialFullSolution, OutPath, BW,"D"); // add the new out path by deleting BW from relavant links

				SinglePath NFPath;
				ReturnPath RPath4;
				bool NoInPath = false;

				if(NFSvr != LeftNFSvr){

					int NoOfPaths = CurCommonList->PathsList.PathsList[LeftNFSvr][NFSvr].size();

					for (int h=(NoOfPaths-1); h>= 0; h--){
						bool NoNFPath = false;
						bool AssignedInThisLoop = false;

						RPath4 = Find_Shortest_PathAll(LeftNFSvr, NFSvr, CurCommonList, &InitialFullSolution->CurLists, BW, h);
						NFPath = RPath4.NewPath;
						NoNFPath = RPath4.NoPathFound;

						if(NoNFPath == false){ // there is a path so continue

							PartialSol TempVMMap;
							TempVMMap.VMsForAPolicy = newVMMap->VMsForAPolicy;
							TempVMMap.PolicyID = policyIndex;

							NewNF.ServerID = NFSvr;
							NewNF.NFID = NFID;
							NewNF.path = NFPath;
							TempVMMap.VMsForAPolicy.insert((TempVMMap.VMsForAPolicy.begin()+CurrentNF), NewNF);

							ReturnDelay RD;
							RD = LocalSearchDelayOK(&TempVMMap, CurCommonList, LinksDelay);

							bool DelayOK = RD.DeadlineOK;
							float RDelay = RD.delay;

							if(DelayOK == true){ // obj is better, so proceed

								newVMMap->VMsForAPolicy = TempVMMap.VMsForAPolicy;
								newVMMap->accepted = true;
								newVMMap->Delay = RDelay;
								newVMMap->HaveResources = true;
								newVMMap->PolicyID = policyIndex;
								newVMMap->Splits = newVMMap->Splits + 1;
					    
								if(NFPath.Path.size() > 1 ){
									Update_LinksMatrix(InitialFullSolution, NFPath, BW,"D"); // add the new out path by deleting BW from relavant links
								}

									// update the new server
									// First delete the entry for current capacity		
								int CurrentCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr];
								auto it55 = InitialFullSolution->CurLists.ServerCapHashTable.find(CurrentCapOfServer);
								it55->second.ServerIndex.erase(NFSvr);

								if(it55->second.ServerIndex.size() <= 0){
									InitialFullSolution->CurLists.ServerCapHashTable.erase(CurrentCapOfServer);
									InitialFullSolution->CurLists.ServerCapMap.erase(CurrentCapOfServer);
								}
				
								// update the server with new capacity
								int NewCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr] - NFCapacity;
								InitialFullSolution->CurLists.ServerCapArray[NFSvr] = NewCapOfServer;
								InitialFullSolution->CurLists.ServerUsedList[NFSvr] = InitialFullSolution->CurLists.ServerUsedList[NFSvr] + 1;

								std::map<int,short int>::iterator  it33 = InitialFullSolution->CurLists.ServerCapMap.find(NewCapOfServer);
								if( it33 != InitialFullSolution->CurLists.ServerCapMap.end()){
									auto it44 = InitialFullSolution->CurLists.ServerCapHashTable.find(NewCapOfServer);
									it44->second.ServerIndex.insert(NFSvr);
								}
								else{
									InitialFullSolution->CurLists.ServerCapMap.insert(std::pair<int,short int>(NewCapOfServer,1));
									set<int> ValsNew1;
									ValsNew1.insert(NFSvr);
									VecForHashTable NewVecForHashTable1;
									NewVecForHashTable1.ServerIndex = ValsNew1;
									InitialFullSolution->CurLists.ServerCapHashTable.insert(std::pair<int,VecForHashTable>(NewCapOfServer,NewVecForHashTable1));
								}

								InitialFullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy = newVMMap->VMsForAPolicy;
								InitialFullSolution->VectorOfPartialSols[policyIndex].Splits = newVMMap->Splits;
								InitialFullSolution->VectorOfPartialSols[policyIndex].accepted = newVMMap->accepted;
								InitialFullSolution->VectorOfPartialSols[policyIndex].Delay = newVMMap->Delay;
								InitialFullSolution->VectorOfPartialSols[policyIndex].PolicyID = newVMMap->PolicyID;
								InitialFullSolution->VectorOfPartialSols[policyIndex].HaveResources = newVMMap->HaveResources;
								
								PartialSol TempP;
								TempP.VMsForAPolicy = newVMMap->VMsForAPolicy;
								TempP.Splits = newVMMap->Splits;
								TempP.accepted = newVMMap->accepted;
								TempP.Delay = newVMMap->Delay;
								TempP.PolicyID = newVMMap->PolicyID;
								TempP.HaveResources = newVMMap->HaveResources;
								InitialFullSolution->VectorOfChangedSols.push_back(TempP);
							
								PartialSol OriginP;
								OriginP.VMsForAPolicy = SelectedPolicyOriginal->VMsForAPolicy;
								OriginP.Splits = SelectedPolicyOriginal->Splits;
								OriginP.accepted = SelectedPolicyOriginal->accepted;
								OriginP.Delay = SelectedPolicyOriginal->Delay;
								OriginP.PolicyID = SelectedPolicyOriginal->PolicyID;
								OriginP.HaveResources = SelectedPolicyOriginal->HaveResources;
								InitialFullSolution->VectorOfOriginalSols.push_back(OriginP);
									
								AssignedInThisLoop = true;
								sucess = true;
								return sucess;
							}
                            else{
                                 break;
                            }
						}
                                                //} // restricting paths
						if(sucess == true){
							break;
						}
						else if ((sucess == false) && (h != (NoOfPaths-1)) && (AssignedInThisLoop == true)){ // this is not last path for current NF, before going to next path, undo currect configs

							Update_LinksMatrix(InitialFullSolution, NFPath, BW,"A");
							newVMMap->Splits = newVMMap->Splits - 1;

							// revert back the new server

							// First delete the entry for current capacity		
							int CurrentCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr];
							auto it53 = InitialFullSolution->CurLists.ServerCapHashTable.find(CurrentCapOfServer);
							it53->second.ServerIndex.erase(NFSvr);

							if(it53->second.ServerIndex.size() <= 0){
								InitialFullSolution->CurLists.ServerCapHashTable.erase(CurrentCapOfServer);
								InitialFullSolution->CurLists.ServerCapMap.erase(CurrentCapOfServer);
							}
				
							// update the server with new capacity
							int NewCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr] + NFCapacity;
							InitialFullSolution->CurLists.ServerCapArray[NFSvr] = NewCapOfServer;
							InitialFullSolution->CurLists.ServerUsedList[NFSvr] = InitialFullSolution->CurLists.ServerUsedList[NFSvr] - 1;

							std::map<int,short int>::iterator  it33 = InitialFullSolution->CurLists.ServerCapMap.find(NewCapOfServer);
							if( it33 != InitialFullSolution->CurLists.ServerCapMap.end()){
								auto it43 = InitialFullSolution->CurLists.ServerCapHashTable.find(NewCapOfServer);
								it43->second.ServerIndex.insert(NFSvr);
							}
							else{
								InitialFullSolution->CurLists.ServerCapMap.insert(std::pair<int,short int>(NewCapOfServer,1));
								set<int> ValsNew33;
								ValsNew33.insert(NFSvr);
								VecForHashTable NewVecForHashTable33;
								NewVecForHashTable33.ServerIndex = ValsNew33;
								InitialFullSolution->CurLists.ServerCapHashTable.insert(std::pair<int,VecForHashTable>(NewCapOfServer,NewVecForHashTable33));
							}

							newVMMap->VMsForAPolicy.pop_back();
						}

					}	
					if(sucess == false){ // no inpaths, so remove out path
						Update_LinksMatrix(InitialFullSolution, OutPath, BW,"A");
					}
				}

				if(NFSvr == LeftNFSvr){ // NF and left NF in same server
					NFPath.Path.push_back(0);

					PartialSol TempVMMap;
					TempVMMap.VMsForAPolicy = newVMMap->VMsForAPolicy;
					TempVMMap.PolicyID = policyIndex;

					NewNF.ServerID = NFSvr;
					NewNF.NFID = NFID;
					NewNF.path = NFPath;
					TempVMMap.VMsForAPolicy.insert((TempVMMap.VMsForAPolicy.begin()+CurrentNF), NewNF);

					ReturnDelay RD;
					RD = LocalSearchDelayOK(&TempVMMap, CurCommonList, LinksDelay);
					
					bool DelayOK = RD.DeadlineOK;
					float RDelay = RD.delay;
					
					if(DelayOK == true){ // obj is better, so proceed, last NF

						newVMMap->VMsForAPolicy = TempVMMap.VMsForAPolicy;
						newVMMap->accepted = true;
						newVMMap->Delay = RDelay;
						newVMMap->HaveResources = true;
						newVMMap->PolicyID = policyIndex;

							// update the new server
								// First delete the entry for current capacity		
						int CurrentCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr];
						auto it55 = InitialFullSolution->CurLists.ServerCapHashTable.find(CurrentCapOfServer);
						it55->second.ServerIndex.erase(NFSvr);

						if(it55->second.ServerIndex.size() <= 0){
							InitialFullSolution->CurLists.ServerCapHashTable.erase(CurrentCapOfServer);
							InitialFullSolution->CurLists.ServerCapMap.erase(CurrentCapOfServer);
						}
				
						// update the server with new capacity
						int NewCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr] - NFCapacity;
						InitialFullSolution->CurLists.ServerCapArray[NFSvr] = NewCapOfServer;
						InitialFullSolution->CurLists.ServerUsedList[NFSvr] = InitialFullSolution->CurLists.ServerUsedList[NFSvr] + 1;

						std::map<int,short int>::iterator  it33 = InitialFullSolution->CurLists.ServerCapMap.find(NewCapOfServer);
						if( it33 != InitialFullSolution->CurLists.ServerCapMap.end()){
							auto it44 = InitialFullSolution->CurLists.ServerCapHashTable.find(NewCapOfServer);
							it44->second.ServerIndex.insert(NFSvr);
						}
						else{
							InitialFullSolution->CurLists.ServerCapMap.insert(std::pair<int,short int>(NewCapOfServer,1));
							set<int> ValsNew1;
							ValsNew1.insert(NFSvr);
							VecForHashTable NewVecForHashTable1;
							NewVecForHashTable1.ServerIndex = ValsNew1;
							InitialFullSolution->CurLists.ServerCapHashTable.insert(std::pair<int,VecForHashTable>(NewCapOfServer,NewVecForHashTable1));
						}
								
						InitialFullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy = newVMMap->VMsForAPolicy;
						InitialFullSolution->VectorOfPartialSols[policyIndex].Splits = newVMMap->Splits;
						InitialFullSolution->VectorOfPartialSols[policyIndex].accepted = newVMMap->accepted;
						InitialFullSolution->VectorOfPartialSols[policyIndex].Delay = newVMMap->Delay;
						InitialFullSolution->VectorOfPartialSols[policyIndex].PolicyID = newVMMap->PolicyID;
						InitialFullSolution->VectorOfPartialSols[policyIndex].HaveResources = newVMMap->HaveResources;

						PartialSol TempP;
						TempP.VMsForAPolicy = newVMMap->VMsForAPolicy;
						TempP.Splits = newVMMap->Splits;
						TempP.accepted = newVMMap->accepted;
						TempP.Delay = newVMMap->Delay;
						TempP.PolicyID = newVMMap->PolicyID;
						TempP.HaveResources = newVMMap->HaveResources;
						InitialFullSolution->VectorOfChangedSols.push_back(TempP);
							
						PartialSol OriginP;
						OriginP.VMsForAPolicy = SelectedPolicyOriginal->VMsForAPolicy;
						OriginP.Splits = SelectedPolicyOriginal->Splits;
						OriginP.accepted = SelectedPolicyOriginal->accepted;
						OriginP.Delay = SelectedPolicyOriginal->Delay;
						OriginP.PolicyID = SelectedPolicyOriginal->PolicyID;
						OriginP.HaveResources = SelectedPolicyOriginal->HaveResources;
						InitialFullSolution->VectorOfOriginalSols.push_back(OriginP);
							
						sucess = true;
						return sucess;
					}
				}
			}
		}
	}

	sucess = false;
	return sucess;
}
