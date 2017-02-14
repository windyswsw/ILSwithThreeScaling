void IncrementalScale(FullSol* InitialFullSolution, CommonList* CurCommonLists, float LinksDelay, int ServerCap, int InPathLinks, ofstream &myfile2, ChangeInfo CurrentChange, ChangedPolicies* TrackChangedPolicies){

	bool success = false;
	int startNF = 0; // OnePlicyMove start from 1st NF of policy

	int PolicyID = CurrentChange.polToChange;
	int ScaleBW = CurrentChange.ScaleBW;
	string Op = CurrentChange.AR;

	ScaleInfo ChangeReject;
	ChangeReject.AR = CurrentChange.AR;
	ChangeReject.pol = CurrentChange.polToChange;

	PartialSol SelectedPolicyTemp;
	SelectedPolicyTemp.HaveResources = false;
	SelectedPolicyTemp.accepted = false;
	SelectedPolicyTemp.PolicyID = PolicyID;
	SelectedPolicyTemp.Splits = 0;
	SelectedPolicyTemp.Delay = 0;

	int OriginalBW = CurCommonLists->PolicyList[PolicyID].OriginalBW;
	int CurrentBW = CurCommonLists->PolicyList[PolicyID].CurrentBW;
	int TotalNextBW = CurCommonLists->PolicyList[PolicyID].NextBW;

	int TotalCurrentBW = InitialFullSolution->VectorOfPartialSols[PolicyID].AllocatedBW; // total bw distributed to parent and children
	for( int f=0; f< CurCommonLists->PolicyList[PolicyID].Children.size(); f++){
		int ChildID = CurCommonLists->PolicyList[PolicyID].Children[f];
		int ChildBW = InitialFullSolution->VectorOfPartialSols[ChildID].AllocatedBW;
		TotalCurrentBW = TotalCurrentBW + ChildBW;
	}

	if( Op == "A"){  // scale out

		auto it612 = InitialFullSolution->ScaleRejected.find(PolicyID); // if parent in rejected list, remove it
		if( it612 != InitialFullSolution->ScaleRejected.end()){
			InitialFullSolution->ScaleRejected.erase(it612);
		}

		// since we are trying to give total bw needed using a new child, remove all the rejected bw so far saved under child policies
		for( int f1=0; f1< CurCommonLists->PolicyList[PolicyID].Children.size(); f1++){
			int ChildID1 = CurCommonLists->PolicyList[PolicyID].Children[f1];
			auto it61 = InitialFullSolution->ScaleRejected.find(ChildID1); // if child in rejected list, remove it
			if( it61 != InitialFullSolution->ScaleRejected.end()){
				InitialFullSolution->ScaleRejected.erase(it61);
			}
		}

		int GivenBWIncrease = TotalNextBW - TotalCurrentBW;

		if(GivenBWIncrease > 0){ 

			int BWIncrease = ReturnMaxEnteringBWForAPolicy(CurCommonLists, PolicyID,GivenBWIncrease);
			int BWIncreaseTemp = BWIncrease;

			int BWToCheck = BWIncrease;
			int BWToCheckTemp = BWIncrease;
			bool AffordableN = false;
			bool NoPointOfScaling = false;
			while( (AffordableN == false) && ( NoPointOfScaling == false)){
				AffordableN = Affordable(CurCommonLists->PolicyList[PolicyID], ServerCap, CurCommonLists->NFInfo, LinksDelay, InPathLinks, BWToCheck);
				if(AffordableN == false){
					int nextTotBW = NextBW(InitialFullSolution, PolicyID, CurCommonLists, BWToCheck);
					if( (nextTotBW <= CurrentBW) || (nextTotBW > BWToCheckTemp)){
						NoPointOfScaling = true;
					}
					else{
						BWToCheck = nextTotBW;
					}
				}
			}

			BWIncrease = BWToCheck;
			NoPointOfScaling = false;
			int OBW = 0;
			int BWIncreaseFull = BWToCheck;

			int NewPolicyID = CurCommonLists->PolicyList.size();
			CurCommonLists->PolicyList.push_back(CurCommonLists->PolicyList[PolicyID]); // add child policy
			CurCommonLists->PolicyList[NewPolicyID].PolicyID = NewPolicyID;
			CurCommonLists->PolicyList[NewPolicyID].Parent = PolicyID;
			ChangeReject.pol = NewPolicyID;

			CurCommonLists->PolicyList[PolicyID].Children.push_back(NewPolicyID); // children

			CurCommonLists->PolicyList[NewPolicyID].OriginalBW = 0;
			CurCommonLists->PolicyList[NewPolicyID].CurrentBW = 0; // since the policy is just created, current, original, next all BWs are same
			CurCommonLists->PolicyList[NewPolicyID].NextBW = BWIncreaseTemp; // 

			SelectedPolicyTemp.PolicyID = NewPolicyID;
			PartialSol SelectedPolicyOriginal = SelectedPolicyTemp;

			if(AffordableN == true){

				while ((success == false) && (NoPointOfScaling == false)){

					if(OBW != BWIncreaseFull){
						success = DFSIncrementalScaling(InitialFullSolution, CurCommonLists, NewPolicyID, startNF, BWIncreaseFull, LinksDelay, &SelectedPolicyTemp, &SelectedPolicyOriginal, TrackChangedPolicies);
					}
					OBW = BWIncreaseFull;

					if (success == true){ 
						InitialFullSolution->VectorOfPartialSols[NewPolicyID].AllocatedBW = BWIncreaseFull;
						CurCommonLists->PolicyList[NewPolicyID].OriginalBW = BWIncreaseFull;
						CurCommonLists->PolicyList[NewPolicyID].CurrentBW = BWIncreaseFull;
						ScaleObjectValueUpdateForIncrease(InitialFullSolution, NewPolicyID, CurCommonLists,BWIncreaseFull); 

						InitialFullSolution->ScaleAccepted.push_back(NewPolicyID);

						if( BWIncreaseFull != BWIncreaseTemp){ 
							ChangeReject.pol = NewPolicyID;
							ChangeReject.BWChange = BWIncreaseTemp - BWIncreaseFull;
							InitialFullSolution->ScaleRejected.insert(std::pair<short int,ScaleInfo>(NewPolicyID,ChangeReject));
						}
					}	
					else{ // deadline not satisfied
						// if all three methods failed, try to scale less
						int nextTotBW = NextBW(InitialFullSolution, NewPolicyID, CurCommonLists, BWIncrease);
						if( (nextTotBW <= OriginalBW) || (nextTotBW > BWIncrease) || (nextTotBW >= BWIncreaseFull) ){
							NoPointOfScaling = true;
						}
						else{
							BWIncrease = nextTotBW;
							BWIncreaseFull = ReturnMaxEnteringBWForAPolicy(CurCommonLists, PolicyID, BWIncrease);
						}
					}
				}

				if ((success == false) && (NoPointOfScaling == true)){		
					PartialSol EmptyVMMap;
					EmptyVMMap.PolicyID = NewPolicyID;
					EmptyVMMap.Splits = 0;
					EmptyVMMap.accepted = false;
					EmptyVMMap.Delay = 0;
					EmptyVMMap.HaveResources = false;
					EmptyVMMap.AllocatedBW = 0;
					InitialFullSolution->VectorOfPartialSols.push_back(EmptyVMMap);
					ChangeReject.BWChange = BWIncreaseTemp;
					InitialFullSolution->ScaleRejected.insert(std::pair<short int,ScaleInfo>(NewPolicyID,ChangeReject)); // not accepted
				}
			}
			else{ // tot bw is rejectd
				PartialSol EmptyVMMap;
				EmptyVMMap.PolicyID = NewPolicyID;
				EmptyVMMap.Splits = 0;
				EmptyVMMap.accepted = false;
				EmptyVMMap.Delay = 0;
				EmptyVMMap.HaveResources = false;
				EmptyVMMap.AllocatedBW = 0;
				InitialFullSolution->VectorOfPartialSols.push_back(EmptyVMMap);
				ChangeReject.BWChange = BWIncreaseTemp;
				InitialFullSolution->ScaleRejected.insert(std::pair<short int,ScaleInfo>(NewPolicyID,ChangeReject)); // not accepted
			}
		}
	}

	if( Op == "D"){  // scale in

		int BWDecrease = TotalCurrentBW - TotalNextBW;
		
		if(OriginalBW <= TotalNextBW){

			int NoOfChildren = CurCommonLists->PolicyList[PolicyID].Children.size();

			if( TotalCurrentBW > TotalNextBW){
				int loops = 0;
				while( (TotalCurrentBW > TotalNextBW) && (loops < NoOfChildren)){ // currently allocaed bw to parent and child is higher than new bw reduction, so we can deduct bw
				
					int ChildID = CurCommonLists->PolicyList[PolicyID].Children[loops];
					int ChildBW = InitialFullSolution->VectorOfPartialSols[ChildID].AllocatedBW;

					if( BWDecrease < ChildBW){ // child is bigger than bw change
						RemovePolicyInRepeatedprocedure(InitialFullSolution, CurCommonLists, ChildID); // remove the child configs
						ScaleObjectValueUpdateForDecrease(InitialFullSolution, ChildID, CurCommonLists, ChildBW); // remove obj for original

						int ChildNewBWGiven = ChildBW - BWDecrease;
						int ChildNewBW = ReturnMaxEnteringBWForAPolicy(CurCommonLists, PolicyID,ChildNewBWGiven);
						CurCommonLists->PolicyList[ChildID].NextBW = ChildNewBW; //
						CurCommonLists->PolicyList[ChildID].CurrentBW = ChildNewBW;
						InitialFullSolution->VectorOfPartialSols[ChildID].AllocatedBW = ChildNewBW;

						AddPolicyInRepeatedprocedure(InitialFullSolution, CurCommonLists, ChildID);// add new configs
						ScaleObjectValueUpdateForIncrease(InitialFullSolution, ChildID, CurCommonLists, ChildNewBW); // add new obj
						TotalCurrentBW = TotalCurrentBW - BWDecrease;
					}
					else{ // child bw is less than bw change	
						RemovePolicyInRepeatedprocedure(InitialFullSolution, CurCommonLists, ChildID); // remove the child configs
						ScaleObjectValueUpdateForDecrease(InitialFullSolution, ChildID, CurCommonLists, ChildBW); // remove obj for original
						InitialFullSolution->VectorOfPartialSols[ChildID].AllocatedBW = 0;
						CurCommonLists->PolicyList[ChildID].CurrentBW = 0;
						CurCommonLists->PolicyList[ChildID].NextBW = 0;
					
						PartialSol EmptyVMMap; // add an empty VMMap to ChildPolicy
						EmptyVMMap.PolicyID = ChildID;
						EmptyVMMap.Splits = 0;
						EmptyVMMap.accepted = false;
						EmptyVMMap.Delay = 0;
						EmptyVMMap.HaveResources = false;
						EmptyVMMap.AllocatedBW = 0;
						InitialFullSolution->VectorOfPartialSols[ChildID] = EmptyVMMap;

						TotalCurrentBW = TotalCurrentBW - ChildBW;
						BWDecrease = BWDecrease - ChildBW;
					}

					loops++;
				}

				// currently allocaed bw to parent and child is higher than new bw reduction, remove all the rejected bw so far saved under child policies
				for( int f11=0; f11< CurCommonLists->PolicyList[PolicyID].Children.size(); f11++){
					int ChildID11 = CurCommonLists->PolicyList[PolicyID].Children[f11];
					auto it611 = InitialFullSolution->ScaleRejected.find(ChildID11); // if child in rejected list, remove it
					if( it611 != InitialFullSolution->ScaleRejected.end()){
						InitialFullSolution->ScaleRejected.erase(it611);
					}
				}
			}
			else if( TotalCurrentBW < TotalNextBW){
				int rejectedBW = TotalNextBW - TotalCurrentBW;
				// remove all the rejected bw so far saved under child policies, and save the rejected bw as the rejected of original
				for( int f11=0; f11< CurCommonLists->PolicyList[PolicyID].Children.size(); f11++){
					int ChildID11 = CurCommonLists->PolicyList[PolicyID].Children[f11];
					auto it611 = InitialFullSolution->ScaleRejected.find(ChildID11); // if child in rejected list, remove it
					if( it611 != InitialFullSolution->ScaleRejected.end()){
						InitialFullSolution->ScaleRejected.erase(it611);
					}
				}

				auto it6112 = InitialFullSolution->ScaleRejected.find(PolicyID);
				if( it6112 != InitialFullSolution->ScaleRejected.end()){
					it6112->second.BWChange = rejectedBW;
				}
				else{
					ChangeReject.BWChange = rejectedBW;
					InitialFullSolution->ScaleRejected.insert(std::pair<short int,ScaleInfo>(PolicyID,ChangeReject));
				}
			}
			else if( TotalCurrentBW == TotalNextBW){
				// remove all the rejected bw so far saved under child policies
				for( int f11=0; f11< CurCommonLists->PolicyList[PolicyID].Children.size(); f11++){
					int ChildID11 = CurCommonLists->PolicyList[PolicyID].Children[f11];
					auto it611 = InitialFullSolution->ScaleRejected.find(ChildID11); // if child in rejected list, remove it
					if( it611 != InitialFullSolution->ScaleRejected.end()){
						InitialFullSolution->ScaleRejected.erase(it611);
					}
				}
				//remove the rejected bw under parent policies
				auto it6112 = InitialFullSolution->ScaleRejected.find(PolicyID);
				if( it6112 != InitialFullSolution->ScaleRejected.end()){
					InitialFullSolution->ScaleRejected.erase(it6112);
				}
			}
		}
	}
}
