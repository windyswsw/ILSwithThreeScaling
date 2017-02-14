/*
 * Print.h
 *
 *  Created on: Oct 19, 2015
 *      Author: u99319
 */

void PrintFullAllocation(int exp, char Op, FullSol* FinalSolution, ofstream &myfile){

	myfile << Op << "-- Policy Allocations" << "\n";

	for(int i=0; i<FinalSolution->VectorOfPartialSols.size();i++){
		myfile << FinalSolution->VectorOfPartialSols[i].PolicyID << "-";
                myfile << FinalSolution->VectorOfPartialSols[i].AllocatedBW << "-";
		for(int j=0; j<FinalSolution->VectorOfPartialSols[i].VMsForAPolicy.size();j++){
			NFAllocation valnew = FinalSolution->VectorOfPartialSols[i].VMsForAPolicy[j];
			myfile << valnew.ServerID << " ";
			for(int g=0; g< valnew.path.Path.size(); g++){
				myfile << valnew.path.Path[g] << "-";
			}
			myfile << ",";
		}
		myfile << "\n";
	}

	myfile << "Server Capacities" << "\n";

	for(int i=1; i<FinalSolution->CurLists.ServerCapArray.size();i++){
		myfile << i << "-" << FinalSolution->CurLists.ServerCapArray[i] << "," << " ";
	}

	myfile << "\n";

	/*myfile << "Links Capacities" << "\n";

	int a  = sizeof(FinalSolution->CurLists.LinksLists.LinksUse)/sizeof(FinalSolution->CurLists.LinksLists.LinksUse[0]);

	for(int i=0; i < a; ++i ){
		for(int j = 0; j < a; ++j){
			if(FinalSolution->CurLists.LinksLists.LinksUse[i][j] >= 0){
				int usedBW = FinalSolution->CurLists.LinksLists.LinksBW[i][j];
				myfile << i << "-" << j << " " << usedBW;
				myfile << "\n";
			}
		}
	}
	myfile << "\n";*/
}


void PrintAllocationAccepted(int exp, char Op, FullSol* FullSolution, ofstream &myfile){

	for (auto local_it = FullSolution->Accepted.begin(); local_it!= FullSolution->Accepted.end(); ++local_it){
		for(int j=0; j<local_it->second.VMsForAPolicy.size();j++){
			NFAllocation valnew = local_it->second.VMsForAPolicy[j];
			myfile << valnew.ServerID << " ";
			for(int g=0; g< valnew.path.Path.size(); g++){
				myfile << valnew.path.Path[g] << "-";
			}
			myfile << "\n";
		}
	}
}

void PrintFullAllocationNotAccepted(int exp, char Op, FullSol* FullSolution, ofstream &myfile){

	for (auto local_it = FullSolution->NotAccepted.begin(); local_it!= FullSolution->NotAccepted.end(); ++local_it){
		for(int j=0; j<local_it->second.VMsForAPolicy.size();j++){
			NFAllocation valnew = local_it->second.VMsForAPolicy[j];
			myfile << valnew.ServerID << " ";
			for(int g=0; g< valnew.path.Path.size(); g++){
				myfile << valnew.path.Path[g] << "-";
			}
			myfile << "\n";
		}
	}
}

void PrintAllocationPolicy(int exp, char Op, PartialSol* Policy, ofstream &myfile){

	for(int j=0; j<Policy->VMsForAPolicy.size();j++){
		NFAllocation valnew = Policy->VMsForAPolicy[j];
		myfile << valnew.ServerID << " ";
		for(int g=0; g< valnew.path.Path.size(); g++){
			myfile << valnew.path.Path[g] << "-";
		}
		myfile << "\n";
	}
}

void PrintAllocationLine(ofstream &myfile){
        //cout << "\n";
	myfile << "=============================================" << "\n";
}
