/*
    qparted - a frontend to libparted for manipulating disk partitions
    Copyright (C) 2002-2003 Vanni Brutto <zanac (-at-) libero dot it>
    Copyright (C) 2007-2008 David Tio <deux (-at-) arklinux.org>;	2021-2022 StarterX4

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

#include <QMessageBox>
#include <QStringList>
#include <QMenu>
#include <QObject>
#include <sys/param.h>  // MAXPATHLEN

#include "qp_drivelist.h"
#include "qp_devlist.h"
#include "qp_libparted.h"
#include "qp_common.h"

QP_DriveList::QP_DriveList(QWidget *parent, QP_Settings *settings)
    : QTreeWidget(parent)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
    setAllColumnsShowFocus(true);
    setColumnCount(1);
    setHeaderLabel(tr("Device"));

    /*---init the selected device---*/
    _selDevice = nullptr;

    devlist = new QP_DevList(settings);

    connect(this, &QP_DriveList::itemSelectionChanged, this, &QP_DriveList::slotDisksSelected);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QP_DriveList::customContextMenuRequested, this, &QP_DriveList::slotPopUp);
}

QP_DriveList::~QP_DriveList()
{
}

void QP_DriveList::setPopup(QMenu *popup)
{
    _popup = popup;
}

void QP_DriveList::buildView()
{
    /*---make the "root" of the listview---*/
    QTreeWidgetItem* ideRoot = new QTreeWidgetItem(this);
    ideRoot->setText(0, tr("Disks"));

    /*---get a list of all available devices---*/
    devlist->getDevices();

    //QStrList lstdrives = QP_LibParted::device_probe();
    if (devlist->devlist.count() == 0)
    {
        QMessageBox::information(this, "QParted",
            QString(tr("No device found. Maybe you're not using root user?")));
    }

    /*---make the group menu---*/
    _agDevices = new QActionGroup(this);
    _agDevices->setExclusive(true);
    connect(_agDevices, &QActionGroup::selected, this, &QP_DriveList::slotActionSelected);

    /*---add every device found---*/
    for (QP_Device* p : devlist->devlist)
    {
        /*---get the device name---*/
        QString st = p->shortname();

        /*---add to the listview---*/
        QTreeWidgetItem *item = addDevice(st, ideRoot);

        /*---add to the group menu---*/
        QAction *actDisk = new QAction(st, _agDevices);
        actDisk->setCheckable(true);

        QP_DeviceNode *devicenode = new QP_DeviceNode();
        devicenode->action = actDisk;
        devicenode->listitem = item;

        p->setData(reinterpret_cast<void *>(devicenode));
    }

    if (ideRoot->childCount())
        ideRoot->setExpanded(true);
    else
        delete ideRoot;
}

QTreeWidgetItem* QP_DriveList::addDevice(QString dev, QTreeWidgetItem* parent)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parent);
    item->setText(0, dev);

    return item;
}

QActionGroup* QP_DriveList::agDevices()
{
    return _agDevices;
}

QP_Device* QP_DriveList::selDevice()
{
    return _selDevice;
}

void QP_DriveList::slotDisksSelected()
{
    QTreeWidgetItem* item = currentItem();
    /*---if the item selected is not the root---*/
    if (!item->childCount())
    {
        /*---get the device name (ex. /dev/hda)---*/
        QString name = item->text(0);

        /*---scan for every menu in the group---*/
        for (QP_Device* dev : devlist->devlist)
        {
            /*---get the device node from devlist---*/
            QP_DeviceNode* p = static_cast<QP_DeviceNode*>(dev->data());

            /*---the name match, so select it!---*/
            if (p->action->text().compare(name) == 0)
            {
                p->action->setChecked(true);

                /*---and of course selecte the "selected device"---*/
                _selDevice = dev;
            }
        }
        emit deviceSelected(_selDevice);
    }
}

void QP_DriveList::slotActionSelected(QAction* action)
{
    /*---get the device name (ex. /dev/hda)---*/
    QString name = action->text();

    /*---scan for every menu in the group---*/
    for (QP_Device* dev : devlist->devlist)
    {
        /*---get the device node from devlist---*/
        QP_DeviceNode* p = static_cast<QP_DeviceNode*>(dev->data());

        /*---the name match, so select it!---*/
        if (p->listitem->text(0).compare(name) == 0)
        {
            setCurrentItem(p->listitem);

            /*---and of course selecte the "selected device"---*/
            _selDevice = dev;
        }
    }
}

void QP_DriveList::slotPopUp()
{
    if (_popup) _popup->popup(mapToGlobal(QCursor::pos()));
}
