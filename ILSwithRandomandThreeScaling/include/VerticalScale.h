void VerticalScale(FullSol* InitialFullSolution, CommonList* CurCommonLists, float LinksDelay, int ServerCap, int InPathLinks, ofstream &myfile2, ChangeInfo CurrentChange, ChangedPolicies* TrackChangedPolicies){

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
	
	PartialSol SelectedPolicyOriginal;
	SelectedPolicyOriginal.accepted = InitialFullSolution->VectorOfPartialSols[PolicyID].accepted;
	SelectedPolicyOriginal.Delay = InitialFullSolution->VectorOfPartialSols[PolicyID].Delay;
	SelectedPolicyOriginal.HaveResources = InitialFullSolution->VectorOfPartialSols[PolicyID].HaveResources;
	SelectedPolicyOriginal.PolicyID = InitialFullSolution->VectorOfPartialSols[PolicyID].PolicyID;
	SelectedPolicyOriginal.Splits = InitialFullSolution->VectorOfPartialSols[PolicyID].Splits;
	SelectedPolicyOriginal.VMsForAPolicy = InitialFullSolution->VectorOfPartialSols[PolicyID].VMsForAPolicy;
	SelectedPolicyOriginal.AllocatedBW = InitialFullSolution->VectorOfPartialSols[PolicyID].AllocatedBW;

	int OriginalBW = CurCommonLists->PolicyList[PolicyID].OriginalBW;
	int CurrentBW = CurCommonLists->PolicyList[PolicyID].CurrentBW;
	int GivenNewBW = CurCommonLists->PolicyList[PolicyID].NextBW;
	int NewBW = ReturnMaxEnteringBWForAPolicy(CurCommonLists, PolicyID,GivenNewBW);
	int NewBWTemp = NewBW;

	if( Op == "A"){  // scale out

		int BWToCheck = NewBW;
		int BWToCheckTemp = NewBW;
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
	
		NewBW = BWToCheck;
		NoPointOfScaling = false;
		int OBW = 0;
		int NewBWFull = BWToCheck;

		if(AffordableN == true){

			RemovePolicy(InitialFullSolution, CurCommonLists, PolicyID); // remove the original configs
			ScaleObjectValueUpdateForDecrease(InitialFullSolution, PolicyID, CurCommonLists, CurrentBW); // remove obj for original
			
			while ((success == false) && (NoPointOfScaling == false)){
				if(OBW != NewBWFull){
					success = DFSVerticalScaling(InitialFullSolution, CurCommonLists, PolicyID, startNF, NewBWFull, LinksDelay, &SelectedPolicyTemp, &SelectedPolicyOriginal, TrackChangedPolicies);
				}
				OBW = NewBWFull;

				if (success == true){ // deadline satisfied, obj should be increased and solution should be accepted, BW of the original policy should be updated
					InitialFullSolution->VectorOfPartialSols[PolicyID].AllocatedBW = NewBWFull; // (total bw: current + new)
					ScaleObjectValueUpdateForIncrease(InitialFullSolution, PolicyID, CurCommonLists, NewBWFull); // add obj for total (original + new)

					InitialFullSolution->ScaleAccepted.push_back(PolicyID);

					if( NewBWFull != NewBWTemp){ // if total change was not able to accepted, remaining BW goes to rejected list
						ChangeReject.BWChange = NewBWTemp - NewBWFull;
						auto it1 = InitialFullSolution->ScaleRejected.find(PolicyID);
						if( it1 != InitialFullSolution->ScaleRejected.end()){
							it1->second = ChangeReject;
						}
						else{
							InitialFullSolution->ScaleRejected.insert(std::pair<short int,ScaleInfo>(PolicyID,ChangeReject));
						}
					}
				}	
				else{ // deadline not satisfied
					// if all three methods failed, try to scale less
					int nextTotBW = NextBW(InitialFullSolution, PolicyID, CurCommonLists, NewBW);
					if( (nextTotBW <= CurrentBW) || (nextTotBW > NewBWTemp) || (nextTotBW >= NewBWFull)){
						NoPointOfScaling = true;
					}
					else{
						NewBW = nextTotBW;
						NewBWFull = ReturnMaxEnteringBWForAPolicy(CurCommonLists, PolicyID, NewBW);
					}
				}
			}

			if ((success == false) && (NoPointOfScaling == true)){	
				AddPolicy(InitialFullSolution, CurCommonLists, PolicyID); // add the original configs back
				ScaleObjectValueUpdateForIncrease(InitialFullSolution, PolicyID, CurCommonLists, CurrentBW);
				ChangeReject.BWChange = NewBWTemp - CurrentBW;
				auto it2 = InitialFullSolution->ScaleRejected.find(PolicyID);
				if( it2 != InitialFullSolution->ScaleRejected.end()){
					it2->second = ChangeReject;
				}
				else{
					InitialFullSolution->ScaleRejected.insert(std::pair<short int,ScaleInfo>(PolicyID,ChangeReject));
				}
			}
		}
		else{ // tot bw is rejected
			ChangeReject.BWChange = NewBWTemp - CurrentBW;
			auto it3 = InitialFullSolution->ScaleRejected.find(PolicyID);
			if( it3 != InitialFullSolution->ScaleRejected.end()){
				it3->second = ChangeReject;
			}
			else{
				InitialFullSolution->ScaleRejected.insert(std::pair<short int,ScaleInfo>(PolicyID,ChangeReject));
			}
		}
	}

	if( Op == "D"){  // scale in
		if((OriginalBW <= NewBW) && (CurrentBW > NewBW)){ // currently allocated bw is greater than new bw
			RemovePolicy(InitialFullSolution, CurCommonLists, PolicyID); // remove the original configs
			ScaleObjectValueUpdateForDecrease(InitialFullSolution, PolicyID, CurCommonLists, CurrentBW); // remove obj for original
			success = DFSVerticalScaling(InitialFullSolution, CurCommonLists, PolicyID, startNF, NewBW, LinksDelay, &SelectedPolicyTemp, &SelectedPolicyOriginal, TrackChangedPolicies);
			InitialFullSolution->VectorOfPartialSols[PolicyID].AllocatedBW = NewBW;
			ScaleObjectValueUpdateForIncrease(InitialFullSolution, PolicyID, CurCommonLists,NewBW); // add obj for total (original + new)
			
			// since new BW is less than the currently allocated bw, even some part of the bw was previously rejected, we can remove that rejected bw
			auto it4 = InitialFullSolution->ScaleRejected.find(PolicyID); 
			if( it4 != InitialFullSolution->ScaleRejected.end()){
				InitialFullSolution->ScaleRejected.erase(it4);
			}
		}
		else if((OriginalBW <= NewBW) && (CurrentBW < NewBW)){ // currently allocated bw is less than new bw, recalculate rejected part
			ChangeReject.BWChange = NewBWTemp - CurrentBW;
			auto it5 = InitialFullSolution->ScaleRejected.find(PolicyID);
			if( it5 != InitialFullSolution->ScaleRejected.end()){
				it5->second = ChangeReject;
			}
		}
		else if((OriginalBW <= NewBW) && (CurrentBW == NewBW)){ // currently allocated bw is equal to new bw, so we can remove rejected part
			auto it6 = InitialFullSolution->ScaleRejected.find(PolicyID); 
			if( it6 != InitialFullSolution->ScaleRejected.end()){
				InitialFullSolution->ScaleRejected.erase(it6);
			}
		}
	}
}
