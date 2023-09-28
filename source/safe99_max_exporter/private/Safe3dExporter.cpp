//************************************************************************** 
// 
// 파일: Safe3dExporter.h
// 
// 설명: Safe3dExporter 클래스 정의
// 
// 작성자: bumpsgoodman
// 
// 생성일: 2023/09/18
// 
// 3ds Max SDK 샘플 코드를 기반으로 작성한 코드
//
//***************************************************************************

#define _CRT_SECURE_NO_WARNINGS

#include "Safe3dExporter.h"

// Class ID. These must be unique and randomly generated!!
// If you use this as a sample project, this is the first thing
// you should change!
#define SAFE3DEXP_CLASS_ID	Class_ID(0x4f3d6ef1, 0x39dc4eb0)

HINSTANCE hInstance;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, ULONG fdwReason, LPVOID lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        MaxSDK::Util::UseLanguagePackLocale();
        hInstance = hinstDLL;
        DisableThreadLibraryCalls(hInstance);
    }

    return (TRUE);
}

__declspec(dllexport) const TCHAR* LibDescription()
{
    return GetString(IDS_LIBDESCRIPTION);
}

/// MUST CHANGE THIS NUMBER WHEN ADD NEW CLASS
__declspec(dllexport) int LibNumberClasses()
{
    return 1;
}

__declspec(dllexport) ClassDesc* LibClassDesc(int i)
{
    switch (i)
    {
    case 0: return GetSafe3dExpDesc();
    default: return 0;
    }
}

__declspec(dllexport) ULONG LibVersion()
{
    return VERSION_3DSMAX;
}

// Let the plug-in register itself for deferred loading
__declspec(dllexport) ULONG CanAutoDefer()
{
    return 1;
}

class Safe3dExpClassDesc :public ClassDesc {
public:
    int	IsPublic() { return 1; }
    void* Create(BOOL loading = FALSE) { return new Safe3dExporter; }
    const TCHAR* ClassName() { return GetString(IDS_SAFE3DEXP); }
    const TCHAR* NonLocalizedClassName() { return _T("Safe3dExporter"); }
    SClass_ID SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
    Class_ID ClassID() { return SAFE3DEXP_CLASS_ID; }
    const TCHAR* Category() { return GetString(IDS_CATEGORY); }
};

static Safe3dExpClassDesc Safe3dExpDesc;

ClassDesc* GetSafe3dExpDesc()
{
    return &Safe3dExpDesc;
}

Safe3dExporter::~Safe3dExporter()
{
}

int Safe3dExporter::ExtCount()
{
    return 1;
}

const TCHAR* Safe3dExporter::Ext(int n)
{
    switch (n)
    {
    case 0:
        return _T("safe3d");
    }

    return _T("");
}

const TCHAR* Safe3dExporter::LongDesc()
{
    return GetString(IDS_LONGDESC);
}

const TCHAR* Safe3dExporter::ShortDesc()
{
    return GetString(IDS_SHORTDESC);
}

const TCHAR* Safe3dExporter::AuthorName()
{
    return _T("bumpsgoodman");
}

const TCHAR* Safe3dExporter::CopyrightMessage()
{
    return GetString(IDS_COPYRIGHT);
}

const TCHAR* Safe3dExporter::OtherMessage1()
{
    return _T("");
}

const TCHAR* Safe3dExporter::OtherMessage2()
{
    return _T("");
}

unsigned int Safe3dExporter::Version()
{
    return 100;
}

static INT_PTR CALLBACK AboutBoxDlgProc(HWND hWnd, UINT msg,
                                        WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_INITDIALOG:
        CenterWindow(hWnd, GetParent(hWnd));
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK:
            EndDialog(hWnd, 1);
            break;
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

void Safe3dExporter::ShowAbout(HWND hWnd)
{
    DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, AboutBoxDlgProc, 0);
}

int	Safe3dExporter::DoExport(const TCHAR* name, ExpInterface* ei, Interface* i, BOOL suppressPrompts, DWORD options)
{
    mInterface = i;

    mStream = _wfopen(name, L"wb");
    if (mStream == nullptr)
    {
        return IMPEXP_FAIL;
    }

    // 진행 표시줄 시작
    mInterface->ProgressStart(GetString(IDS_PROGRESS_MSG), TRUE, nullptr, nullptr);

    mNumCurNode = 0;
    mNumTotalNode = 0;

    // 총 노드 수 계산
    int numChildren = mInterface->GetRootNode()->NumberOfChildren();
    for (int idx = 0; idx < numChildren; idx++) {
        if (i->GetCancel())
        {
            break;
        }

        calcNumTotalNodeRecursion(mInterface->GetRootNode()->GetChildNode(idx));
    }

    for (int idx = 0; idx < numChildren; idx++)
    {
        if (mInterface->GetCancel())
        {
            break;
        }

        exportNodeRecursion(mInterface->GetRootNode()->GetChildNode(idx));
    }

    // 진행 표시줄 마침
    mInterface->ProgressEnd();

    fclose(mStream);

    return IMPEXP_SUCCESS;
}

BOOL Safe3dExporter::SupportsOptions(int ext, DWORD options)
{
    return(options == SCENE_EXPORT_SELECTED) ? TRUE : FALSE;
}

TCHAR* GetString(int id)
{
    static TCHAR buf[256];

    if (hInstance)
    {
        return LoadString(hInstance, id, buf, _countof(buf)) ? buf : NULL;
    }

    return NULL;
}

void Safe3dExporter::calcNumTotalNodeRecursion(INode* node)
{
    ++mNumTotalNode;

    for (int i = 0; i < node->NumberOfChildren(); ++i)
    {
        calcNumTotalNodeRecursion(node->GetChildNode(i));
    }
}

void Safe3dExporter::exportNodeRecursion(INode* node)
{
    ++mNumCurNode;
    mInterface->ProgressUpdate((int)((float)mNumCurNode / mNumTotalNode * 100.0f));

    if (mInterface->GetCancel())
    {
        return;
    }

    // 객체에 대한 모든 정보..?
    ObjectState os = node->EvalWorldState(0);
    if (os.obj != nullptr)
    {
        switch (os.obj->SuperClassID())
        {
        case GEOMOBJECT_CLASS_ID:
            exportGeomObject(node);
            break;
        default:
            break;
        }
    }

    for (int i = 0; i < node->NumberOfChildren(); ++i)
    {
        exportNodeRecursion(node->GetChildNode(i));
    }
}

void Safe3dExporter::exportGeomObject(INode* node)
{
    ObjectState os = node->EvalWorldState(0);
    if (os.obj == nullptr)
    {
        return;
    }

    bool bDeleteTri = false;
    Object* obj = os.obj;
    TriObject* tri = nullptr;
    if (obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)))
    {
        tri = (TriObject*)obj->ConvertToType(0, Class_ID(TRIOBJ_CLASS_ID, 0));

        // Note that the TriObject should only be deleted
        // if the pointer to it is not equal to the object
        // pointer that called ConvertToType()
        if (obj != tri)
        {
            bDeleteTri = true;
        }
    }

    Mesh* mesh = &tri->GetMesh();

    safe3d_t safe3d = { 0, };
    strcpy(safe3d.magic, "safe3d");

    safe3d.num_vertices = mesh->getNumVerts();
    safe3d.num_indices = mesh->getNumFaces();

    fwrite(&safe3d, sizeof(safe3d_t), 1, mStream);

    for (int i = 0; i < safe3d.num_vertices; ++i)
    {
        Point3 p = mesh->verts[i];
        vector3_t v = { p.x, p.y, p.z };
        fwrite(&v, sizeof(vector3_t), 1, mStream);
    }

    for (int i = 0; i < safe3d.num_indices; ++i)
    {
        Point3 p = mesh->verts[i];
        vector3_int v = { (int)mesh->faces[i].v[2], (int)mesh->faces[i].v[1], (int)mesh->faces[i].v[0] };
        fwrite(&v, sizeof(vector3_int), 1, mStream);
    }

    if (bDeleteTri)
    {
        delete tri;
    }
}