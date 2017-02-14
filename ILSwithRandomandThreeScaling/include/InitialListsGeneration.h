/*
 * InitialListsGeneration.h
 *
 *  Created on: Oct 19, 2015
 *      Author: u99319
 */

void GenUniqueList(UniqueList* NewAllList, int NoOfServers, int ServerCap, int MaxLinksCap){

	gen_ServerCapacityList(NewAllList, NoOfServers, ServerCap);
	gen_LinksList(NewAllList,"Links.txt", MaxLinksCap);

	for(int i=0; i<=NoOfServers;i++){
		NewAllList->ServerUsedList.push_back(0);
	}
}


void GenCommonList(CommonList* NewAllList){

	gen_PolicyList(NewAllList,"Policy.txt");
	GenAllPathsLists(NewAllList,"Paths.txt", "InPaths.txt", "OutPaths.txt");
	gen_NFList(NewAllList,"NFs.txt");
	MaxEnteringBW(NewAllList); // change into max BW
	vector<ChangeInfo> FullChangeListCurT;
	gen_PolicyChangeList(&FullChangeListCurT, "ScalePattern.txt");
	NewAllList->FullChangeListCur = FullChangeListCurT;

}
