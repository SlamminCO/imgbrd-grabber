#ifndef DOWNLOADS_TAB_H
#define DOWNLOADS_TAB_H

#include <QProgressBar>
#include <QQueue>
#include <QSet>
#include <QSettings>
#include <QSoundEffect>
#include <QTableWidget>
#include <QWidget>
#include "models/image.h"
#include "models/site.h"


namespace Ui
{
	class DownloadsTab;
}


class BatchDownloader;
class BatchDownloadImage;
class BatchWindow;
class DownloadGroupTableModel;
class DownloadImageTableModel;
class DownloadProgressWidget;
class DownloadQueryGroup;
class DownloadQueryImage;
class DownloadQueue;
class GroupDownloadProgress;
class ImageDownloader;
struct ImageSaveResult;
class PackLoader;
class Page;
class Profile;
class QElapsedTimer;
class QTimer;
class MainWindow;

class DownloadsTab : public QWidget
{
	Q_OBJECT

	public:
		explicit DownloadsTab(Profile *profile, DownloadQueue *downloadQueue, MainWindow *parent);
		~DownloadsTab() override;

	public slots:
		// Clear
		void batchClear();
		void batchClearSel();
		void batchClearSelGroups();
		void batchClearSelUniques();
		void batchRemoveGroups(QList<int> rows);
		void batchRemoveUniques(QList<int> rows);
		void siteDeleted(Site *site);

		// Move
		void batchMove(int);
		void batchMoveToTop();
		void batchMoveUp();
		void batchMoveDown();
		void batchMoveToBottom();

		// Add
		void addGroup();
		void addUnique();
		void batchAddGroup(const DownloadQueryGroup &values);
		void batchAddUnique(const DownloadQueryImage &query, bool save = true);
		QTableWidgetItem *addTableItem(QTableWidget *table, int row, int col, const QString &text);

		// Update
		void updateGroupCount();

		// Downloads lists
		void saveFile();
		void loadFile();
		void saveLinkListLater();
		bool saveLinkListDefault();
		bool saveLinkList(const QString &filename, bool saveProgress = true);
		bool loadLinkList(const QString &filename);

		// Download
		void batchSel();
		void getAll(bool all = true);
		void getAllFinishedPage(Page *page);
		void getAllFinishedImages(const QList<QSharedPointer<Image>> &images);
		void getAllImages();
		void getAllGetImage(const BatchDownloadImage &download, int siteId);
		void getAllGetImageSaved(const QSharedPointer<Image> &img, QList<ImageSaveResult> result);
		void getAllProgress(const QSharedPointer<Image> &img, qint64 bytesReceived, qint64 bytesTotal);
		void getAllCancel();
		void getAllPause();
		void getAllSkip();
		void getAllLogin();
		void getNextPack();
		void getAllGetPages();
		void getAllFinished();
		void getAllFinishedLogin(Site *site, Site::LoginResult result);
		void getAllFinishedLogins();
		int getRowForSite(int siteId);
		void getAllImageOk(const BatchDownloadImage &download, int siteId, bool retry = false);
		void imageUrlChanged(const QUrl &before, const QUrl &after);
		void _getAll();

		// Others
		QIcon &getIcon(const QString &path);
		bool isDownloading() const;
		void batchDownloadsTableContextMenu(const QPoint &pos);

	protected:
		void changeEvent(QEvent *event) override;
		void closeEvent(QCloseEvent *event) override;
		QSet<int> selectedRows(QTableView *table) const;

	private:
		Ui::DownloadsTab *ui;
		Profile *m_profile;
		QSettings *m_settings;
		DownloadQueue *m_downloadQueue;
		MainWindow *m_parent;

		int m_getAllDownloaded, m_getAllExists, m_getAllIgnored, m_getAllIgnoredPre, m_getAll404s, m_getAllErrors, m_getAllSkipped, m_getAllResumed, m_getAllLimit;
		bool m_getAll;
		BatchWindow *m_progressDialog;
		QMap<QUrl, QElapsedTimer> m_downloadTime;
		QMap<QUrl, QElapsedTimer> m_downloadTimeLast;
		QList<DownloadQueryImage> m_batchs;
		QMap<int, DownloadQueryGroup> m_batchPending;
		QMap<int, BatchDownloader*> m_batchDownloaders;
		QSet<int> m_batchDownloading;
		QSet<int> m_batchUniqueDownloading;
		QList<DownloadQueryGroup> m_groupBatchs;
		QList<BatchDownloadImage> m_getAllRemaining, m_getAllDownloading, m_getAllFailed, m_getAllSkippedImages;
		QMap<QSharedPointer<Image>, ImageDownloader*> m_getAllImageDownloaders;
		QMap<QString, QIcon> m_icons;
		QQueue<PackLoader*> m_waitingPackLoaders;
		PackLoader *m_currentPackLoader = nullptr;
		QList<Site*> m_getAllLogins;
		int m_batchAutomaticRetries, m_getAllImagesCount, m_batchCurrentPackSize;
		QAtomicInt m_getAllCurrentlyProcessing;
		QTimer *m_saveLinkList;
		DownloadGroupTableModel *m_groupBatchsModel;
		DownloadImageTableModel *m_batchsModel;
		QSoundEffect m_finishedSoundEffect;
		DownloadProgressWidget *m_downloadProgressWidget;
};

#endif // DOWNLOADS_TAB_H
