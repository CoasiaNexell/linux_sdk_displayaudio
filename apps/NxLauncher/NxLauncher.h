#ifndef NXLAUNCHER_H
#define NXLAUNCHER_H

#include <QDialog>
#include <QQuickWidget>
#include <QWidget>
#include <QListWidgetItem>
#include <QPushButton>

//#include <CNX_StatusBar.h>

#include <NX_Type.h>

#include "nxpackagemanager.h"
#include "nxprocessmanager.h"

#include <NxEvent.h>

#include <QQueue>
#include "nxappinfo.h"

#include <QTimer>
#include <QFileSystemWatcher>

namespace Ui {
class NxLauncher;
}

class NxLauncher : public QDialog
{
	Q_OBJECT

private slots:
	void slotExecute(QString);

	void slotAccept();

	void slotReject();

	void slotTimer();

	void slotDetectCommand();

public:
	explicit NxLauncher(QWidget *parent = 0);
	~NxLauncher();

public:
	// call function from qml
	Q_INVOKABLE QVariantList getPluginInfoList();

	void LauncherShow(bool *bOk, bool bRequireRequestFocus);

protected:
	bool event(QEvent *event);

private:
	// callback functions
	static void RequestLauncherShow(bool *bOk);

	static void RequestShow();	

	static void RequestSendMessage(const char *pDst, const char *pMsg, int32_t iMsgSize);

	// Plugin Management from other plugin
	static void RequestPlugInRun(const char *pPlugin, const char *pArgs);

	static void RequestPlugInTerminate(const char *pPlugin);

	static void RequestPlugInIsRunning(const char *pPlugin, bool *bOk);

	// Audio Focus Management
	static void RequestAudioFocus(FocusPriority ePriority, bool *bOk);

	static void RequestAudioFocusTransient(FocusPriority ePriority, bool *bOk);

	static void RequestAudioFocusLoss();

	// Video Focus Management
	static void RequestVideoFocus(FocusPriority ePriority, bool *bOk);

	static void RequestVideoFocusTransient(FocusPriority ePriority, bool *bOk);

	static void RequestVideoFocusLoss();

	// Popup message
	static void RequestPopupMessage(PopupMessage *psPopup, bool *bOk);

	static void RequestExpirePopupMessage();

	void ExpirePopupMessage();

	//
	static void RequestTerminate();

	void KeyEvent(NxKeyEvent* e);

	void PopupMessageEvent(NxPopupMessageEvent *e);

	static QString FindCaller(uint32_t uiLevel);

	void NextVideoFocus();

	void Terminate(QString requestor);

	void Execute(QString plugin);

private:
	static NxLauncher *m_spInstance;

	QMap<QString, NxPluginInfo*> m_PlugIns;

	NxPackageScanner* m_pPackageManager;
	NxProcessManager* m_pProcessManager;

	static QQueue<QString> m_AudioFocusQueue;
	static QQueue<QString> m_VideoFocusQueue;

	QTimer m_Timer;

	int m_iPosition;

	QFileSystemWatcher *m_pWatcher;

private:
	Ui::NxLauncher *ui;
};

#endif // NXLAUNCHER_H