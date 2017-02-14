void InitialLocalSearch(FullSol* FullSolution, CommonList* CurCommonLists, float LinksDelay, ofstream &myfile2, ofstream &myfile3){

	 for (auto local_it = FullSolution->NotAccepted.begin(); local_it!= FullSolution->NotAccepted.end(); ++local_it){
	
		bool sucess = false;
		int startNF = 0; // OnePlicyMove start from 1st NF of policy

		PartialSol SelectedPolicyOriginal = local_it->second;
		PartialSol SelectedPolicyTemp;
		int PolicyID = SelectedPolicyOriginal.PolicyID;
		int TotNFs = CurCommonLists->PolicyList[PolicyID].NFIDs.size();
		int BW = CurCommonLists->PolicyList[PolicyID].OriginalBW;

		if(TotNFs <= FullSolution->CPULeft){
			SelectedPolicyTemp.HaveResources = false;
			SelectedPolicyTemp.accepted = false;
			SelectedPolicyTemp.PolicyID = PolicyID;
			SelectedPolicyTemp.Splits = 0;
			SelectedPolicyTemp.Delay = 0;
			SelectedPolicyTemp.AllocatedBW = BW;

			sucess = InitialLocalSearchWithOnePolicyMove(FullSolution, CurCommonLists, PolicyID, startNF, BW, LinksDelay, &SelectedPolicyTemp, &SelectedPolicyOriginal);

			if (sucess == false){ // didnt find an improved sol, add the original policy back
				FullSolution->VectorOfPartialSols[PolicyID] = SelectedPolicyOriginal;
			}	
			if (sucess == true){ // deadline satisfied, obj should be increased and solution should be accepted
				GlobalObjectValueUpdateForIncrease(FullSolution, PolicyID, CurCommonLists);
				local_it->second = FullSolution->VectorOfPartialSols[PolicyID];
			}

                        PrintFullAllocation(0, 'I', FullSolution, myfile3);
			PrintAllocationLine(myfile3);	
		}
	}

	for (auto local_it1 = FullSolution->NotAccepted.begin(); local_it1!= FullSolution->NotAccepted.end();){	
		if(local_it1->second.accepted == true){
			int PolicyID = local_it1->first;
			FullSolution->Accepted.insert(std::pair<short int,PartialSol>(PolicyID,local_it1->second));
			local_it1 = FullSolution->NotAccepted.erase(local_it1);
		}
		else{
			local_it1++;
		}
	}

}
