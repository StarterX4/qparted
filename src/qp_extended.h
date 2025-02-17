/*
    qparted - a frontend to libparted for manipulating disk partitions
    Copyright (C) 2002-2003 Vanni Brutto; 2015 ZZYZX; 2021-2022 StarterX4

    Vanni Brutto <zanac (-at-) libero dot it>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* About QP_Extended class:
 *
 * This is a widget derived from QP_PartWidget that display an extended partition.
 * You can add logical partition (QP_Partition) to the QP_Extended using the
 * function addLogical.
 */

#ifndef QP_EXTENDED_H
#define QP_EXTENDED_H

#include <QWidget>
#include "qp_partwidget.h"
#include "qp_partition.h"
#include <QResizeEvent>

class QP_Extended : public QP_PartWidget {
    Q_OBJECT

public:
    QP_Extended(QP_PartInfo *, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~QP_Extended();
    QWidget *container; // Widget in which you attach logical partition
    QP_Partition *addLogical(QP_PartInfo *); //Add logical partition to the container

protected:
    void resizeEvent(QResizeEvent *event) override; //Reimplemented to resize the container
};

#endif