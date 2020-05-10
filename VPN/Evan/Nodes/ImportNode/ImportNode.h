#ifndef IMPORTNODE_H
#define IMPORTNODE_H

#include "../INode.h"
#include <QDialog>
#include <QtGui>
#include "../../DataTypes/Variables.h"
#include "../../DataTypes/Specimens.h"
#include "../../DataTypes/Surface.h"
#include "DataTypes/Volume.h"

#include "Designer/ui_import.h"

class ImportNode : public QDialog , public INode, public Ui::ImportDialog
{
Q_OBJECT
private:
	Specimens* m_specimens;
	Specimens* m_currentSpecimens;
	Surface* m_surface;
	Surface* m_curve;
	Variables* m_matrix;
	Volumes* m_volumes;

    bool m_lmkFileChanged;
    bool m_labelsFileChanged;
    bool m_addVarsChanged;
    bool m_lmkTagsFileChanged;
    bool m_filePathsDisabled;
    bool m_surfaceFileChanged;
    bool m_curveFileChanged;
    bool m_matrixFileChanged;
    bool m_lmkPairsFileChanged;
    bool m_volumeFileChanged;
    QString m_settings;

    int m_sizePoint;
    int m_facePoint;

    QVBoxLayout *mainLayout;
    QDialogButtonBox *buttonBox;

protected:
	void setOutputPort0(Specimens* p)   {setOutputData(0,p);}
	void setOutputPort1(Surface* p)     {setOutputData(1,p);}
	void setOutputPort2(Variables* p)   {setOutputData(2,p);}
//	void setOutputPort3(Volumes* p)     {setOutputData(3,p);}

public:
	ImportNode(QWidget* parent=0);
	~ImportNode();
	void showGUI()
    {
        m_settings = toString();
        QWidget::activateWindow();
        QWidget::show();
    }
	const QObject* getDialog() const {return this;}
	void process();
	bool isUpdated() const;

	void setLmkFileInput();
	void importLabels(QString filename);
	void importAdditionalVars();
	void importLmkTags();
	void importLmkPairs();

    bool importRaw(const QString& fileName);
	bool importMorphologikaSurface(const QString& fileName);
	bool importMorphologikaCurve(const QString& fileName);
	bool importPly(const QString& fileName);
	bool importSur(const QString& fileName);
	bool importVolume(const QString& fileName);
	bool importObj(const QString& fileName);

	QString toString() const;
	void fromString(const QString&);

public slots:
    void accept();
    void reject();
    void browseDatafile();
    void browseLabelsFile();
    void browseAddVarsFile();
    void browseLmkTagsFile();
    void enableFilePaths();
    void disableFilePaths();
    void enableRawTextOptions();
    void disableRawTextOptions();
    void browseForSurface();
    void browseForTexture();
    void browseForMatrix();
    void browseForLmkPairs();
    void browseForVolume();
    void browseForTransferFunction();

    void lmkFileChanged()       {m_lmkFileChanged = true;}
    void labelsFileChanged()    {m_labelsFileChanged = true;}
    void addVarsFileChanged()   {m_addVarsChanged = true;}
    void lmkTagsFileChanged()   {m_lmkTagsFileChanged = true;}
    void matrixFileChanged()    {m_matrixFileChanged = true;}
    void curveFileChanged()     {m_curveFileChanged = true;}
    void surfaceFileChanged()   {m_surfaceFileChanged = true;}
    void lmkPairsFileChanged()  {m_lmkPairsFileChanged = true;}
    void volumeFileChanged()    {m_volumeFileChanged = true;}

signals:
	void dataReady();
};
#endif
