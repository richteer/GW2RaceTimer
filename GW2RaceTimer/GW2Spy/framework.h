#pragma once

#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーからほとんど使用されていない部分を除外する
#define DllExport __declspec(dllexport)
// Windows ヘッダー ファイル
#include <windows.h>

typedef struct LinkedMem {
    UINT32 uiVersion;
    UINT32 uiTick;
    float fAvatarPosition[3];
    float fAvatarFront[3];
    float fAvatarTop[3];
    WCHAR name[256];
    float fCameraPosition[3];
    float fCameraFront[3];
    float fCameraTop[3];
    WCHAR identity[256];
    UINT32 context_len; // Despite the actual context containing more data, this value is always 48. See "context" section below.
    unsigned char context[256];
    WCHAR description[2048];
} LinkedMeme, *PLinkedMem;

typedef struct MumbleContext {
    unsigned char serverAddress[28]; // contains sockaddr_in or sockaddr_in6
    UINT32 mapId;
    UINT32 mapType;
    UINT32 shardId;
    UINT32 instance;
    UINT32 buildId;
    // Additional data beyond the 48 bytes Mumble uses for identification
    UINT32 uiState; // Bitmask: Bit 1 = IsMapOpen, Bit 2 = IsCompassTopRight, Bit 3 = DoesCompassHaveRotationEnabled
    UINT16 compassWidth; // pixels
    UINT16 compassHeight; // pixels
    float compassRotation; // radians
    float playerX; // continentCoords
    float playerY; // continentCoords
    float mapCenterX; // continentCoords
    float mapCenterY; // continentCoords
    float mapScale;
} MumbleContext, *PMumbleContext;


BOOL OpenMumbleLink();

void CloseMumbleLink();

extern "C" {

    BOOL DllExport GetCharacterPosition(float* pPos);

    BOOL DllExport GetMapId(int* pMapId);
}