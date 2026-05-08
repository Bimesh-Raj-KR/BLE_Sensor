/***************************************************************************************
 *
 *  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
 *
 *  By downloading, copying, installing or using the software you agree to this license.
 *  If you do not agree to this license, do not download, install, 
 *  copy or use the software.
 *
 *  Copyright (C) 2014-2025, Happytimesoft Corporation, all rights reserved.
 *
 *  Redistribution and use in binary forms, with or without modification, are permitted.
 *
 *  Unless required by applicable law or agreed to in writing, software distributed 
 *  under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 *  CONDITIONS OF ANY KIND, either express or implied. See the License for the specific
 *  language governing permissions and limitations under the License.
 *
****************************************************************************************/

#ifndef VIDEO_RENDER_SDL_H
#define VIDEO_RENDER_SDL_H

#include "video_render.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>

/***************************************************************************************/

class CSDLVideoRender : public CVideoRender
{
public:
    CSDLVideoRender();
    ~CSDLVideoRender();

    BOOL init(WId hWnd, int w, int h, int videofmt);
    void unInit();

    BOOL render(AVFrame * frame, int mode);
    void setWindowSize(QSize size);
    void setWindowTitle(const char * title);
    
private:
    BOOL reInit(WId hWnd, int w, int h, int videofmt);
    BOOL initTexture();
    BOOL renderTexture(AVFrame * frame, int mode);

private:
    SDL_Window    * m_pWindow;
    SDL_Renderer  * m_pRenderer;
    SDL_Texture   * m_pTexture;
};


#endif



