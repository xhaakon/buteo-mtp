/*
* This file is part of libmeegomtp package
*
* Copyright (C) 2010 Nokia Corporation. All rights reserved.
*
* Contact: Deepak Kodihalli <deepak.kodihalli@nokia.com>
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this list
* of conditions and the following disclaimer. Redistributions in binary form must
* reproduce the above copyright notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided with the distribution.
* Neither the name of Nokia Corporation nor the names of its contributors may be
* used to endorse or promote products derived from this software without specific
* prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

#include "storageitem.h"

#include <QDateTime>
#include <QFileInfo>

using namespace meegomtp1dot0;

// Constructor.
StorageItem::StorageItem() : m_handle(0), m_path(""), m_wd(-1), m_objectInfo(0),
                             m_parent(0), m_firstChild(0), m_nextSibling(0), m_puoid(MtpInt128(0))
{
}

// Destructor
StorageItem::~StorageItem()
{
    if( m_objectInfo )
    {
        delete m_objectInfo;
        m_objectInfo = 0;
    }
}

MTPObjFormatCode StorageItem::objectFormatByExtension() const
{
    static QHash<QString,MTPObjFormatCode> extensionToFormat;
    if (extensionToFormat.isEmpty()) {
        extensionToFormat["3gp"] = MTP_OBF_FORMAT_3GP_Container;
        extensionToFormat["aac"] = MTP_OBF_FORMAT_AAC;
        extensionToFormat["alb"] = MTP_OBF_FORMAT_Abstract_Audio_Album;
        extensionToFormat["avi"] = MTP_OBF_FORMAT_AVI;
        extensionToFormat["bmp"] = MTP_OBF_FORMAT_BMP;
        extensionToFormat["gif"] = MTP_OBF_FORMAT_GIF;
        extensionToFormat["htm"] = MTP_OBF_FORMAT_HTML;
        extensionToFormat["html"] = MTP_OBF_FORMAT_HTML;
        extensionToFormat["jpeg"] = MTP_OBF_FORMAT_EXIF_JPEG;
        extensionToFormat["jpg"] = MTP_OBF_FORMAT_EXIF_JPEG;
        extensionToFormat["mp3"] = MTP_OBF_FORMAT_MP3;
        extensionToFormat["mp4"] = MTP_OBF_FORMAT_MP4_Container;
        extensionToFormat["mpeg"] = MTP_OBF_FORMAT_MPEG;
        extensionToFormat["mpg"] = MTP_OBF_FORMAT_MPEG;
        extensionToFormat["ogg"] = MTP_OBF_FORMAT_OGG;
        extensionToFormat["pla"] = MTP_OBF_FORMAT_Abstract_Audio_Video_Playlist;
        extensionToFormat["pls"] = MTP_OBF_FORMAT_PLS_Playlist;
        extensionToFormat["png"] = MTP_OBF_FORMAT_PNG;
        extensionToFormat["tif"] = MTP_OBF_FORMAT_TIFF;
        extensionToFormat["tiff"] = MTP_OBF_FORMAT_TIFF;
        extensionToFormat["txt"] = MTP_OBF_FORMAT_Text;
        extensionToFormat["wav"] = MTP_OBF_FORMAT_WAV;
        extensionToFormat["wma"] = MTP_OBF_FORMAT_WMA;
        extensionToFormat["wmv"] = MTP_OBF_FORMAT_WMV;
    }

    // TODO Fetch from tracker or determine from the file.

    QFileInfo item(m_path);
    if(item.isDir()) {
        return MTP_OBF_FORMAT_Association;
    } else {
        QString ext(item.suffix().toLower());
        return extensionToFormat.value(ext, MTP_OBF_FORMAT_Undefined);
    }
}

quint64 StorageItem::size() const
{
    QFileInfo item(m_path);
    return item.isFile() ? item.size() : 0;
}

bool StorageItem::isImage() const
{
    MTPObjFormatCode objectFormat = MTP_OBF_FORMAT_Undefined;

    // Do we have the format already stored in object info?
    if (m_objectInfo) {
        objectFormat = m_objectInfo->mtpObjectFormat;
    }

    if (objectFormat == MTP_OBF_FORMAT_Undefined) {
        // Looks like we don't, determine from extension.
        objectFormat = objectFormatByExtension();
    }

    switch (objectFormat) {
        case MTP_OBF_FORMAT_BMP:
        case MTP_OBF_FORMAT_EXIF_JPEG:
        case MTP_OBF_FORMAT_GIF:
        case MTP_OBF_FORMAT_PNG:
        case MTP_OBF_FORMAT_TIFF:
            return true;
        default:
            return false;
    }
}

MTPAssociationType StorageItem::associationType() const
{
    QFileInfo item(m_path);
    /* GenFolder is the only type used in MTP. The others may be used for PTP
     * compatibility but are not required. */
    return item.isDir() ?
            MTP_ASSOCIATION_TYPE_GenFolder : MTP_ASSOCIATION_TYPE_Undefined;
}

QString StorageItem::dateCreated() const
{
    return formatMTPDateTime(QFileInfo(m_path).created());
}

QString StorageItem::dateModified() const
{
    return formatMTPDateTime(QFileInfo(m_path).lastModified());
}

QString StorageItem::formatMTPDateTime(const QDateTime &date)
{
    return date.toUTC().toString("yyyyMMdd'T'hhmmss'Z'");
}
