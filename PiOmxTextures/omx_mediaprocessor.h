/*
 * Project: PiOmxTextures
 * Author:  Luca Carlon
 * Date:    11.01.2012
 *
 * Copyright (c) 2012, 2013 Luca Carlon. All rights reserved.
 *
 * This file is part of PiOmxTextures.
 *
 * PiOmxTextures is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PiOmxTextures is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PiOmxTextures.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef OMX_MEDIAPROCESSOR_H
#define OMX_MEDIAPROCESSOR_H

/*------------------------------------------------------------------------------
|    includes
+-----------------------------------------------------------------------------*/
#include <QString>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

#include <GLES2/gl2.h>
#include <stdexcept>

#include "omx_qthread.h"

// omxplayer lib.
#include <linux/RBP.h>
#include <OMXPlayerVideo.h>
#include <OMXPlayerAudio.h>
#include <OMXPlayerSubtitles.h>
#include <DllOMX.h>

using namespace std;


/*------------------------------------------------------------------------------
|    OMX_MediaProcessor class
+-----------------------------------------------------------------------------*/
/**
 * @brief The OMX_MediaProcessor class Plays the media passed to the constructor
 * using the HDMI as audio output and a texture as the rendering surface. The
 * ID of the texture is sent when it is ready.
 */
class OMX_MediaProcessor : public QObject
{
    Q_OBJECT
public:
    enum OMX_MediaProcessorState {
        STATE_STOPPED,
        STATE_INACTIVE,
        STATE_PAUSED,
        STATE_PLAYING
    };

    enum OMX_MediaProcessorError {
        ERROR_CANT_OPEN_FILE,
        ERROR_WRONG_THREAD
    };

    OMX_MediaProcessor(OMX_TextureProvider* provider);
    ~OMX_MediaProcessor();

    bool setFilename(QString filename, OMX_TextureData*& textureData);
    QString filename();
    QStringList streams();

    long currentPosition();

    OMX_TextureData* textureData();

    inline bool hasAudio() {
        return m_has_audio;
    }

    inline bool hasVideo() {
        return m_has_video;
    }

#ifdef ENABLE_SUBTITLES
    inline bool hasSubtitle() {
        return m_has_subtitle;
    }
#endif

    inline OMX_MediaProcessorState state() {
        return m_state;
    }

    COMXStreamInfo    m_hints_audio;
    COMXStreamInfo    m_hints_video;

public slots:
    bool play();
    bool stop();
    bool pause();
    bool seek(long position);

signals:
    void playbackStarted();
    void playbackCompleted();
    void textureInvalidated();
    void textureReady(const OMX_TextureData* textureData);
    void errorOccurred(OMX_MediaProcessorError error);

private slots:
    void mediaDecoding();
    void cleanup();

private:
    void setSpeed(int iSpeed);
    bool checkCurrentThread();

    OMX_QThread m_thread;
    QString m_filename;
    OMX_TextureData* m_textureData;

    AVFormatContext* fmt_ctx;
    AVStream* streamVideo;
    AVPacket pkt;

    volatile OMX_MediaProcessorState m_state;

    QMutex m_sendCmd;

    OMXClock*           m_av_clock;
    OMXPlayerVideo*     m_player_video;
    OMXPlayerAudio*     m_player_audio;
#ifdef ENABLE_SUBTITLES
    OMXPlayerSubtitles* m_player_subtitles;
#endif
    OMXReader           m_omx_reader;
    OMXPacket*          m_omx_pkt;

    CRBP m_RBP;
    COMXCore m_OMX;

    bool m_bMpeg;
    bool m_has_video;
    bool m_has_audio;
#ifdef ENABLE_SUBTITLES
    bool m_has_subtitle;
#endif
    bool m_buffer_empty;
    bool m_pendingStop;
    bool m_pendingPause;

    int m_subtitle_index;
    int m_audio_index;

    OMX_TextureProvider* m_provider;

    QMutex m_mutexPending;
    QWaitCondition m_waitPendingCommand;
};

#endif // OMX_MEDIAPROCESSOR_H
