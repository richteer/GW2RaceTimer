#include <Windows.h>

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
} LinkedMeme, * PLinkedMem;

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
} MumbleContext, * PMumbleContext;


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
            PMumbleContext temp = (PMumbleContext)pLinkedMem->context;
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

int main(int argc, char* argv[]) {

    if (OpenMumbleLink() != FALSE) {
        CloseMumbleLink();
    }
    return 0;
}