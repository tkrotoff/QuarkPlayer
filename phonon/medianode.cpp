/*  This file is part of the KDE project
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>
    Copyright (C) 2007 Trolltech ASA. <thierry.bastian@trolltech.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#include "medianode.h"
#include "medianode_p.h"
#include "medianodedestructionhandler_p.h"
#include "factory_p.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{

    MediaNode::MediaNode(MediaNodePrivate &dd)
        : k_ptr(&dd)
    {
        k_ptr->q_ptr = this;
    }

bool MediaNode::isValid() const
{
    return const_cast<MediaNodePrivate *>(k_ptr)->backendObject() != 0;
}

    QList<Path> MediaNode::inputPaths() const
    {
        return k_ptr->inputPaths;
    }

    QList<Path> MediaNode::outputPaths() const
    {
        return k_ptr->outputPaths;
    }

    MediaNode::~MediaNode()
    {
        delete k_ptr;
    }

    QObject *MediaNodePrivate::backendObject()
    {
        if (!m_backendObject && Factory::backend()) {
            createBackendObject();
        }
        return m_backendObject;
    }

    MediaNodePrivate::~MediaNodePrivate()
    {
        foreach (MediaNodeDestructionHandler *handler, handlers) {
            handler->phononObjectDestroyed(this);
        }
        Factory::deregisterFrontendObject(this);
        delete m_backendObject;
        m_backendObject = 0;
    }

void MediaNodePrivate::deleteBackendObject()
{
    if (m_backendObject && aboutToDeleteBackendObject()) {
        delete m_backendObject;
        m_backendObject = 0;
    }
}

    MediaNodePrivate::MediaNodePrivate(MediaNodePrivate::CastId _castId) : castId(_castId),
        m_backendObject(0)
    {
        Factory::registerFrontendObject(this);
    }

    void MediaNodePrivate::addDestructionHandler(MediaNodeDestructionHandler *handler)
    {
        handlers.append(handler);
    }

    void MediaNodePrivate::removeDestructionHandler(MediaNodeDestructionHandler *handler)
    {
        handlers.removeAll(handler);
    }

    void MediaNodePrivate::addOutputPath(const Path &p)
    {
        outputPaths.append(p);
    }

    void MediaNodePrivate::addInputPath(const Path &p)
    {
        inputPaths.append(p);
    }

    void MediaNodePrivate::removeOutputPath(const Path &p)
    {
        int ret = outputPaths.removeAll(p);
        Q_ASSERT(ret == 1);
        Q_UNUSED(ret);
    }
    
    void MediaNodePrivate::removeInputPath(const Path &p)
    {
        int ret = inputPaths.removeAll(p);
        Q_ASSERT(ret == 1);
        Q_UNUSED(ret);
    }



} // namespace Phonon

QT_END_NAMESPACE
