// (C) Copyright 2002-2012 by Autodesk, Inc. 
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
//----- DocData.cpp : Implementation file
//-----------------------------------------------------------------------------
#include "StdAfx.h"

AcApDataManager<CDocData> DocVars;

//-----------------------------------------------------------------------------
//----- The one and only document manager object. You can use the DocVars object to retrieve
//----- document specific data throughout your application

//-----------------------------------------------------------------------------

CDocData::CDocData() : m_editCommand{ false }, m_doRepositioning{ false }
{
	attachEmployeeReactorToAllEmployee(true);
}

CDocData::CDocData(const CDocData& data) : m_editCommand{ false }, m_doRepositioning{ false }
{	
}

CDocData::~CDocData () {
}

Acad::ErrorStatus attachEmployeeReactorToAllEmployee(bool attach)
{
	try
	{
		Acad::ErrorStatus errSt;
		AcDbBlockTable* pBlockTable;
		if ((errSt = acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, AcDb::kForRead)) != Acad::eOk) return errSt;

		AcDbBlockTableRecord* pModelSpace;
		if ((errSt = pBlockTable->getAt(ACDB_MODEL_SPACE, pModelSpace, AcDb::kForRead)) != Acad::eOk) {
			pBlockTable->close();
			return errSt;
		}

		pBlockTable->close();

		AcDbBlockTableRecordIterator* pIter;
		if ((errSt = pModelSpace->newIterator(pIter)) != Acad::eOk) {
			pModelSpace->close();
			return errSt;
		}

		for (; !pIter->done(); pIter->step()) {
			AcDbEntity* pEntity;
			errSt = pIter->getEntity(pEntity, AcDb::kForRead);

			AcDbBlockReference* pInsert = AcDbBlockReference::cast(pEntity);

			if (!pInsert) {
				pEntity->close();
				continue;
			}

			AcDbObjectId blockId = pInsert->blockTableRecord();
			AcDbBlockTableRecord* pBlockTableRecord;

			if (acdbOpenAcDbObject((AcDbObject*&)pBlockTableRecord, blockId, AcDb::kForRead) != Acad::eOk) {
				acutPrintf(L"\nCannot open block table record");
				pEntity->close();
				break;
			}

			const TCHAR* blockName;
			pBlockTableRecord->getName(blockName);
			pBlockTableRecord->close();

			if (_tcscmp(blockName, L"EMPLOYEE")) {
				pEntity->close();
				continue;
			}

			if (attach) {
				pEntity->addReactor(pEmployeeReactor.get());
			}
			else
			{
				pEntity->removeReactor(pEmployeeReactor.get());
			}
			pEntity->close();
		}
		delete pIter;
		pModelSpace->close();
		return (Acad::eOk);
	}
	catch (...)
	{
	}
}

void detachAllEmployeeReactors()
{
	try
	{
		AcApDocumentIterator* pIter = acDocManager->newAcApDocumentIterator();
		if (!pIter) return;

		AcApDocument* pOldDocument = acDocManager->curDocument();

		while (!pIter->done())
		{
			AcApDocument* pDoc = pIter->document();
			if (pDoc->lockMode() == AcAp::kNone) {
				if (acDocManager->setCurDocument(pDoc, AcAp::kAutoWrite, Adesk::kFalse) == Acad::eOk) {
					attachEmployeeReactorToAllEmployee(false);
					acDocManager->unlockDocument(pDoc);
				}
			}
			else
			{
				acDocManager->setCurDocument(pDoc);
				attachEmployeeReactorToAllEmployee(false);
			}
			pIter->step();
		}
		delete pIter;

		acDocManager->setCurDocument(pOldDocument, AcAp::kNone, Adesk::kFalse);
	}
	catch (...)
	{
	}
}