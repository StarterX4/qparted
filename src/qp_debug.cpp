/***************************************************************************
                         qp_debug.cpp  -  description
                             -------------------
    begin                : Wed Sep  4 19:21:54 UTC 2002
    copyright            : (C) 2002 by Francois Dupoux
    email                : fdupoux@partimage.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qstring.h>
#include <qdatetime.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
//#include <qmessagebox.h>

#include "qp_debug.h"

// define the global debug object
QP_Debug g_debug;

// =====================================
QP_Debug::QP_Debug()
{
   m_fDebug = NULL;
}

// =====================================
QP_Debug::~QP_Debug()
{

}

// =====================================
int QP_Debug::isOpen()
{
   return (m_fDebug != NULL);
}

// =====================================
int QP_Debug::write(const char *fmt, ...)
{
   va_list args;

  if (!isOpen())
      return -1;

   va_start(args, fmt);
   vfprintf(m_fDebug, fmt, args);
   va_end(args);
   fflush(m_fDebug);

   return 0;
}

// =====================================
int QP_Debug::write(const char *szFile, const char *szFunction, int nLine, const char *fmt, ...)
{
   va_list args;

   if (!isOpen())
      return -1;

   va_start(args, fmt);
   fprintf(m_fDebug, "[%s]->[%s]#%d: ", szFile, szFunction, nLine);
   vfprintf(m_fDebug, fmt, args);
   va_end(args);

   fflush(m_fDebug);

   return 0;
}

// =====================================
int QP_Debug::open()
{
   QDateTime dt = QDateTime::currentDateTime();
   QString strFilename = QString("/var/log/qtparted-%1-%2-%3-%4h%5m%6s.log")
                          .arg(dt.date().toString("yyyy-MM-dd"))
                          .arg(dt.time().toString("hh"))
                          .arg(dt.time().toString("mm"))
                          .arg(dt.time().toString("ss"));

   // open the file
   m_fDebug = fopen(strFilename.toLocal8Bit().constData(), "w+");
   if (!m_fDebug)
      return -1;

   return 0; // success
}

// =====================================
int QP_Debug::close()
{
   if (m_fDebug == nullptr)
     return -1;

   fclose(m_fDebug);
   m_fDebug = nullptr;

   return 0; // success
}
