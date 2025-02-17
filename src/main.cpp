/*
	qparted - a frontend to libparted for manipulating disk partitions
	Copyright (C) 2002-2003 Vanni Brutto

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

#include <getopt.h>
#include <QApplication>
#include <QDebug>
#include <QTranslator>
#include <QMessageBox>
#include <QLocale>
#include <QSplashScreen>
#include <QTimer>
#include "qp_libparted.h"
#include "qp_window.h"
#include "qp_settings.h"
#include "qp_common.h"
#include "qp_debug.h"

#include <iostream>

using namespace std;

QP_MainWindow *mainwindow;

void print_usage(const char *program_name) {
    cout << "Usage: " << program_name << " [OPTION]..." << endl
            << "A nice QT GUI for libparted" << endl << endl
            << "Options used by qparted:" << endl
            << "  -l, --log=value	   use 1 to enable log, 0 for disable it." << endl
            << "						[default = 1])" << endl
            << "  -h, --help			Show this usage message" << endl
            << endl << endl
            << program_name << " by Zanac copyright 2003, (C) 2005 Ark Linux" << endl
            << "Send bug reports to bero@arklinux.org" << endl;
    exit(EXIT_SUCCESS);
}


int main(int argc, char *argv[]) {
    // This allows to run QParted with QtEmbedded without having
    // to pass parameters "-qws".
    // This is, however, potentially harmful, for example if we're being launched
    // from another QWS application (OPIE, Ark Linux installer, .....) - so we
    // add a configure option to turn it off...
#if defined(Q_WS_QWS) && !defined(QWS_CLIENT) // Frame Buffer
    QApplication app(argc, argv, QApplication::GuiServer);
#else // X11
    QApplication app(argc, argv);
#endif // Q_WS_QWS

    /*---program name ;)---*/
    const char *program_name = argv[0];

    int next_option;

    /*---Flag log on/off, default 1 = on---*/
    int iLog = 1;

    /*---valid short options---*/
    const char *const short_options = "hl:";

    /*---valid long options---*/
    const option long_options[] = {
        { "help",	0, nullptr, 'h' },
        { "log",	 1, nullptr, 'l' },
        { nullptr,	  0, nullptr, 0   } // end of getopt array
    };

    do {
        next_option = getopt_long(argc,
                                  argv,
                                  short_options,
                                  long_options,
                                  nullptr);

		switch (next_option) {
		case 'h': // -h ... --help
			print_usage(program_name);
			break;

		case 'l': // -l ... --log
			if (!sscanf(optarg, "%d", &iLog)) {
				qDebug() << "You must specify a numeric value.\n"
					   "Parameter \"" << optarg << "\" is not valid for --log\n\n";
				print_usage(program_name);
			}

			if ((iLog != 0) && (iLog != 1)) {
				qDebug() << "You must use 1/0 to set log on/off.\n"
				       "Parameter \"" << optarg << "\" is not valid for --log\n\n";
				print_usage(program_name);
			}
			break;

		case '?': // opzione invalida :(
			print_usage(program_name);

		case -1:  // opzioni concluse
			break;

		default:  // errore! inaspettato pure!
			abort();
		}
	} while (next_option != -1);
/*---install translation file for application strings---*/
	QTranslator *translator = new QTranslator();
	translator->load("qparted_"+QLocale::system().name(), ":/locale");
	app.installTranslator(translator);

	/*---initialize the debug system---*/
	if (iLog) g_debug.open();
	showDebug("QParted debug logfile (https://github.com/ZZYZX/qparted) version %s\n---------\n", VERSION);

	/*---check the Parted version---*/
	if (!QP_LibParted::checkForParted())
		return EXIT_FAILURE;

	/*---check if the kernel support devfs---*/
	isDevfsEnabled();

	QP_Settings settings;
	
	QP_MainWindow mainwindow(&settings, nullptr);

	QSplashScreen *splash = new QSplashScreen(QPixmap(DATADIR "/pixmaps/qtp_splash.png"));
	splash->connect(&mainwindow, SIGNAL(sigSplashInfo(const QString &)),
					SLOT(message(const QString &)));
	splash->finish(&mainwindow);
	splash->show();

	mainwindow.init();

#ifdef Q_WS_QWS // Frame Buffer
	mainwindow.showMaximized();
#endif // Q_WS_QWS

	mainwindow.show();

	bool rc = app.exec();

	delete splash;
	delete &mainwindow;

	if (iLog) g_debug.close();

	return rc;
}

