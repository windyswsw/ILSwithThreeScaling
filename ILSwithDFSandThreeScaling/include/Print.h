/*
 * Print.h
 *
 *  Created on: Oct 19, 2015
 *      Author: u99319
 */

void PrintSummaryGlobal(int exp, FullSol* InitialFullSolution, ofstream &myfile, char Op, CommonList* CurCommonList){

     // std::ostream myfile(std::cout.rdbuf());

	time_t rawtime;
	  struct tm * timeinfo;

	  time (&rawtime);
	  timeinfo = localtime (&rawtime);

        //myfile << asctime(timeinfo) <<  "---" << ",";

	int NoOfServersUsed = 0;
	int NoOfLinksUsed = 0;

	for(int f=0; f<InitialFullSolution->CurLists.ServerUsedList.size();f++){
		if(InitialFullSolution->CurLists.ServerUsedList[f] > 0){
			NoOfServersUsed = NoOfServersUsed + 1;
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

	/*int a  = sizeof(InitialFullSolution->CurLists.LinksLists.LinksUse)/sizeof(InitialFullSolution->CurLists.LinksLists.LinksUse[0]);

	for(int i=0; i < a; ++i ){
		for(int j = 0; j < a; ++j){
			if(InitialFullSolution->CurLists.LinksLists.LinksUse[i][j] > 0){
				NoOfLinksUsed = NoOfLinksUsed + 1;
			}
		}
	}*/

	int Accepted = 0;
	int NotAccepted = 0;

	for (int y = 0; y < InitialFullSolution->Accepted.size(); y++){
		Accepted = Accepted + 1;
	}

	for (int y = 0; y < InitialFullSolution->NotAccepted.size(); y++){
		NotAccepted = NotAccepted + 1;
	}

	myfile << exp  << "," << Op << "," << NoOfServersUsed << "," << ServerCapLeft << "," << Accepted << "," << NotAccepted << "," << InitialFullSolution->ObjVal << ",";

}

void PrintNewLine(ofstream &myfile){
	myfile <<  "\n";
}

void PrintFirstLine(ofstream &myfile){
	myfile <<  "exp"  << "," << "Op" << "," << "NoOfServersUsed" << "," << "ServerCapLeft" << "," << "Accepted" << "," << "NotAccepted" << "," << "ObjVal" << ",";
}

void PrintScaleFirstLine(ofstream &myfile){
	myfile << "exp"  << "," << "Op" << "," << "NoOfServersUsed" << "," << "ServerCapLeft" << "," << "ScaleAccepted" << "," << "ScaleNotAccepted" << "," << "BWRejected" << "," << "svrChange" << "," << "linksChange" << "," << "ObjVal" << "," << "MaxCPUs" << ",";
}


void PrintSummaryScale(int exp, FullSol* InitialFullSolution, ofstream &myfile, char Op, CommonList* CurCommonList, ChangedPolicies* TrackChangedPolicies){

	// std::ostream myfile(std::cout.rdbuf());

	time_t rawtime;
	  struct tm * timeinfo;

	  time (&rawtime);
	  timeinfo = localtime (&rawtime);

        //myfile << asctime(timeinfo) <<  "---" << ",";

	int NoOfServersUsed = 0;
	int NoOfLinksUsed = 0;

	for(int f=0; f<InitialFullSolution->CurLists.ServerUsedList.size();f++){
		if(InitialFullSolution->CurLists.ServerUsedList[f] > 0){
			NoOfServersUsed = NoOfServersUsed + 1;
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

	/*int a  = sizeof(InitialFullSolution->CurLists.LinksLists.LinksUse)/sizeof(InitialFullSolution->CurLists.LinksLists.LinksUse[0]);

	for(int i=0; i < a; ++i ){
		for(int j = 0; j < a; ++j){
			if(InitialFullSolution->CurLists.LinksLists.LinksUse[i][j] > 0){
				NoOfLinksUsed = NoOfLinksUsed + 1;
			}
		}
	}*/

	int ScaleAccepted = 0;
	int ScaleNotAccepted = 0;

	for (int y = 0; y < InitialFullSolution->ScaleAccepted.size(); y++){
		ScaleAccepted = ScaleAccepted + 1;
	}

	for (int y = 0; y < InitialFullSolution->ScaleRejected.size(); y++){
		ScaleNotAccepted = ScaleNotAccepted + 1;
	}

	int BWRejected = 0;
	for (auto local_it = InitialFullSolution->ScaleRejected.begin(); local_it!= InitialFullSolution->ScaleRejected.end(); local_it++){
		ScaleInfo RejectedInfo = local_it->second;
		int PolicyID = RejectedInfo.pol;
		BWRejected = BWRejected + RejectedInfo.BWChange;
	}

	int svrChange = 0;
	int linksChange = 0;

	for (std::map<short int, PartialSol>::iterator it=TrackChangedPolicies->CurrentlyChangedPolicies.begin(); it!=TrackChangedPolicies->CurrentlyChangedPolicies.end(); ++it){
		short int Pol= it->first;
		PartialSol NewSol = it->second;

		if(it->second.VMsForAPolicy.size() > 0){ // current VMMap is not empty
			// compare it with original policies
			std::map<short int,PartialSol>::iterator  it4 = TrackChangedPolicies->OriginalPolicies.find(Pol);
			if( it4 != TrackChangedPolicies->OriginalPolicies.end()){
				PartialSol OrgSol = it4->second;

				if(it4->second.VMsForAPolicy.size() > 0){ // old VMMap is not empty
					for(int k=0; k< OrgSol.VMsForAPolicy.size(); k++){
						NFAllocation NFO = OrgSol.VMsForAPolicy[k];
						int NFSvrO = NFO.ServerID;	
						SinglePath PathO = NFO.path;

						NFAllocation NFC = NewSol.VMsForAPolicy[k];
						int NFSvrC = NFC.ServerID;	
						SinglePath PathC = NFC.path;

						if(NFSvrO != NFSvrC){
							svrChange = svrChange + 1;
						}

						if(PathO.Path != PathC.Path){
							if(PathC.Path.size() >1){
								linksChange =  linksChange + (PathC.Path.size()-1);
							}
						}
					}
				}
				else{ // old VM map is empty, so everything is new
					for(int k=0; k< NewSol.VMsForAPolicy.size(); k++){
						svrChange = svrChange + 1;
						
						NFAllocation NFC = NewSol.VMsForAPolicy[k];
						int NFSvrC = NFC.ServerID;	
						SinglePath PathC = NFC.path;

						if(PathC.Path.size() >1){
							linksChange =  linksChange + (PathC.Path.size()-1);
						}
					}
				}
			}
		}
	}

	myfile << exp  << "," << Op << "," << NoOfServersUsed << "," << ServerCapLeft << "," << ScaleAccepted << "," << ScaleNotAccepted << "," << BWRejected << "," << svrChange << "," << linksChange << "," << InitialFullSolution->ObjVal << "," << InitialFullSolution->MaxCPUs <<",";
}
