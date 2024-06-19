#pragma once

#include <array>
#include <vector>
#include <string>
#include <functional>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <span>
#include <memory>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>
#include <cstdlib>


#include <whb/gfx.h>
#include <gx2/draw.h>
#include <gx2r/buffer.h>
#include <gx2/mem.h>
#include <gx2r/draw.h>
#include <gfd.h>

#include <cafecompiler/CafeGLSLCompiler.h>


// shader utilities
std::string LoadShaderFromFile(const std::string& filepath) {
    std::ifstream shaderFile(filepath);
    std::stringstream shaderStream;

    shaderStream << shaderFile.rdbuf();
    shaderFile.close();

    return shaderStream.str();
}

WHBGfxShaderGroup* GLSL_CompileShader(const char* vsSrc, const char* psSrc)
{
    char infoLog[1024];
    GX2VertexShader* vs = GLSL_CompileVertexShader(vsSrc, infoLog, sizeof(infoLog), GLSL_COMPILER_FLAG_NONE);
    if (!vs) {
        OSReport("Failed to compile vertex shader. Infolog: %s\n", infoLog);
        return NULL;
    }
    GX2PixelShader* ps = GLSL_CompilePixelShader(psSrc, infoLog, sizeof(infoLog), GLSL_COMPILER_FLAG_NONE);
    if (!ps) {
        OSReport("Failed to compile pixel shader. Infolog: %s\n", infoLog);
        return NULL;
    }
    WHBGfxShaderGroup* shaderGroup = (WHBGfxShaderGroup*)malloc(sizeof(WHBGfxShaderGroup));
    memset(shaderGroup, 0, sizeof(*shaderGroup));
    shaderGroup->vertexShader = vs;
    shaderGroup->pixelShader = ps;
    return shaderGroup;
}


