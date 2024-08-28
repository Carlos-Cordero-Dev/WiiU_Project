
#ifndef SHADER_H_
#define SHADER_H_ 1

#include <string>
#include <memory>

#include <gx2/shaders.h>
#include <whb/gfx.h>

#include "logger.h"
#include "cafecompiler/CafeGLSLCompiler.h"
#include "cafecompiler/shader_utils.h"

std::shared_ptr<WHBGfxShaderGroup> LoadShader(std::string vs_path, std::string fs_path)
{
    std::shared_ptr<WHBGfxShaderGroup> group = std::make_shared<WHBGfxShaderGroup>();

    //sprintf(vs_path, "romfs:/test.vs.glsl");
    //sprintf(fs_path, "romfs:/test.fs.glsl");

    VORP_LOG("vs_path %s", vs_path.c_str());
    VORP_LOG("fs_path %s", fs_path.c_str());

    std::string s_vertexShader = LoadShaderFromFile(vs_path);
    std::string s_fragmentShader = LoadShaderFromFile(fs_path);

    VORP_LOG("VS: %s", s_vertexShader.c_str());
    VORP_LOG("\nFS: %s", s_fragmentShader.c_str());

    //always says cemu idk why
    //if (WHBLogConsoleInit()) {
    //    VORP_LOG("I'm on the WiiU");
    //    WHBLogConsoleFree();
    //}
    //else {
    //    VORP_LOG("I'm on CEMU or another environment");
    //}

    std::string errorLog(1024, '\0');
    GX2VertexShader* vertexShader = GLSL_CompileVertexShader(s_vertexShader.c_str(), errorLog.data(), (int)errorLog.size(), GLSL_COMPILER_FLAG_NONE);
    if (!vertexShader) {
        VORP_LOG("Vertex shader compilation failed for triangle example: %s", errorLog.data());
        return nullptr;
    }
    GX2PixelShader* pixelShader = GLSL_CompilePixelShader(s_fragmentShader.c_str(), errorLog.data(), (int)errorLog.size(), GLSL_COMPILER_FLAG_NONE);
    if (!pixelShader) {
        VORP_LOG("Pixel shader compilation failed for triangle example: %s", errorLog.data());
        return nullptr;
    }

    VORP_LOG("Shaders compiled");

    group->vertexShader = vertexShader;
    group->pixelShader = pixelShader;

    GX2Invalidate(GX2_INVALIDATE_MODE_CPU_SHADER, group->vertexShader->program, group->vertexShader->size);
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU_SHADER, group->pixelShader->program, group->pixelShader->size);


    return group;
}

#endif //SHADER_H_