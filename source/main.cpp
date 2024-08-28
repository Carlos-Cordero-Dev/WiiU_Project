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
#include <memory>

#include <romfs-wiiu.h>
#include <tga/TGATexture.h>

#include "shader.h"
#include "input.h"
#include "logger.h"
#include "model.h"
#include "jobs.h"
#include "timer.h"


void Shutdown(GX2RBuffer* positionBuffer, GX2RBuffer* colorBuff)
{
    WHBLogPrintf("Exiting...");
    GX2RDestroyBufferEx(positionBuffer, 0);
    GX2RDestroyBufferEx(colorBuff, 0);
    WHBUnmountSdCard();
    WHBGfxShutdown();
    WHBProcShutdown();
    WHBLogUdpDeinit();
    GLSL_Shutdown();
    romfsExit();
}

int main(int argc, char **argv)
{

   GX2RBuffer positionBuffer = { 0 };
   GX2RBuffer colourBuffer = { 0 };
   std::shared_ptr<WHBGfxShaderGroup> group;
   void *buffer = NULL;
   char *gshFileData = NULL;
   char *sdRootPath = NULL;
   char path[256];

   WHBLogCafeInit();
   WHBLogUdpInit();
   WHBLogPrint("Logging initialised.");
   WHBProcInit();
   WHBGfxInit();
   romfsInit();

   std::unique_ptr<JobsManager> jb = std::make_unique<JobsManager>();


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

   LoadFBX("romfs:/models/primitives/cube2.obj");
   std::shared_ptr<ObjInfo> cube = LoadObjFromFile("romfs:/models/primitives/cube.obj");

   group = LoadShader("romfs:/test.vs.glsl", "romfs:/test.fs.glsl");

   WHBGfxInitShaderAttribute(group.get(), "aPosition", 0, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32);
   WHBGfxInitShaderAttribute(group.get(), "aColor", 1, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32);
   WHBGfxInitFetchShader(group.get());


   //free and = null
   //WHBFreeWholeFile(gshFileData);
   //gshFileData = NULL;

   const float sPositionData[] =
   {
       1.0f, -1.0f,  0.0f, 1.0f,
       0.0f,  1.0f,  0.0f, 1.0f,
      -1.0f, -1.0f,  1.0f, 1.0f,
   };

   const float sColourData[] =
   {
      1.0f,  0.0f,  0.0f, 1.0f,
      0.0f,  1.0f,  0.0f, 1.0f,
      0.0f,  0.0f,  1.0f, 1.0f,
   };

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

   //Texture shader =============================================================================0

   std::shared_ptr<WHBGfxShaderGroup> basicTextureShaderGroup = LoadShader("romfs:/texture.vs.glsl", "romfs:/texture.fs.glsl");
   WHBGfxInitShaderAttribute(basicTextureShaderGroup.get(), "aPos", 0, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32);
   WHBGfxInitShaderAttribute(basicTextureShaderGroup.get(), "aTexCoord", 1, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32);
   WHBGfxInitFetchShader(basicTextureShaderGroup.get());

   GX2RBuffer s_positionBuffer = {};
   GX2RBuffer s_texCoordBuffer = {};
   GX2Texture* s_texture = nullptr;
   GX2Sampler s_sampler = {};

   const float s_positionData[] =
   {
       -1.0f, -1.0f,  0.0f, 1.0f,
       1.0f,  -1.0f,  0.0f, 1.0f,
       1.0f, 1.0f,  0.0f, 1.0f,
       -1.0f, 1.0f,  0.0f, 1.0f,
   };

   const float s_texCoords[] = {
       0.0f, 1.0f,
       1.0f, 1.0f,
       1.0f, 0.0f,
       0.0f, 0.0f,
   };

   // upload vertex position
   s_positionBuffer.flags = GX2R_RESOURCE_BIND_VERTEX_BUFFER | GX2R_RESOURCE_USAGE_CPU_READ | GX2R_RESOURCE_USAGE_CPU_WRITE | GX2R_RESOURCE_USAGE_GPU_READ;
   s_positionBuffer.elemSize = 4 * sizeof(float);
   s_positionBuffer.elemCount = 4;
   GX2RCreateBuffer(&s_positionBuffer);
   void* posUploadBuffer = GX2RLockBufferEx(&s_positionBuffer, GX2R_RESOURCE_BIND_NONE);
   memcpy(posUploadBuffer, s_positionData, s_positionBuffer.elemSize * s_positionBuffer.elemCount);
   GX2RUnlockBufferEx(&s_positionBuffer, GX2R_RESOURCE_BIND_NONE);

   // upload texture coords
   s_texCoordBuffer.flags = GX2R_RESOURCE_BIND_VERTEX_BUFFER | GX2R_RESOURCE_USAGE_CPU_READ | GX2R_RESOURCE_USAGE_CPU_WRITE | GX2R_RESOURCE_USAGE_GPU_READ;
   s_texCoordBuffer.elemSize = 2 * sizeof(float);
   s_texCoordBuffer.elemCount = 4;
   GX2RCreateBuffer(&s_texCoordBuffer);
   void* coordsUploadBuffer = GX2RLockBufferEx(&s_texCoordBuffer, GX2R_RESOURCE_BIND_NONE);
   memcpy(coordsUploadBuffer, s_texCoords, s_texCoordBuffer.elemSize * s_texCoordBuffer.elemCount);
   GX2RUnlockBufferEx(&s_texCoordBuffer, GX2R_RESOURCE_BIND_NONE);

   // upload texture
   std::string texPath = "romfs:/sonic.tga";
   std::ifstream fs(texPath, std::ios::in | std::ios::binary);
   if (!fs.is_open())
   {
       VORP_LOG("Cant open TGA file");
       return;
   }
   std::vector<uint8_t> data((std::istreambuf_iterator<char>(fs)), std::istreambuf_iterator<char>());
   s_texture = TGA_LoadTexture((uint8_t*)data.data(), data.size());
   if (s_texture == nullptr)
   {
       VORP_LOG("Texture is null");
       return;
   }
   else VORP_LOG("Texture %s loaded", texPath.c_str());

   GX2Sampler sampler;
   GX2InitSampler(&sampler, GX2_TEX_CLAMP_MODE_CLAMP, GX2_TEX_XY_FILTER_MODE_LINEAR);


   VORP_LOG("Begin rendering...");

   InitWiiUGamepad();
   InitWiiController();

   Timer timer = Timer();

   while (WHBProcIsRunning()) {
      // Animate colours...

      ReadInputWiiUGamepad();
      ReadInputWiiControllers();

      timer.step();
      VORP_LOG("gametime %f fps %d deltatime %f",(float)(timer.get_time()), (int)timer.fps, (float)timer.deltaTime);


      //PrintGamepadCompleteData();
      //PrintWiiControllerCompleteData(0);

      //float *colours = (float *)GX2RLockBufferEx(&colourBuffer, 0);
      //colours[0] = 1.0f;
      //colours[1] = colours[1] >= 1.0f ? 0.0f : (colours[1] + 0.01f);
      //colours[2] = colours[2] >= 1.0f ? 0.0f : (colours[2] + 0.01f);
      //colours[3] = 1.0f;

      //colours[4] = colours[4] >= 1.0f ? 0.0f : (colours[4] + 0.01f);
      //colours[5] = 1.0f;
      //colours[6] = colours[6] >= 1.0f ? 0.0f : (colours[6] + 0.01f);
      //colours[7] = 1.0f;

      //colours[8] = colours[8] >= 1.0f ? 0.0f : (colours[8] + 0.01f);
      //colours[9] = colours[9] >= 1.0f ? 0.0f : (colours[9] + 0.01f);
      //colours[10] = 1.0f;
      //colours[11] = 1.0f;
      //GX2RUnlockBufferEx(&colourBuffer, 0);

      // Render!
      WHBGfxBeginRender();

      WHBGfxBeginRenderTV();
      WHBGfxClearColor(0.0f, 0.0f, 1.0f, 1.0f);

      //triangle

      //GX2SetFetchShader(&group->fetchShader);
      //GX2SetVertexShader(group->vertexShader);
      //GX2SetPixelShader(group->pixelShader);
      //GX2RSetAttributeBuffer(&s_positionBuffer, 0, s_positionBuffer.elemSize, 0);
      //GX2RSetAttributeBuffer(&colourBuffer, 1, colourBuffer.elemSize, 0);
      //GX2DrawEx(GX2_PRIMITIVE_MODE_TRIANGLES, 3, 0, 1);

      //Texture
      
      GX2SetFetchShader(&basicTextureShaderGroup->fetchShader);
      GX2SetVertexShader(basicTextureShaderGroup->vertexShader);
      GX2SetPixelShader(basicTextureShaderGroup->pixelShader);
      GX2RSetAttributeBuffer(&s_positionBuffer, 0, s_positionBuffer.elemSize, 0);
      GX2RSetAttributeBuffer(&s_texCoordBuffer, 1, s_texCoordBuffer.elemSize, 0);
      //GX2SetShaderMode(GX2_SHADER_MODE_UNIFORM_BLOCK);

      GX2SetPixelTexture(s_texture, basicTextureShaderGroup->pixelShader->samplerVars[0].location);
      GX2SetPixelSampler(&s_sampler, basicTextureShaderGroup->pixelShader->samplerVars[0].location);

      GX2DrawEx(GX2_PRIMITIVE_MODE_QUADS, 4, 0, 1);

      WHBGfxFinishRenderTV();

      WHBGfxBeginRenderDRC();
      WHBGfxClearColor(1.0f, 0.0f, 1.0f, 1.0f);

      //triangle

      //GX2SetFetchShader(&group->fetchShader);
      //GX2SetVertexShader(group->vertexShader);
      //GX2SetPixelShader(group->pixelShader);
      //GX2RSetAttributeBuffer(&positionBuffer, 0, positionBuffer.elemSize, 0);
      //GX2RSetAttributeBuffer(&colourBuffer, 1, colourBuffer.elemSize, 0);
      //GX2DrawEx(GX2_PRIMITIVE_MODE_TRIANGLES, 3, 0, 1);

      //Texture

      GX2SetFetchShader(&basicTextureShaderGroup->fetchShader);
      GX2SetVertexShader(basicTextureShaderGroup->vertexShader);
      GX2SetPixelShader(basicTextureShaderGroup->pixelShader);
      GX2RSetAttributeBuffer(&s_positionBuffer, 0, s_positionBuffer.elemSize, 0);
      GX2RSetAttributeBuffer(&s_texCoordBuffer, 1, s_texCoordBuffer.elemSize, 0);
      //GX2SetShaderMode(GX2_SHADER_MODE_UNIFORM_BLOCK);

      GX2SetPixelTexture(s_texture, basicTextureShaderGroup->pixelShader->samplerVars[0].location);
      GX2SetPixelSampler(&s_sampler, basicTextureShaderGroup->pixelShader->samplerVars[0].location);

      GX2DrawEx(GX2_PRIMITIVE_MODE_QUADS, 4, 0, 1);

      WHBGfxFinishRenderDRC();

      WHBGfxFinishRender();
   }

   Shutdown(&positionBuffer, &colourBuffer);
   return 0;
}

