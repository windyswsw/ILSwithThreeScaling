SinglePath Check_InPath(int dst, CommonList* All){
	SinglePath spath = All->PathsList.InPaths[dst];
	return spath;
}

SinglePath Check_OutPath(int dst, CommonList* All){
	SinglePath spath = All->PathsList.OutPaths[dst];
	return spath;
}

bool Link_Check(SinglePath Path, UniqueList* All, int BW){
	bool NoBWflag = false;
	for(int l=0; l<Path.Path.size()-1;l++){
		if(All->LinksLists.LinksBW[Path.Path[l]][Path.Path[l+1]] < BW){
			NoBWflag = true;
			break;
		}
	}
   return NoBWflag;
}

ReturnPath Find_InPath(int dst, CommonList* CurCommon, UniqueList* CurUnique, int BW){
	ReturnPath Rpath;
	Rpath.NewPath = CurCommon->PathsList.InPaths[dst];
	Rpath.NoPathFound = Link_Check(Rpath.NewPath, CurUnique, BW);
	return Rpath;
}

ReturnPath Find_OutPath(int src, CommonList* CurCommon, UniqueList* CurUnique, int BW){
	ReturnPath Rpath;
	Rpath.NewPath = CurCommon->PathsList.OutPaths[src];
	Rpath.NoPathFound = Link_Check(Rpath.NewPath, CurUnique, BW);
	return Rpath;
}

ReturnPath Find_Shortest_Path(int src, int dst, CommonList* CurCommon, UniqueList* CurUnique, int BW){
	ReturnPath Rpath;
	if(CurCommon->PathsList.PathsList[src][dst].size() > 1){
		for(int s=0; s< CurCommon->PathsList.PathsList[src][dst].size(); s++){
			Rpath.NewPath = CurCommon->PathsList.PathsList[src][dst][s];
			Rpath.NoPathFound = Link_Check(Rpath.NewPath, CurUnique, BW);
			if(Rpath.NoPathFound == false){
				break;
			}
		}
	}
	else{
		Rpath.NewPath = CurCommon->PathsList.PathsList[src][dst][0];
		Rpath.NoPathFound = Link_Check(Rpath.NewPath, CurUnique, BW);
	}
	return Rpath;
}

ReturnPath Find_Shortest_PathRandom(int src, int dst, CommonList* All, UniqueList* CurUnique, int BW){
	ReturnPath Rpath;
	if(All->PathsList.PathsList[src][dst].size() == 1){
		Rpath.NewPath = All->PathsList.PathsList[src][dst][0];
	}
	else{
		int Pos = All->PathsList.PathsList[src][dst].size() - 1;
		int PathPos = rand() % Pos + 1;
		Rpath.NewPath = All->PathsList.PathsList[src][dst][PathPos];
	}

	Rpath.NoPathFound = Link_Check(Rpath.NewPath, CurUnique, BW);
	return Rpath;
}

ReturnPath Find_Shortest_PathAll(int src, int dst, CommonList* All, UniqueList* CurUnique, int BW, int loop){
	ReturnPath Rpath;
	Rpath.NewPath = All->PathsList.PathsList[src][dst][loop];
	Rpath.NoPathFound = Link_Check(Rpath.NewPath, CurUnique, BW);
	return Rpath;
}

bool NewLinks_Check(SinglePath Path, UniqueList* All){
	bool NoNewLinks = false;
	/*int OldLinksCount = 0;
	for(int l=0; l<Path.Path.size()-1;l++){
		if(All->LinksLists.LinksUse[Path.Path[l]][Path.Path[l+1]] >= 1 ){
			OldLinksCount = OldLinksCount + 1;
		}
	}

	int NewLinksCount = (Path.Path.size()-1) - OldLinksCount;
	if( NewLinksCount < 2){
		NoNewLinks = true;
	}*/
   return NoNewLinks;
}

bool ServerCapCheck(UniqueList* CurUnique,int Server, int CapNeeded){
	bool NoServerCap = false;
	int CurrentCapOfServer = CurUnique->ServerCapArray[Server];
	if(CurrentCapOfServer < CapNeeded){
		NoServerCap = true;
	}

	return NoServerCap;
}
