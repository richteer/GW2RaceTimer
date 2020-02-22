// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include "pch.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

PLinkedMem pLinkedMem = NULL;
HANDLE hMumbleMap = NULL;

BOOL OpenMumbleLink()
{

    BOOL bRet = FALSE;

    if (hMumbleMap == NULL) {
        hMumbleMap = OpenFileMappingW(FILE_MAP_READ, FALSE, L"MumbleLink");
    }

    if (hMumbleMap != NULL) {
        pLinkedMem = (PLinkedMem)MapViewOfFile(hMumbleMap, FILE_MAP_READ, NULL, NULL, sizeof(LinkedMem));

        if (pLinkedMem != NULL) {
            bRet = TRUE;
        };
    }

    return bRet;
}

void CloseMumbleLink()
{
    if (pLinkedMem != NULL) {
        UnmapViewOfFile(pLinkedMem);
        pLinkedMem = NULL;
    }
    if (hMumbleMap != NULL) {
        CloseHandle(hMumbleMap);
        hMumbleMap = NULL;
    }
}

BOOL DllExport GetCharacterPosition(float* pPos)
{
    if (pPos != NULL && pLinkedMem != NULL) {
        pPos[0] = pLinkedMem->fAvatarPosition[0];
        pPos[1] = pLinkedMem->fAvatarPosition[1];
        pPos[2] = pLinkedMem->fAvatarPosition[2];

        return TRUE;
    }

    return FALSE;
}

BOOL DllExport GetMapId(int* pMapId)
{
    if (pMapId != NULL && pLinkedMem != NULL) {
        *pMapId = ((PMumbleContext)pLinkedMem->context)->mapId;
        return TRUE;
    }

    return FALSE;
}
