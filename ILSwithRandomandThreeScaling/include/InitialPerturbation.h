void InitialPerturbation(FullSol* FullSolution, CommonList* CurCommonLists, short int NoOfPoliciesToChangeInPerturbation, float LinksDelay, ofstream &myfile1, ofstream &myfile2){
	
	for (int k = 0; k< NoOfPoliciesToChangeInPerturbation; k++){

		if(FullSolution->Accepted.size() > 1){

			int TotPolicy = CurCommonLists->PolicyList.size() -1;
			int PolicyGuessed = 0;
			int PolicyID = 0;
			PartialSol SelectedPolicyOriginal;
			bool success = false;
			bool PolicySelected = false;

			while(PolicySelected == false){
				PolicyGuessed = rand() % TotPolicy + 1;
				auto it1 = FullSolution->Accepted.find(PolicyGuessed);

				if (it1 != FullSolution->Accepted.end()){
					PolicyID = it1->first;
					SelectedPolicyOriginal = it1->second;
					PolicySelected = true;
				}
			}

			RemovePolicy(FullSolution, CurCommonLists, PolicyID);

			PartialSol EmptyVMMap;
			EmptyVMMap.PolicyID = PolicyID;
			EmptyVMMap.Splits = 0;
			EmptyVMMap.accepted = false;
			EmptyVMMap.Delay = 0;
			EmptyVMMap.HaveResources = false;
			EmptyVMMap.AllocatedBW = 0;
			FullSolution->VectorOfPartialSols[PolicyID] = EmptyVMMap;

			FullSolution->VectorOfOriginalSols.push_back(SelectedPolicyOriginal);
			FullSolution->VectorOfChangedSols.push_back(FullSolution->VectorOfPartialSols[PolicyID]);
			GlobalObjectValueUpdateForDecrease(FullSolution, PolicyID, CurCommonLists);

			auto it2 = FullSolution->Accepted.find(PolicyID);
			if (it2 != FullSolution->Accepted.end()){
				it2->second = FullSolution->VectorOfPartialSols[PolicyID];
				FullSolution->NotAccepted.insert(std::pair<short int,PartialSol>(PolicyID,it2->second));
				it2 = FullSolution->Accepted.erase(it2);
			}
		}

	}
}
