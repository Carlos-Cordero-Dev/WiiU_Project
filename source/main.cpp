#include <gfd.h>
#include <gx2/draw.h>
#include <gx2/shaders.h>
#include <gx2/mem.h>
#include <gx2/registers.h>
#include <gx2r/draw.h>
#include <gx2r/buffer.h>
#include <string.h>
#include <stdio.h>
#include <whb/file.h>
#include <whb/proc.h>
#include <whb/sdcard.h>
#include <whb/gfx.h>

#include <whb/log.h>
#include <whb/log_cafe.h>
#include <whb/log_udp.h>
#include <whb/log_console.h>

#include <coreinit/systeminfo.h>
#include <coreinit/thread.h>
#include <coreinit/time.h>
#include <coreinit/dynload.h>

#include <string>

#include <romfs-wiiu.h>
#include <cafecompiler/CafeGLSLCompiler.h>

#include "cafecompiler/shader_utils.h"
#include "input.h"
#include "logger.h"

static const float sPositionData[] =
{
    1.0f, -1.0f,  0.0f, 1.0f,
    0.0f,  1.0f,  0.0f, 1.0f,
   -1.0f, -1.0f,  1.0f, 1.0f,
};

static const float sColourData[] =
{
   1.0f,  0.0f,  0.0f, 1.0f,
   0.0f,  1.0f,  0.0f, 1.0f,
   0.0f,  0.0f,  1.0f, 1.0f,
};

void Shutdown(GX2RBuffer* positionBuffer, GX2RBuffer* colorBuff)
{
    WHBLogPrintf("Exiting...");
    GX2RDestroyBufferEx(positionBuffer, 0);
    GX2RDestroyBufferEx(colorBuff, 0);
    WHBUnmountSdCard();
    WHBGfxShutdown();
    WHBProcShutdown();
    WHBLogUdpDeinit();

    romfsExit();
}

int main(int argc, char **argv)
{

   GX2RBuffer positionBuffer = { 0 };
   GX2RBuffer colourBuffer = { 0 };
   WHBGfxShaderGroup group = { 0 };
   void *buffer = NULL;
   char *gshFileData = NULL;
   char *sdRootPath = NULL;
   char path[256];

   WHBLogCafeInit();
   WHBLogUdpInit();
   WHBLogPrint("Logging initialised.");
   WHBProcInit();
   WHBGfxInit();

   if (!WHBMountSdCard()) {
       Shutdown(&positionBuffer, &colourBuffer);
       return -1;
   }

   sdRootPath = WHBGetSdCardMountPath();
#if 0
   sdRootPath = "../sdcard";
#endif // 1
   VORP_LOG("root path %s", sdRootPath);

   //gshFileData = WHBReadWholeFile(path, NULL);
   //if (!gshFileData) {
   //   VORP_LOG("WHBReadWholeFile(%s) returned NULL", path);
   //   Shutdown(&positionBuffer, &colourBuffer);
   //   return -1;
   //}

   //if (!WHBGfxLoadGFDShaderGroup(&group, 0, gshFileData)) {

   //   VORP_LOG("WHBGfxLoadGFDShaderGroup returned FALSE");
   //   Shutdown(&positionBuffer, &colourBuffer);
   //   return -1;
   //}

   GLSL_Init();

   char path_vs[256];
   char path_fs[256];


   int res = romfsInit();
   VORP_LOG("Romfs res %d", res);

   sprintf(path_vs, "romfs:/test.vs.glsl");
   sprintf(path_fs, "romfs:/test.fs.glsl");

   VORP_LOG("path_vs %s", path_vs);
   VORP_LOG("path_fs %s", path_fs);

   std::string path_vs_s(path_vs);
   std::string path_fs_s(path_fs);

   std::string s_vertexShader = LoadShaderFromFile(path_vs_s);
   std::string s_fragmentShader = LoadShaderFromFile(path_fs_s);
   VORP_LOG("VS: %s", s_vertexShader.c_str());
   VORP_LOG("\nFS: %s", s_fragmentShader.c_str());

    if (WHBLogConsoleInit()) {
        VORP_LOG("I'm on the WiiU");
        WHBLogConsoleFree();
    }
    else {
        VORP_LOG("I'm on CEMU or another environment");
    }




   std::string errorLog(1024, '\0');
   GX2VertexShader* vertexShader = GLSL_CompileVertexShader(s_vertexShader.c_str(), errorLog.data(), (int)errorLog.size(), GLSL_COMPILER_FLAG_NONE);
   if (!vertexShader) {
       VORP_LOG("Vertex shader compilation failed for triangle example: %s", errorLog.data());
       return;
   }
   GX2PixelShader* pixelShader = GLSL_CompilePixelShader(s_fragmentShader.c_str(), errorLog.data(), (int)errorLog.size(), GLSL_COMPILER_FLAG_NONE);
   if (!pixelShader) {
       VORP_LOG("Pixel shader compilation failed for triangle example: %s", errorLog.data());
       return;
   }

   group.vertexShader = vertexShader;
   group.pixelShader = pixelShader;

   GX2Invalidate(GX2_INVALIDATE_MODE_CPU_SHADER, group.vertexShader->program, group.vertexShader->size);
   GX2Invalidate(GX2_INVALIDATE_MODE_CPU_SHADER, group.pixelShader->program, group.pixelShader->size);

   WHBGfxInitShaderAttribute(&group, "aPosition", 0, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32);
   WHBGfxInitShaderAttribute(&group, "aColor", 1, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32);
   WHBGfxInitFetchShader(&group);

   //free and = null
   //WHBFreeWholeFile(gshFileData);
   //gshFileData = NULL;

   // Set vertex position
   positionBuffer.flags = GX2R_RESOURCE_BIND_VERTEX_BUFFER |
       GX2R_RESOURCE_USAGE_CPU_READ |
       GX2R_RESOURCE_USAGE_CPU_WRITE |
       GX2R_RESOURCE_USAGE_GPU_READ;
   positionBuffer.elemSize = 4 * 4;
   positionBuffer.elemCount = 3;
   GX2RCreateBuffer(&positionBuffer);
   buffer = GX2RLockBufferEx(&positionBuffer, 0);
   memcpy(buffer, sPositionData, positionBuffer.elemSize * positionBuffer.elemCount);
   GX2RUnlockBufferEx(&positionBuffer, 0);

   // Set vertex colour
   colourBuffer.flags = GX2R_RESOURCE_BIND_VERTEX_BUFFER |
       GX2R_RESOURCE_USAGE_CPU_READ |
       GX2R_RESOURCE_USAGE_CPU_WRITE |
       GX2R_RESOURCE_USAGE_GPU_READ;
   colourBuffer.elemSize = 4 * 4;
   colourBuffer.elemCount = 3;
   GX2RCreateBuffer(&colourBuffer);
   buffer = GX2RLockBufferEx(&colourBuffer, 0);
   memcpy(buffer, sColourData, colourBuffer.elemSize * colourBuffer.elemCount);
   GX2RUnlockBufferEx(&colourBuffer, 0);

   VORP_LOG("Begin rendering...");

   InitWiiUGamepad();
   InitWiiController();


   while (WHBProcIsRunning()) {
      // Animate colours...

      ReadInputWiiUGamepad();
      ReadInputWiiControllers();

      //PrintGamepadCompleteData();
      //PrintWiiControllerCompleteData(0);

      float *colours = (float *)GX2RLockBufferEx(&colourBuffer, 0);
      colours[0] = 1.0f;
      colours[1] = colours[1] >= 1.0f ? 0.0f : (colours[1] + 0.01f);
      colours[2] = colours[2] >= 1.0f ? 0.0f : (colours[2] + 0.01f);
      colours[3] = 1.0f;

      colours[4] = colours[4] >= 1.0f ? 0.0f : (colours[4] + 0.01f);
      colours[5] = 1.0f;
      colours[6] = colours[6] >= 1.0f ? 0.0f : (colours[6] + 0.01f);
      colours[7] = 1.0f;

      colours[8] = colours[8] >= 1.0f ? 0.0f : (colours[8] + 0.01f);
      colours[9] = colours[9] >= 1.0f ? 0.0f : (colours[9] + 0.01f);
      colours[10] = 1.0f;
      colours[11] = 1.0f;
      GX2RUnlockBufferEx(&colourBuffer, 0);

      // Render!
      WHBGfxBeginRender();

      WHBGfxBeginRenderTV();
      WHBGfxClearColor(0.0f, 0.0f, 1.0f, 1.0f);
      GX2SetFetchShader(&group.fetchShader);
      GX2SetVertexShader(group.vertexShader);
      GX2SetPixelShader(group.pixelShader);
      GX2RSetAttributeBuffer(&positionBuffer, 0, positionBuffer.elemSize, 0);
      GX2RSetAttributeBuffer(&colourBuffer, 1, colourBuffer.elemSize, 0);
      GX2DrawEx(GX2_PRIMITIVE_MODE_TRIANGLES, 3, 0, 1);
      WHBGfxFinishRenderTV();

      WHBGfxBeginRenderDRC();
      WHBGfxClearColor(1.0f, 0.0f, 1.0f, 1.0f);
      GX2SetFetchShader(&group.fetchShader);
      GX2SetVertexShader(group.vertexShader);
      GX2SetPixelShader(group.pixelShader);
      GX2RSetAttributeBuffer(&positionBuffer, 0, positionBuffer.elemSize, 0);
      GX2RSetAttributeBuffer(&colourBuffer, 1, colourBuffer.elemSize, 0);
      GX2DrawEx(GX2_PRIMITIVE_MODE_TRIANGLES, 3, 0, 1);
      WHBGfxFinishRenderDRC();

      WHBGfxFinishRender();
   }

   Shutdown(&positionBuffer, &colourBuffer);
   return 0;
}

