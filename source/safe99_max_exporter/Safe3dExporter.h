//************************************************************************** 
// 
// 파일: Safe3dExporter.h
// 
// 설명: Safe3dExporter 클래스 선언
// 
// 작성자: bumpsgoodman
// 
// 생성일: 2023/09/18
// 
// 3ds Max SDK 샘플 코드를 기반으로 작성한 코드
//
//***************************************************************************

#pragma once

#include "max.h"
#include "resource.h"
#include "safe3d.h"
#include "stdmat.h"
#include "decomp.h"
#include "shape.h"
#include "interpik.h"

#include "istdplug.h"

#define CFGFILENAME		_T("SAFE3DEXPORTER.CFG")	// Configuration file

extern ClassDesc* GetSafe3dExpDesc();
extern TCHAR* GetString(int id);
extern HINSTANCE hInstance;

class Safe3dExporter final : public SceneExport
{
public:
    Safe3dExporter() = default;
    Safe3dExporter(const Safe3dExporter&) = delete;
    Safe3dExporter& operator=(const Safe3dExporter&) = delete;
    Safe3dExporter(Safe3dExporter&&) = default;
    Safe3dExporter& operator=(Safe3dExporter&&) = default;
    ~Safe3dExporter();

    // SceneExport 메서드
    int ExtCount();                     // 지원되는 확장자 수
    const TCHAR* Ext(int n);            // 확장자
    const TCHAR* LongDesc();            // 긴 설명
    const TCHAR* ShortDesc();           // 짧은 설명
    const TCHAR* AuthorName();          // 작성자
    const TCHAR* CopyrightMessage();    // 저작권 메시지
    const TCHAR* OtherMessage1();       // Other message #1
    const TCHAR* OtherMessage2();       // Other message #2
    unsigned int Version();             // 버전 번호
    void ShowAbout(HWND hWnd);          // DLL About 박스
    int	DoExport(const TCHAR* name, ExpInterface* ei, Interface* i, BOOL suppressPrompts = FALSE, DWORD options = 0); // Export 실행 함수
    BOOL SupportsOptions(int ext, DWORD options);

private:
    void calcNumTotalNodeRecursion(INode* node);
    void exportNodeRecursion(INode* node);
    void exportGeomObject(INode* node);

private:
    int mNumTotalNode = 0;
    int mNumCurNode = 0;
    Interface* mInterface = nullptr;

    FILE* mStream = nullptr;
};