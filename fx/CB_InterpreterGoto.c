void CB_Goto( char *SRC, short *StackGotoAdrs, StkWhileDo *StackWhileDo, CurrentStk *CurrentStruct ){
	int c;
	int label;
	int ptr;
	c=SRC[ExecPtr];
	label = CB_CheckLbl( c );
	if ( label < 0 ) { CB_Error(SyntaxERR); return; }	// syntax error
	
	ptr = StackGotoAdrs[label] ;
	if ( ptr == 0 ) {
		if ( Search_Lbl(SRC, c) == 0 ) { CB_Error(UndefinedLabelERR); return; }	// undefined label error
		ExecPtr++;
		StackGotoAdrs[label]=ExecPtr;
	} else  ExecPtr = ptr ;
	
	StackFor->Ptr=0;
	StackWhileDo->WhilePtr=0;
	StackWhileDo->DoPtr=0;
	StackSwitch->Ptr=0;
	CurrentStruct->CNT=0;
}
void CB_Goto( char *SRC, short *StackGotoAdrs, StkWhileDo *StackWhileDo, CurrentStk *CurrentStruct ){
	int c;
	int label;
	int ptr;

	{	c=SRC[ExecPtr];
		label = CB_CheckLbl( c );
		if ( label < 0 ) { CB_Error(SyntaxERR); return; }	// syntax error
	
		ptr = StackGotoAdrs[label] ;
		if ( ptr == 0 ) {
			if ( Search_Lbl(SRC, c) == 0 ) { CB_Error(UndefinedLabelERR); return; }	// undefined label error
			ExecPtr++;
			StackGotoAdrs[label]=ExecPtr;
		} else  ExecPtr = ptr ;
	}

}

void CB_Goto_ReStruct_While( char *SRC, StkWhileDo *StackWhileDo, CurrentStk *CurrentStruct ) {
	int wPtr=ExecPtr;
//	int i;
//	i=CB_EvalInt( SRC );
//	if ( i == 0 ) {		// false
//		if ( Search_WhileEnd(SRC) == 0 ) { CB_Error(WhileWithoutWhileEndERR); return; }  // While without WhileEnd error
//		return ; // exit
//	}
	if ( StackWhileDo->WhilePtr >= StackWhileMax ) { CB_Error(NestingERR); return; }  //  nesting error
	StackWhileDo->WhileAdrs[StackWhileDo->WhilePtr] = wPtr;
	StackWhileDo->WhilePtr++;
	CurrentStruct->TYPE[CurrentStruct->CNT]=TYPE_While_WhileEnd;
	CurrentStruct->CNT++;
}

void CB_Goto_ReStruct_WhileEnd( char *SRC, StkWhileDo *StackWhileDo, CurrentStk *CurrentStruct ) {
	int exitPtr=ExecPtr;
	int i;
	if ( StackWhileDo->WhilePtr <= 0 ) { CB_Error(WhileEndWithoutWhileERR); return; }  // WhileEnd without While error
	StackWhileDo->WhilePtr--;
	CurrentStruct->CNT--;
//	ExecPtr = StackWhileDo->WhileAdrs[StackWhileDo->WhilePtr] ;
//	i=CB_EvalInt( SRC );
//	if ( i == 0 ) {		// false
//		ExecPtr=exitPtr;
//		return ; // exit
//	}
	StackWhileDo->WhilePtr++;
	CurrentStruct->TYPE[CurrentStruct->CNT]=TYPE_While_WhileEnd;
	CurrentStruct->CNT++;
}

void CB_Goto_ReStruct_Do( char *SRC, StkWhileDo *StackWhileDo, CurrentStk *CurrentStruct ) {
	if ( StackWhileDo->DoPtr >= StackDoMax ) { CB_Error(NestingERR); return; }  //  nesting error
	StackWhileDo->DoAdrs[StackWhileDo->DoPtr] = ExecPtr;
	StackWhileDo->DoPtr++;
	CurrentStruct->TYPE[CurrentStruct->CNT]=TYPE_Do_LpWhile;
	CurrentStruct->CNT++;
}

void CB_Goto_ReStruct_LpWhile( char *SRC, StkWhileDo *StackWhileDo, CurrentStk *CurrentStruct ) {
	int i;
	if ( StackWhileDo->DoPtr <= 0 ) { CB_Error(LpWhileWithoutDoERR); return; }  // LpWhile without Do error
	StackWhileDo->DoPtr--;
	CurrentStruct->CNT--;
//	i=CB_EvalInt( SRC );
//	if ( i == 0  ) return ; // exit
	ExecPtr = StackWhileDo->DoAdrs[StackWhileDo->DoPtr] ;				// true
	StackWhileDo->DoPtr++;
	CurrentStruct->TYPE[CurrentStruct->CNT]=TYPE_Do_LpWhile;
	CurrentStruct->CNT++;
}
void CB_Goto_ReStruct_Switch( char *SRC, StkSwitch *StackSwitch, CurrentStk *CurrentStruct ,CchIf *CacheSwitch ) {
	int wPtr;
	int c,i,j,value;
	value=CB_EvalInt( SRC );
	wPtr=ExecPtr;

	for ( i=0; i<CacheSwitch->CNT; i++ ) {
		if ( CacheSwitch->Ptr[i]==ExecPtr ) break; 	// adrs ok
	}
	if ( i >= CacheSwitch->CNT ) {
		CacheSwitch->Ptr[CacheSwitch->CNT]=ExecPtr;
		if ( Search_SwitchEnd(SRC) == 0 ) { CB_Error(SwitchWithoutSwitchEndERR); return; }  // Switch without SwitchEnd error
		if ( CacheSwitch->CNT < IfCntMax ) {
			CacheSwitch->Adrs[CacheSwitch->CNT]=ExecPtr;
			CacheSwitch->CNT++;
		} else CacheSwitch->CNT=0;	// over reset
	} else  ExecPtr = CacheSwitch->Adrs[i];	// Break out adrs set
	
	StackSwitch->EndAdrs[StackSwitch->Ptr] = ExecPtr;	// Break out adrs set
	StackSwitch->Value[StackSwitch->Ptr] = value;
	StackSwitch->flag[StackSwitch->Ptr] = 0;	// case through clear
	
	if ( StackSwitch->Ptr >= StackSwitchMax ) { CB_Error(NestingERR); return; }  //  nesting error
	StackSwitch->Ptr++;
	CurrentStruct->TYPE[CurrentStruct->CNT]=TYPE_Switch_Case;
	CurrentStruct->CNT++;
	ExecPtr=wPtr;
	
}
void CB_Goto_ReStruct_Case( char *SRC, StkSwitch *StackSwitch, CurrentStk *CurrentStruct ) {
	int exitPtr=ExecPtr;
	int c,value;
	if ( StackSwitch->Ptr <= 0 ) { CB_Error(CaseWithoutSwitchERR); return; }  // Case without Switch error
	StackSwitch->Ptr--;

	c=SRC[ExecPtr];
	value=Eval_atoi( SRC, c );
//	c=SRC[ExecPtr];
//	if ( ( c!=0x0D ) && ( c!=':' ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
//	ExecPtr++;

	if ( StackSwitch->flag[StackSwitch->Ptr] == 0 ) {
		if ( value != StackSwitch->Value[StackSwitch->Ptr] ) {		// false
			Search_CaseEnd(SRC);
		} else {	// true
			StackSwitch->flag[StackSwitch->Ptr]=1;	// case through set
		}
	}
	StackSwitch->Ptr++;
	CurrentStruct->CNT--;
	CurrentStruct->TYPE[CurrentStruct->CNT]=TYPE_Switch_Case;
	CurrentStruct->CNT++;
}

void CB_Goto_ReStruct_Default( char *SRC, StkSwitch *StackSwitch, CurrentStk *CurrentStruct ) {
	int exitPtr=ExecPtr;
	int c,i;
	if ( StackSwitch->Ptr <= 0 ) { CB_Error(DefaultWithoutSwitchERR); return; }  // Default without Switch error
	CurrentStruct->CNT--;
	CurrentStruct->TYPE[CurrentStruct->CNT]=TYPE_Switch_Case;
	CurrentStruct->CNT++;
}

void CB_Goto_ReStruct_SwitchEnd( char *SRC, StkSwitch *StackSwitch, CurrentStk *CurrentStruct ) {
	int exitPtr=ExecPtr;
	int c,i;
	if ( StackSwitch->Ptr <= 0 ) { CB_Error(SwitchEndWithoutSwitchERR); return; }  // SwitchEnd without Switch error
	StackSwitch->Ptr--;
	CurrentStruct->CNT--;
}

void CB_Goto_ReStruct_Break( char *SRC, StkFor *StackFor, StkWhileDo *StackWhileDo, StkSwitch *StackSwitch, CurrentStk *CurrentStruct ) {
	
	if ( CurrentStruct->CNT <=0 ) { CB_Error(NotLoopERR); return; }  // Not Loop error
	CurrentStruct->CNT--;
	switch ( CurrentStruct->TYPE[CurrentStruct->CNT] ) {
		case TYPE_For_Next:			// For Next
			if ( Search_Next(SRC) == 0 )     { CB_Error(ForWithoutNextERR); return; }  // For without Next error
			StackFor->Ptr--;
			return ;
		case TYPE_While_WhileEnd:	// While WhileEnd
			if ( Search_WhileEnd(SRC) == 0 ) { CB_Error(WhileWithoutWhileEndERR); return; }  // While without WhileEnd error
			StackWhileDo->WhilePtr--;
			return ;
		case TYPE_Do_LpWhile:		// DO LpWhile
			if ( Search_LpWhile(SRC) == 0 )  { CB_Error(DoWithoutLpWhileERR); return; }  // Do without LpWhile error
			StackWhileDo->DoPtr--;
			return ;
		case TYPE_Switch_Case:		// Switch
			StackSwitch->Ptr--;
			ExecPtr=StackSwitch->EndAdrs[StackSwitch->Ptr];
			return ;
		default:
			break;
	}
}


