#include <QMessageBox>
#include <QDebug>

#include "mainwindow.h"
#include "util.h"
#include "modifiedfilesdialog.h"


int handleModifiedFiles() {
	/*
	 * if(numOpenFiles == 0)
	 *		return 1;
	 */

	ModifiedFilesDialog m;
	m.exec();
	int clickResult = m.getResult();
	if(clickResult == 2) {
		// user clicked "Save All"
		/*
		 * for(openFille in openFiles) {
		 *		openFile.save();
		 * }
		 */
	}

	return clickResult;
}
