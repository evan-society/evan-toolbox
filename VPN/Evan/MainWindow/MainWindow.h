#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QSpinBox>
#include "Designer/ui_MainWindow.h"
#include "Designer/ui_license.h"
#include "Designer/ui_register.h"
#include "Designer/ui_regInstruct.h"

class XMLParser;
class QVBoxLayout;
class QAbstractButton;
class DropArea;
class VPNLayout;
class Logger;


class MainWindow : public QMainWindow, public Ui::MainWindow
{
	Q_OBJECT

private:
    DropArea* m_activeProject;
    Logger* m_logger;
    QSpinBox *m_zoomSpin;
    QString m_licenseKey;
    QString m_licenseText;

	typedef QVector<QString>::iterator nodeNameIterator;
	QVector<QString> m_nodeNames;

	typedef QVector<QAbstractButton*>::iterator nodeButtonIterator;
	QVector<QAbstractButton*> m_nodeButtons;

	void loadNodeBox();
	void connectActions();
	void connectVPN();
	void loadTabNodes(XMLParser&, QVBoxLayout*);

	QAction *recentFileActs[5];

    Ui::RegisterDialog Form;
    Ui::Dialog registerInstuctDialog;

    void crashBugHack();

protected:
    void closeEvent(QCloseEvent *event);

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

    void setDeleteActionDisabled(bool on);

public slots:
    void zoomChanged(int);
	void nodeButtonChecked(QAbstractButton*);
	void nodeButtonClicked(bool);
	void nodeInsertDone();
	void linkCreateDone();
	VPNLayout* createNewProject();
	void runNetwork(bool);
	void forceRunNetwork(bool);
	void saveNetwork();
	void saveNetworkAs();
	void saveNetworkWithStates();
	void saveNetworkWithoutStates();
	void loadNetwork(QString filename);
    void loadNetworkFromFilename();
    void updateRecentFileActions();
    void openRecentFile();
    void showAbout();
    void showRegistration();
    void showLicense();
    bool validateLicenseKey();
    void enterLicenseKey();
    QString strippedName(const QString &fullFileName);
    void updateActions();
};

#endif
