//
// Created by moose-home on 8/10/2018.
//

#ifndef INCLUDES_H
#define INCLUDES_H

//
// INCLUDES FOR OGL/SDL
//
#if __ANDROID__
#include <dlfcn.h>
#define USE_GLES
#endif

#ifdef _WIN32
#define USE_GLEW
#endif

#include "SDL.h"

//
// GLES HEADERS
//
#ifdef USE_GLES
#define GL_GLEXT_PROTOTYPES 1
#include "SDL_opengles2.h"
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

// OES_vertex_array_object
#define glGenVertexArrays glGenVertexArraysOES
#define glBindVertexArray glBindVertexArrayOES
#define glDeleteVertexArrays glDeleteVertexArraysOES
#else
#include "GL/glew.h"		// handles loadin of GL extensions
#include "SDL_opengl.h"
#endif

#include <string>
#include <iostream>

//
// ANDROID HEADERS
//
#if __ANDROID__
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>

#define  LOG_TAG    "MOOSE"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#endif

void logMessage(const std::string &msg);
void logSDLError(std::ostream &os, const std::string &msg);

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);
#if __ANDROID__
GLuint LoadShadersAndroid(AAssetManager*, const char * vertex_file_path,const char * fragment_file_path);
#endif
#endif // INCLUDES_H
