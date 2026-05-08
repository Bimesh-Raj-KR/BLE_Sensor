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

#include "sys_inc.h"
#include "video_render_sdl.h"
#include "lock.h"
extern "C"
{
#include <libavutil/imgutils.h>
}


/***************************************************************************************/

static void * g_sdl_mutex = sys_os_create_mutex();

/***************************************************************************************/

CSDLVideoRender::CSDLVideoRender()
: CVideoRender()
, m_pWindow(NULL)
, m_pRenderer(NULL)
, m_pTexture(NULL)
{
}

CSDLVideoRender::~CSDLVideoRender()
{
    unInit();
}

BOOL CSDLVideoRender::init(WId hWnd, int w, int h, int videofmt)
{
    CVideoRender::init(hWnd, w, h, videofmt);

    CLock lock(g_sdl_mutex);
    
    m_bInited = initTexture();
    
    return m_bInited;
}

void CSDLVideoRender::unInit()
{
    CLock lock(g_sdl_mutex);
    
    if (m_pTexture)
    {
        SDL_DestroyTexture(m_pTexture);
        m_pTexture = NULL;
    }
    
    if (m_pRenderer)
    {
        SDL_DestroyRenderer(m_pRenderer);
        m_pRenderer = NULL;
    }

    if (m_pWindow)
    {
        SDL_DestroyWindow(m_pWindow);
        m_pWindow = NULL;
    }

    m_bInited = FALSE;
}

BOOL CSDLVideoRender::reInit(WId hWnd, int w, int h, int videofmt)
{
    BOOL ret;
#ifdef IOS    
    char title[512] = {'\0'};

    if (m_pWindow)
    {
        const char * p_title = SDL_GetWindowTitle(m_pWindow);
        if (p_title)
        {
            strncpy(title, p_title, sizeof(title)-1);
        }
    }
#endif

    unInit();

    ret = init(m_hWnd, w, h, m_nVideoFmt);

#ifdef IOS
    if (ret && m_pWindow)
    {
        SDL_SetWindowTitle(m_pWindow, title);
    }
#endif

    return ret;
}

BOOL CSDLVideoRender::render(AVFrame * frame, int mode)
{
    if (!m_bInited || NULL == frame)
    {
        return FALSE;
    }
    
    return renderTexture(frame, mode);
}

BOOL CSDLVideoRender::initTexture()
{
#ifdef IOS
    // On Mac platform, SDL does not support rendering videos on child windows 
    //  and must be presented on a separate top-level window
    m_pWindow = SDL_CreateWindow("", 0, 0, 400, 300, SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE);
#else
    m_pWindow = SDL_CreateWindowFrom((void *)m_hWnd);
#endif
    if (NULL == m_pWindow)
    {
        return FALSE;
    }

    m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, 0);
    if (NULL == m_pRenderer)
    {
        return FALSE;
    }

    m_pTexture = SDL_CreateTexture(m_pRenderer, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING, m_nVideoWidth, m_nVideoHeight);
    if (NULL == m_pTexture)
    {
        return FALSE;
    }

    SDL_ShowWindow(m_pWindow);
    
    return TRUE;
}

BOOL CSDLVideoRender::renderTexture(AVFrame * frame, int mode)
{
    int pitch;
    uint8 * ptr;
    uint8 * pixels;
    AVFrame dstFrame;
    
    if (m_nVideoWidth != frame->width || m_nVideoHeight != frame->height)
    {
        if (!reInit(m_hWnd, frame->width, frame->height, m_nVideoFmt))
        {
            return TRUE;
        }

        m_nVideoWidth = frame->width;
        m_nVideoHeight = frame->height;
    }

    memset(&dstFrame, 0, sizeof(AVFrame));
    
    SDL_LockTexture(m_pTexture, NULL, (void **)&pixels, &pitch);

    av_image_fill_arrays(dstFrame.data, dstFrame.linesize, pixels, AV_PIX_FMT_YUV420P, m_nVideoWidth, m_nVideoHeight, 1);
    
    // swap UV plane
    ptr = dstFrame.data[1]; 
    dstFrame.data[1] = dstFrame.data[2]; 
    dstFrame.data[2] = ptr;

    av_image_copy(dstFrame.data, dstFrame.linesize, (const uint8_t**)frame->data, frame->linesize, AV_PIX_FMT_YUV420P, m_nVideoWidth, m_nVideoHeight);

    SDL_UnlockTexture(m_pTexture);
    
    SDL_RenderClear(m_pRenderer);

    int w, h;
    SDL_Rect rect;
    
    SDL_GetRendererOutputSize(m_pRenderer, &w, &h);
    
    if (mode == RENDER_MODE_KEEP)
    {
        double dw = w / (double) m_nVideoWidth;
        double dh = h / (double) m_nVideoHeight;
        double rate = (dw > dh) ? dh : dw;

        int rw = (int)(m_nVideoWidth * rate);
        int rh = (int)(m_nVideoHeight * rate);

        rect.x = (w - rw) / 2;
        rect.y = (h - rh) / 2;
        rect.w = rw;
        rect.h = rh;
    }
    else
    {
        rect.x = 0;
        rect.y = 0;
        rect.w = w;
        rect.h = h;
    }
    
    SDL_RenderCopy(m_pRenderer, m_pTexture, NULL, &rect);
    
    SDL_RenderPresent(m_pRenderer);
    
    return TRUE;
}

void CSDLVideoRender::setWindowSize(QSize size)
{
    reInit(m_hWnd, m_nVideoWidth, m_nVideoHeight, m_nVideoFmt);
}

void CSDLVideoRender::setWindowTitle(const char * title)
{
    if (m_pWindow)
    {
        if (title)
        {
            SDL_SetWindowTitle(m_pWindow, title);
        }
        else
        {
            SDL_SetWindowTitle(m_pWindow, "");
        }
    }
}


