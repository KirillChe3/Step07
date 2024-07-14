#include "StdAfx.h"
#include "Utilities.h"

Acad::ErrorStatus createLayer(const ACHAR* layerName, AcDbObjectId& layerId)
{
    try {
        Acad::ErrorStatus errorStatus;
        AcDbLayerTable* pTable;

        layerId = AcDbObjectId::kNull;
        if ((errorStatus = acdbHostApplicationServices()->workingDatabase()->getLayerTable(pTable, AcDb::kForRead)) == Acad::eOk)
        {
            if ((errorStatus = pTable->getAt(layerName, layerId, Adesk::kFalse)) != Acad::eOk) {
                AcDbLayerTableRecord* pLTRec = new AcDbLayerTableRecord();
                pLTRec->setName(layerName);

                if ((errorStatus = pTable->upgradeOpen()) == Acad::eOk) {
                    errorStatus = pTable->add(layerId, pLTRec);
                    pLTRec->close();
                }
                else
                {
                    delete pLTRec;
                }
            }
            pTable->close();
        }
        return errorStatus;
    }
    catch (...) {
    }
}

Acad::ErrorStatus createBlockRecord(const ACHAR* name)
{
    try {
        AcDbBlockTable* pBlockTable;
        Acad::ErrorStatus errorStatus;

        if ((errorStatus = acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, AcDb::kForRead)) != Acad::eOk) {
            return errorStatus;
        }

        if (pBlockTable->has(name) == Adesk::kTrue) {
            pBlockTable->close();
            return(Acad::eDuplicateKey);
        }

        AcDbBlockTableRecord* pBlockTableRecord = new AcDbBlockTableRecord();

        pBlockTableRecord->setName(name);
        pBlockTableRecord->setOrigin(AcGePoint3d::kOrigin);

        if ((errorStatus = pBlockTable->upgradeOpen()) != Acad::eOk) {
            delete pBlockTableRecord;
            pBlockTable->close();
            return errorStatus;
        }

        if ((errorStatus = pBlockTable->add(pBlockTableRecord)) != Acad::eOk) {
            pBlockTable->close();
            delete pBlockTableRecord;
            return errorStatus;
        }

        pBlockTable->close();

        AcDbCircle* pFace = new AcDbCircle (AcGePoint3d::kOrigin, AcGeVector3d::kZAxis, 1.0);
        AcDbCircle* pLeftEye = new AcDbCircle (AcGePoint3d(0.33, 0.25, 0.0), AcGeVector3d::kZAxis, 0.1);
        AcDbCircle* pRightEye = new AcDbCircle (AcGePoint3d(-0.33, 0.25, 0.0), AcGeVector3d::kZAxis, 0.1);
        AcDbArc* pMouth = new AcDbArc (AcGePoint3d(0, 0.5, 0), 1.0, 3.141592 + (3.141592 * 0.3), 3.141592 + (3.141592 * 0.7));

        pFace->setColorIndex(2);
        pLeftEye->setColorIndex(5);
        pRightEye->setColorIndex(5);
        pMouth->setColorIndex(1);

        if ((errorStatus = pBlockTableRecord->appendAcDbEntity(pFace)) != Acad::eOk) {
            delete pFace;
            delete pLeftEye;
            delete pRightEye;
            delete pMouth;
            pBlockTableRecord->erase();
            pBlockTableRecord->close();
            return errorStatus;
        }
        pFace->close();

        if ((errorStatus = pBlockTableRecord->appendAcDbEntity(pLeftEye)) != Acad::eOk) {
            delete pFace;
            delete pLeftEye;
            delete pRightEye;
            delete pMouth;
            pBlockTableRecord->erase();
            pBlockTableRecord->close();
            return errorStatus;
        }
        pLeftEye->close();

        if ((errorStatus = pBlockTableRecord->appendAcDbEntity(pRightEye)) != Acad::eOk) {
            delete pFace;
            delete pLeftEye;
            delete pRightEye;
            delete pMouth;
            pBlockTableRecord->erase();
            pBlockTableRecord->close();
            return errorStatus;
        }
        pRightEye->close();

        if ((errorStatus = pBlockTableRecord->appendAcDbEntity(pMouth)) != Acad::eOk) {
            delete pFace;
            delete pLeftEye;
            delete pRightEye;
            delete pMouth;
            pBlockTableRecord->erase();
            pBlockTableRecord->close();
            return errorStatus;
        }
        pMouth->close();

        pBlockTableRecord->close();

        return (Acad::eOk);
    }
    catch (...) {
    }
}
