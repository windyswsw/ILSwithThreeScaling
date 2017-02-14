/*
 * UpdateLists.h
 *
 *  Created on: Oct 19, 2015
 *      Author: u99319
 */

void Update_LinksMatrix(FullSol* InitialFullSolution, SinglePath path, int BW, string mode){
	if(path.Path.size() > 1){
		if(mode == "D"){
			for(int l=0; l<path.Path.size()-1;l++){
				int newBW = InitialFullSolution->CurLists.LinksLists.LinksBW[path.Path[l]][path.Path[l+1]] - BW;

				InitialFullSolution->CurLists.LinksLists.LinksBW[path.Path[l]][path.Path[l+1]] = newBW;
				InitialFullSolution->CurLists.LinksLists.LinksBW[path.Path[l+1]][path.Path[l]] = newBW;

				InitialFullSolution->CurLists.LinksLists.LinksUse[path.Path[l]][path.Path[l+1]] = ++ InitialFullSolution->CurLists.LinksLists.LinksUse[path.Path[l]][path.Path[l+1]];
				InitialFullSolution->CurLists.LinksLists.LinksUse[path.Path[l+1]][path.Path[l]] = ++ InitialFullSolution->CurLists.LinksLists.LinksUse[path.Path[l+1]][path.Path[l]];
			}
		}

		if(mode == "A"){
			for(int l=0; l<path.Path.size()-1;l++){
				int newBW = InitialFullSolution->CurLists.LinksLists.LinksBW[path.Path[l]][path.Path[l+1]] + BW;

				InitialFullSolution->CurLists.LinksLists.LinksBW[path.Path[l]][path.Path[l+1]] = newBW;
				InitialFullSolution->CurLists.LinksLists.LinksBW[path.Path[l+1]][path.Path[l]] = newBW;

				InitialFullSolution->CurLists.LinksLists.LinksUse[path.Path[l]][path.Path[l+1]] = -- InitialFullSolution->CurLists.LinksLists.LinksUse[path.Path[l]][path.Path[l+1]];
				InitialFullSolution->CurLists.LinksLists.LinksUse[path.Path[l+1]][path.Path[l]] = -- InitialFullSolution->CurLists.LinksLists.LinksUse[path.Path[l+1]][path.Path[l]];
			}
		}
	}
}

void Update_LinksMatrixForTraffic(FullSol* InitialFullSolution, SinglePath path, int BW, string mode){
	if(path.Path.size() > 1){
		if(mode == "D"){
			for(int l=0; l<path.Path.size()-1;l++){
				int newBW = InitialFullSolution->CurLists.LinksLists.LinksBW[path.Path[l]][path.Path[l+1]] - BW;

				InitialFullSolution->CurLists.LinksLists.LinksBW[path.Path[l]][path.Path[l+1]] = newBW;
				InitialFullSolution->CurLists.LinksLists.LinksBW[path.Path[l+1]][path.Path[l]] = newBW;
			}
		}

		if(mode == "A"){
			for(int l=0; l<path.Path.size()-1;l++){
				int newBW = InitialFullSolution->CurLists.LinksLists.LinksBW[path.Path[l]][path.Path[l+1]] + BW;

				InitialFullSolution->CurLists.LinksLists.LinksBW[path.Path[l]][path.Path[l+1]] = newBW;
				InitialFullSolution->CurLists.LinksLists.LinksBW[path.Path[l+1]][path.Path[l]] = newBW;
			}
		}
	}
}
