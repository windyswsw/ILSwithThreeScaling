void LSVerticalScale(FullSol* InitialFullSolution, CommonList* CurCommonLists, float LinksDelay, int ServerCap, int InPathLinks,  ofstream &myfile2, ChangedPolicies* TrackChangedPolicies){
	
	for (auto local_it = InitialFullSolution->ScaleRejected.begin(); local_it!= InitialFullSolution->ScaleRejected.end();){
	
		bool success = false;
		bool NoPointOfScaling = false;
		int startNF = 0; // OnePlicyMove start from 1st NF of policy

		ScaleInfo RejectedInfo = local_it->second;
		int PolicyID = RejectedInfo.pol;
		int RejectedBW = RejectedInfo.BWChange;

		ScaleInfo ChangeReject = RejectedInfo;

		int AllocatedBW = InitialFullSolution->VectorOfPartialSols[PolicyID].AllocatedBW;
		int GivenNewBW = CurCommonLists->PolicyList[PolicyID].NextBW;
		int NewBW = ReturnMaxEnteringBWForAPolicy(CurCommonLists, PolicyID,GivenNewBW);
		int NewBWTemp = NewBW;

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

		int CurrentBW = CurCommonLists->PolicyList[PolicyID].CurrentBW;


		int BWToCheck = NewBW;
		int BWToCheckTemp = NewBW;
		bool AffordableN = false;
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

			RemovePolicyInRepeatedprocedure(InitialFullSolution, CurCommonLists, PolicyID); // remove the original configs
			ScaleObjectValueUpdateForDecrease(InitialFullSolution, PolicyID, CurCommonLists, AllocatedBW); // remove obj for original

			while ((success == false) && (NoPointOfScaling == false)){
				
				if(OBW != NewBWFull){
					success = DFSVerticalScaling(InitialFullSolution, CurCommonLists, PolicyID, startNF, NewBWFull, LinksDelay, &SelectedPolicyTemp, &SelectedPolicyOriginal, TrackChangedPolicies);
				}
				OBW = NewBWFull;
				
				if (success == true){ // deadline satisfied, obj should be increased and solution should be accepted, BW of the original policy should be updated
					InitialFullSolution->VectorOfPartialSols[PolicyID].AllocatedBW = NewBWFull; // (total bw: current + new)
					ScaleObjectValueUpdateForIncrease(InitialFullSolution, PolicyID, CurCommonLists, NewBWFull); // add obj for total (original + new)

					bool AlreadyAccepted = false; // check if its already in scale accepted
					for(int j=0; j<InitialFullSolution->ScaleAccepted.size(); j++){
						if(InitialFullSolution->ScaleAccepted[j] == PolicyID){
							AlreadyAccepted = true;
							break;
						}
					}
					if( AlreadyAccepted == false){
						InitialFullSolution->ScaleAccepted.push_back(PolicyID);
					}

					if( NewBWFull != NewBWTemp){ // if total change was not able to accepted, remaining BW goes to rejected list
						ChangeReject.BWChange = NewBWTemp - NewBWFull;
						//its already in scale rejected, update with new info and move to next item
						local_it->second = ChangeReject;
						local_it++;
					}
					else{ // total change was accepted, remove from rejeted list, now the next iteration shud access same position
						local_it = InitialFullSolution->ScaleRejected.erase(local_it);
					}
				}	
				else{ // deadline not satisfied
					// if all three methods failed, try to scale less
					int nextTotBW = NextBW(InitialFullSolution, PolicyID, CurCommonLists, NewBW);
					if(( nextTotBW <= AllocatedBW) || (nextTotBW > NewBWTemp) || (nextTotBW >= NewBWFull)){
						NoPointOfScaling = true;
					}
					else{
						NewBW = nextTotBW;
						NewBWFull = ReturnMaxEnteringBWForAPolicy(CurCommonLists, PolicyID, NewBW);
					}
				}
			}
			
			if ((success == false) && (NoPointOfScaling == true)){	
				AddPolicyInRepeatedprocedure(InitialFullSolution, CurCommonLists, PolicyID); // add the original configs back
				ScaleObjectValueUpdateForIncrease(InitialFullSolution, PolicyID, CurCommonLists, AllocatedBW);
				local_it++;
			}
		} // tot bw is rejected
		else{
			local_it++;
		}
	}
}

















