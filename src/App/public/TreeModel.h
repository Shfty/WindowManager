#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QObject>
#include <QRect>

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(treeModel);

#include <Win.h>

class TreeItem;

class TreeModel : public QObject
{
	Q_OBJECT

	Q_PROPERTY(TreeItem* rootItem MEMBER m_rootItem NOTIFY rootItemChanged)

public:
	explicit TreeModel(QObject *parent = nullptr);

	static TreeModel* getInstance(const QObject* child) {
		QObject* candidate = child->parent();
		while(candidate != nullptr)
		{
			TreeModel* treeModel = qobject_cast<TreeModel*>(candidate);
			if(treeModel != nullptr)
			{
				return treeModel;
			}

			candidate = candidate->parent();
		}

		return nullptr;
	}

	TreeItem* getRootItem() const { return m_rootItem; }

signals:
	void rootItemChanged();

	void moveWindows(QVariantList message);
	void setWindowStyle(HWND hwnd, qint32 style);

public slots:
	void load();
	void startup();
	void save();
	void saveDefault();

	void moveWindow(HWND hwnd, QRect geometry, bool visible);
	void commitWindowMove();

private:
	QString m_autosavePath;
	TreeItem* m_rootItem;

	int m_windowMoveCount;
	QVariantList m_windowMoveMessage;
};

#endif // TREEMODEL_H
