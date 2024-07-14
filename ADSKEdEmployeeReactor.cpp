// (C) Copyright 2002-2007 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//----- ADSKEdEmployeeReactor.cpp : Implementation of ADSKEdEmployeeReactor
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ADSKEdEmployeeReactor.h"

//-----------------------------------------------------------------------------
ADSKEdEmployeeReactor::ADSKEdEmployeeReactor (const bool autoInitAndRelease) : AcEditorReactor(), mbAutoInitAndRelease(autoInitAndRelease) {
	if ( autoInitAndRelease ) {
		if ( acedEditor )
			acedEditor->addReactor (this) ;
		else
			mbAutoInitAndRelease =false ;
	}
}

//-----------------------------------------------------------------------------
ADSKEdEmployeeReactor::~ADSKEdEmployeeReactor () {
	Detach () ;
}

//-----------------------------------------------------------------------------
void ADSKEdEmployeeReactor::Attach () {
	Detach () ;
	if ( !mbAutoInitAndRelease ) {
		if ( acedEditor ) {
			acedEditor->addReactor (this) ;
			mbAutoInitAndRelease =true ;
		}
	}
}

void ADSKEdEmployeeReactor::Detach () {
	if ( mbAutoInitAndRelease ) {
		if ( acedEditor ) {
			acedEditor->removeReactor (this) ;
			mbAutoInitAndRelease =false ;
		}
	}
}

void ADSKEdEmployeeReactor::commandWillStart(const ACHAR* cmdStr) {
	try
	{
		if (_tcscmp(cmdStr, L"MOVE") && _tcscmp(cmdStr, L"ROTATE")) return;

		DocVars.docData().m_editCommand = true;
		DocVars.docData().m_doRepositioning = false;
		DocVars.docData().m_changedObjects.setLogicalLength(0);
		DocVars.docData().m_employeePositions.setLogicalLength(0);
	}
	catch (...)
	{
	}
}

void ADSKEdEmployeeReactor::commandEnded(const ACHAR* cmdStr) {
	try
	{
		if (!DocVars.docData().m_editCommand) return;

		DocVars.docData().m_editCommand = false;
		DocVars.docData().m_doRepositioning = true;

		AcDbBlockReference* pInsert;
		for (int i = 0; i < DocVars.docData().m_changedObjects.length(); ++i) {
			if (acdbOpenObject(pInsert, DocVars.docData().m_changedObjects.at(i), AcDb::kForWrite) == Acad::eOk) {
				AcGePoint3d currPos = pInsert->position();
				AcGePoint3d prevPos = DocVars.docData().m_employeePositions.at(i);
				if (currPos != prevPos) {
					pInsert->setPosition(prevPos);
					acutPrintf(L"\nEMPLOYEE's reseted to original position");
				}
				pInsert->close();
			}
		}
	}
	catch (...)
	{
	}
}