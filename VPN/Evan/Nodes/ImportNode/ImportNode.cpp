#include "ImportNode.h"

#include "../../Utilities/Logger.h"

#include "../TemplandNode/ViewTree/LandmarkItem.h"


#include <QFileDialog>
#include <QSettings>

#include <cstring>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "ImportFilters.h"

#include "../../Utilities/rply.h"

#include "ew/Dig3Tableau.h"
#include "ew/Dig3Space.h"
#include "ew/DataflowSpline3.h"
#include "ew/View3Surface.h"
#include "ew/Surface3.h"
#include "ew/ErrorIO.h"
#include "ew/DataflowSurface3E.h"




// global variabels for reading ply
vector<double> x_vertices;
vector<double> y_vertices;
vector<double> z_vertices;
vector<int> x_faces;
vector<int> y_faces;
vector<int> z_faces;

// callbacks for reading ply
static int vertex_cb(p_ply_argument argument)
{
    long value_index;
    ply_get_argument_user_data(argument, NULL, &value_index);
    switch (value_index)
    {
    case 0:
        x_vertices.push_back((double)ply_get_argument_value(argument));
        break;
    case 1:
        y_vertices.push_back((double)ply_get_argument_value(argument));
        break;
    case 2:
        z_vertices.push_back((double)ply_get_argument_value(argument));
        break;
    default:
        break;
    }
    return 1;
}

static int face_cb(p_ply_argument argument)
{
    long length, value_index;
    ply_get_argument_property(argument, NULL, &length, &value_index);
    switch (value_index)
    {
    case 0:
        x_faces.push_back((int)ply_get_argument_value(argument));
        break;
    case 1:
        y_faces.push_back((int)ply_get_argument_value(argument));
        break;
    case 2:
        z_faces.push_back((int)ply_get_argument_value(argument));
        break;
    default:
        break;
    }
    return 1;
}

int ReadPlyFile(const char * filename, Matrix<double> * vertices, Matrix<unsigned int> * faces)
{
    p_ply ply = ply_open(filename, NULL);
    if (!ply)
    {
        string str("\tFailed to open ply file: ");
        str.append(filename);
        Logger::getInstance()->log(str.c_str(), Logger::RUN_ERROR);
        return 1;
    }
    if (!ply_read_header(ply))
    {
        string str("\tFailed to read header from ply file: ");
        str.append(filename);
        Logger::getInstance()->log(str.c_str(), Logger::RUN_ERROR);
        return 1;
    }

    long nvertices, nfaces;

    nvertices = ply_set_read_cb(ply, "vertex", "x", vertex_cb, NULL, 0);
    ply_set_read_cb(ply, "vertex", "y", vertex_cb, NULL, 1);
    ply_set_read_cb(ply, "vertex", "z", vertex_cb, NULL, 2);
    nfaces = ply_set_read_cb(ply, "face", "vertex_indices", face_cb, NULL, 0);

    // clear existing data
    x_vertices.clear();
    y_vertices.clear();
    z_vertices.clear();
    x_faces.clear();
    y_faces.clear();
    z_faces.clear();

    if (!ply_read(ply))
    {
        string str("\tError reading ply file: ");
        str.append(filename);
        Logger::getInstance()->log(str.c_str(), Logger::RUN_ERROR);
        return 1;
    }
    ply_close(ply);

    // save data to matrices
    vertices->ReSize(x_vertices.size(),3);
    for (unsigned int i=0;i<x_vertices.size();i++)
    {
        (*vertices)[i][0] = x_vertices[i];
        (*vertices)[i][1] = y_vertices[i];
        (*vertices)[i][2] = z_vertices[i];
    }

    faces->ReSize(x_faces.size(),3);
    for (unsigned int i=0;i<x_faces.size();i++)
    {
        (*faces)[i][0] = x_faces[i];
        (*faces)[i][1] = y_faces[i];
        (*faces)[i][2] = z_faces[i];
    }
    return 0;
}


int ReadSurFile(const char * filename, Matrix<double> * vertices, Matrix<unsigned int> * faces)
{
    QFile file(filename);
    if (!file.exists())
    {
        QString msg = "\tFile not found: " + QString(filename);
        Logger::getInstance()->log(msg, Logger::WARNING);
        return 1;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString msg = "\tError opening file: " + QString(filename);
        Logger::getInstance()->log(msg, Logger::WARNING);
        return 1;
    }
    QTextStream input(&file);

    vector<vector<double> > tempVertices;
    vector<vector<int> > tempFaces;

    if (input.atEnd())
    {
        QString msg = "\tNo data in file: " + QString(filename);
        Logger::getInstance()->log(msg, Logger::WARNING);
        return 1;
    }

    QString line;
    // read raw data into matrix
    while (!input.atEnd())
    {
        // read next line
        line = input.readLine();
        if (line.toLower().contains("points"))
        {
            while (!input.atEnd())
            {
                vector<double> vec(3);
                line = input.readLine();
                if (line.startsWith("{") && line.endsWith("}"))
                {
                    line.replace("{","");
                    line.replace("}","");

                    QStringList values = line.split(" ");
                    if (values.size()==3)
                    {
                        vec[0]=values[0].toDouble();
                        vec[1]=values[1].toDouble();
                        vec[2]=values[2].toDouble();
                        tempVertices.push_back(vec);
                    }
                }
                else if (line.startsWith("}"))
                {
                    break;
                }
            }
        }
        if (line.toLower().contains("faces"))
        {
            while (!input.atEnd())
            {
                vector<int> vec(3);
                line = input.readLine();
                if (line.startsWith("{") && line.endsWith("}"))
                {
                    line.replace("{","");
                    line.replace("}","");

                    QStringList values = line.split(" ");
                    if (values.size()==3)
                    {
                        vec[0]=values[0].toInt();
                        vec[1]=values[1].toInt();
                        vec[2]=values[2].toInt();
                        tempFaces.push_back(vec);
                    }
                }
                else if (line.startsWith("}"))
                {
                    break;
                }
            }
        }
    }

    // save data to matrices
    vertices->ReSize(tempVertices.size(),3);
    for (unsigned int i=0;i<tempVertices.size();i++)
    {
        (*vertices)[i][0] = tempVertices[i][0];
        (*vertices)[i][1] = tempVertices[i][1];
        (*vertices)[i][2] = tempVertices[i][2];
    }

    faces->ReSize(tempFaces.size(),3);
    for (unsigned int i=0;i<tempFaces.size();i++)
    {
        (*faces)[i][0] = tempFaces[i][0];
        (*faces)[i][1] = tempFaces[i][1];
        (*faces)[i][2] = tempFaces[i][2];
    }
    return 0;
}

int ReadMorphologikaFile(const char * filename, Matrix<double> * vertices, Matrix<unsigned int> * faces,  Matrix<unsigned int> * lines)
{
    // get the data from file
    ifstream datafile (filename);

    // check if file exists
    if (datafile.fail())
    {
        string str("\tFailed to open Morphologika file: ");
        str.append(filename);
        Logger::getInstance()->log(str.c_str(), Logger::RUN_ERROR);
        return 1;
    }

    int i=0;
    int j=0;
    string str;

    vector<double> tempraw(3);
    vector<int> temppoly(3);
    vector<int> tempwire(2);

    int individuals = 1;
    int landmarks = 0;
    int dimensions = 3;

    vector<vector<double> > rawdata;
    vector<vector<int> > wireframe;
    vector<vector<int> > polygons;

    // put data into data matrix
    while (datafile >> str)
    {
startloop:
        transform(str.begin(), str.end(), str.begin(), ::tolower);

        if (str.substr(0,1) == "'")
        {
            // get comments at the top of the file
            string strline;
            getline(datafile,strline);

            // add rest of the line to str
            str.append(strline);

            // ignore spaces and tabs in the beginning of the string
            unsigned int startspace = 1;
            while ( startspace < str.length() )
            {
                if ( (str[ startspace ] == ' ') || (str[ startspace ] == '\t') )
                {
                    startspace++;
                }
                else
                {
                    break;
                }
            }
        }
        if (str.substr(0,13) == "[individuals]")
        {
            datafile >> individuals;
        }
        if (str.substr(0,11) == "[landmarks]")
        {
            datafile >> landmarks;
        }
        if (str.substr(0,12) == "[dimensions]")
        {
            datafile >> dimensions;
        }
        if (str.substr(0,11) == "[rawpoints]")
        {
            while (datafile >> str && i < landmarks)
            {
                if (str.substr(0,1) == "[")
                {
                    // stop if we have reached next section
                    goto startloop;
                }
                if (str.substr(0,1) != "'")
                {
                    tempraw[j] = atof(str.c_str());
                    j++;
                    if (j== dimensions)
                    {
                        j=0;
                        i++;
                        rawdata.push_back(tempraw);
                    }
                }
                if (str.substr(0,1) == "'")
                {
                    // ignore comments within section
                    string strline;
                    getline(datafile,strline);
                }
            }
        }
        if (str.substr(0,11) == "[wireframe]")
        {
            while (datafile >> str)
            {
                if (str.substr(0,1) == "[")
                {
                    // stop if we have reached next section
                    goto startloop;
                }
                if (str.substr(0,1) != "'")
                {
                    tempwire[j] = atoi(str.c_str());

                    j++;
                    if (j == 2)
                    {
                        j=0;
                        i++;
                        wireframe.push_back(tempwire);
                    }
                }
                if (str.substr(0,1) == "'")
                {
                    // ignore comments within section
                    string strline;
                    getline(datafile,strline);
                }
            }
        }
        if (str.substr(0,10) == "[polygons]")
        {
            while (datafile >> str)
            {
                if (str.substr(0,1) == "[")
                {
                    // stop if we have reached next section
                    goto startloop;
                }
                if (str.substr(0,1) != "'")
                {
                    temppoly[j] = atoi(str.c_str());

                    j++;
                    if (j == 3)
                    {
                        j=0;
                        i++;
                        polygons.push_back(temppoly);
                    }
                }
                else
                {
                    // ignore comments within section
                    string strline;
                    getline(datafile,strline);
                }
            }
        }
    }
    datafile.close();

    if (lines!=0)
    {
        lines->ReSize(wireframe.size(),2);
        for (unsigned int i=0;i<wireframe.size();i++)
        {
            (*lines)[i][0] = wireframe[i][0]-1;
            (*lines)[i][1] = wireframe[i][1]-1;
        }
    }
    if (faces!=0)
    {
        faces->ReSize(polygons.size(),3);
        for (unsigned int i=0;i<polygons.size();i++)
        {
            // reverse order because morphologika polygons are defined anti-clockwise
            // subtract one because morphologika is one-based but GMMVis zero-based
            (*faces)[i][0] = polygons[i][2]-1;
            (*faces)[i][1] = polygons[i][1]-1;
            (*faces)[i][2] = polygons[i][0]-1;
        }
    }
    if (vertices!=0)
    {
        vertices->ReSize(landmarks,dimensions);
        for (int i=0;i<landmarks;i++)
        {
            for (int j=0;j<dimensions;j++)
            {
                (*vertices)[i][j] = rawdata[i][j];
            }
        }
    }
    return 0;
}

//!Reads in a file to retrieve points
/*!
\param filename a string file name. This can be either the absolute or the relative path to the input file
Reads in filename points coordinate x y z and construct the point matrix accordingly
*/
bool ImportNode::importRaw(const QString& filename)
{
    //Initialization of input stream
    QFile file(filename);
    if (!file.exists())
    {
        Logger::getInstance()->log("[Import Node] Surface File doesn't exist!", Logger::RUN_ERROR);
        return false;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        Logger::getInstance()->log("[Import Node] Couldn't Open Surface file: "+filename, Logger::RUN_ERROR);
        return false;
    }
    QTextStream input(&file);

    //initialization
    bool ok1,ok2=true;
    double x,y,z; //x,y,z temporary value
    char dummy;
    input >> m_sizePoint >> dummy;
    Matrix<double>* pointOutput = new Matrix<double>(m_sizePoint,3);

    for (int i=0;i<m_sizePoint;i++)
    {
        ok2=true;
        QString line = input.readLine();
        QStringList stringLine = line.split(QRegExp("\\s+"));
        if (stringLine.size() >= 3)
        {
            x = stringLine.at(0).toDouble(&ok1);
            y = stringLine.at(1).toDouble(&ok1);
            z = stringLine.at(2).toDouble(&ok1);
            pointOutput->set(i,0,x);
            pointOutput->set(i,1,y);
            pointOutput->set(i,2,z);
        }
    }
    m_surface->setVertices(pointOutput);

    int i1,i2,i3;
    input >> m_facePoint >> dummy;
    Matrix<unsigned int>* faceOutput = new Matrix<unsigned int>(m_facePoint,3);
    for (int j=0;j<m_facePoint;j++)
    {
        QString line = input.readLine();
        QStringList stringLine = line.split(QRegExp("\\s+"));
        if (stringLine.size() >= 3)
        {
            i1 = stringLine.at(0).toInt(&ok1);
            i2 = stringLine.at(1).toInt(&ok1);
            i3 = stringLine.at(2).toInt(&ok1);
            if ( i1 < 0 || i2 < 0 || i3 < 0 )
            {
                Logger::getInstance()->log("[Import Node] Negative Face Index Found!", Logger::RUN_ERROR);
                return false;
            }
            int max = qMax(i1,i2);
            max = qMax(max,i3);
            if (max > m_sizePoint)
            {
                Logger::getInstance()->log("[Import Node] Invalid Face Index Found!", Logger::RUN_ERROR);
                return false;
            }
            faceOutput->set(j,0,i1);
            faceOutput->set(j,1,i2);
            faceOutput->set(j,2,i3);
        }
    }

    m_surface->setIndices(faceOutput);
    Logger::getInstance()->log(QString("[Import Node] Imported (%1) Vertices").arg(m_sizePoint));
    Logger::getInstance()->log(QString("[Import Node] Imported (%1) Faces").arg(m_facePoint));
    m_surface->initialize("Imported Surface");
    return true;
}

bool ImportNode::importObj(const QString& filename)
{
    //Initialization of input stream
    QFile file(filename);
    if (!file.exists())
    {
        Logger::getInstance()->log("[Import Node] Curve File doesn't exist!", Logger::RUN_ERROR);
        return false;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        Logger::getInstance()->log("[Import Node] Couldn't Open Curve file: "+filename, Logger::RUN_ERROR);
        return false;
    }
    QTextStream input(&file);

    QVector<QString> xc;
    QVector<QString> yc;
    QVector<QString> zc;
    QVector<QString> e1c;
    QVector<QString> e2c;
    QVector<QString> f1c;
    QVector<QString> f2c;
    QVector<QString> f3c;

    while (!input.atEnd())
    {
        QString line = input.readLine();

//        line = line.replace("\t"," ");
//        while (line.contains("  "))
//        {
//        	line = line.replace("  "," ");
//        }

        QStringList stringLine = line.split(" ");
        if (stringLine.size() == 4 && stringLine[0] == "v")
        {
        	xc.append(stringLine[1]);
        	yc.append(stringLine[2]);
        	zc.append(stringLine[3]);
        }
        else if(stringLine.size() == 4 && stringLine[0] == "f")
        {
        	if(stringLine[1].contains("//"))
        	{
            	f1c.append(stringLine[1].split("//")[0]);
        	}
        	else
        	{
        		f1c.append(stringLine[1]);
        	}
        	if(stringLine[2].contains("//"))
        	{
            	f2c.append(stringLine[2].split("//")[0]);
        	}
        	else
        	{
        		f2c.append(stringLine[2]);
        	}
        	if(stringLine[3].contains("//"))
        	{
            	f3c.append(stringLine[3].split("//")[0]);
        	}
        	else
        	{
        		f3c.append(stringLine[3]);
        	}
        }
        else if (stringLine.size() == 3 && stringLine[0] == "e")
        {
        	e1c.append(stringLine[1]);
        	e2c.append(stringLine[2]);
        }
    }
    m_sizePoint = xc.size();
    if(f1c.size()>0)
    {
    	m_facePoint = f1c.size();
    	Matrix<double>* pointOutput = new Matrix<double>(m_sizePoint,3);
    	Matrix<unsigned int>* faceOutput = new Matrix<unsigned int>(m_facePoint,3);

    	double x,y,z;
    	for (int i=0;i<m_sizePoint;i++)
    	{
    		x = xc[i].toDouble();
    		y = yc[i].toDouble();
    		z = zc[i].toDouble();
    		pointOutput->set(i,0,x);
    		pointOutput->set(i,1,y);
    		pointOutput->set(i,2,z);
    	}

    	for (int i=0;i<m_facePoint;i++)
    	{
    		unsigned int xi = f1c[i].toUInt();
    		unsigned int yi = f2c[i].toUInt();
    		unsigned int zi = f3c[i].toUInt();
    		faceOutput->set(i,0,xi-1);
    		faceOutput->set(i,1,yi-1);
    		faceOutput->set(i,2,zi-1);
    	}

    	m_surface->setVertices(pointOutput);
    	m_surface->setIndices(faceOutput);
    	Logger::getInstance()->log(QString("[Import Node] Imported (%1) Vertices").arg(m_sizePoint));
    	Logger::getInstance()->log(QString("[Import Node] Imported (%1) Faces").arg(m_facePoint));
    	m_surface->initialize("Imported Surface");
    }
    else
    {
    	m_facePoint = e1c.size();
    	Matrix<double>* pointOutput = new Matrix<double>(m_sizePoint,3);
    	Matrix<unsigned int>* faceOutput = new Matrix<unsigned int>(m_facePoint,2);

    	double x,y,z;
    	for (int i=0;i<m_sizePoint;i++)
    	{
    		x = xc[i].toDouble();
    		y = yc[i].toDouble();
    		z = zc[i].toDouble();
    		pointOutput->set(i,0,x);
    		pointOutput->set(i,1,y);
    		pointOutput->set(i,2,z);
    	}

    	for (int i=0;i<m_facePoint;i++)
    	{
    		int xi = e1c[i].toInt();
    		int yi = e2c[i].toInt();
    		faceOutput->set(i,0,xi-1);
    		faceOutput->set(i,1,yi-1);
    	}

    	m_surface->setVertices(pointOutput);
    	m_surface->setIndices(faceOutput);
    	Logger::getInstance()->log(QString("[Import Node] Imported (%1) Vertices").arg(m_sizePoint));
    	Logger::getInstance()->log(QString("[Import Node] Imported (%1) Edges").arg(m_facePoint));
    	m_surface->initialize("Imported Curve");
    }

    return true;
}

bool ImportNode::importMorphologikaSurface(const QString& filename)
{
    Matrix<double> *vertices = new Matrix<double>;
    Matrix<unsigned int> *faces = new Matrix<unsigned int>;
    Matrix<unsigned int> *lines = new Matrix<unsigned int>;

    ReadMorphologikaFile(filename.toAscii(), vertices, faces, lines);

    if (vertices->GetRows() > 0 && faces->GetRows() > 0)
    {
    	m_surface->setVertices(vertices);
    	m_surface->setIndices(faces);

    	Logger::getInstance()->log(QString("[Import Node] Imported (%1) Vertices").arg(vertices->GetRows()));
    	Logger::getInstance()->log(QString("[Import Node] Imported (%1) Faces").arg(faces->GetRows()));

    	m_surface->initialize("Imported Surface");
        return true;
    }
    return false;
}

bool ImportNode::importMorphologikaCurve(const QString& filename)
{
    Matrix<double> *vertices = new Matrix<double>;
    Matrix<unsigned int> *faces = new Matrix<unsigned int>;
    Matrix<unsigned int> *lines = new Matrix<unsigned int>;

    ReadMorphologikaFile(filename.toAscii(), vertices, faces, lines);

    if (vertices->GetRows() > 0 && lines->GetRows() > 0)
    {
    	m_curve->setVertices(vertices);
    	m_curve->setIndices(lines);

    	Logger::getInstance()->log(QString("[Import Node] Imported (%1) Vertices").arg(vertices->GetRows()));
    	Logger::getInstance()->log(QString("[Import Node] Imported (%1) Edges").arg(lines->GetRows()));

    	m_curve->initialize("Imported Curve");
        return true;
    }
    return false;
}

bool ImportNode::importPly(const QString& filename)
{
    Matrix<double> *vertices = new Matrix<double>;
    Matrix<unsigned int> *faces = new Matrix<unsigned int>;

    ReadPlyFile(filename.toAscii(), vertices, faces);

    m_surface->setVertices(vertices);
    m_surface->setIndices(faces);

    Logger::getInstance()->log(QString("[Import Node] Imported (%1) Vertices").arg(vertices->GetRows()));
    Logger::getInstance()->log(QString("[Import Node] Imported (%1) Faces").arg(faces->GetRows()));
    m_surface->initialize("Imported Surface");
    return true;
}

bool ImportNode::importSur(const QString& filename)
{
    Matrix<double> *vertices = new Matrix<double>;
    Matrix<unsigned int> *faces = new Matrix<unsigned int>;

    ReadSurFile(filename.toAscii(), vertices, faces);

    m_surface->setVertices(vertices);
    m_surface->setIndices(faces);

    Logger::getInstance()->log(QString("[Import Node] Imported (%1) Vertices").arg(vertices->GetRows()));
    Logger::getInstance()->log(QString("[Import Node] Imported (%1) Faces").arg(faces->GetRows()));
    m_surface->initialize("Imported Surface");
    return true;
}

bool ImportNode::importVolume(const QString& filename)
{
    if (!m_volumes->importVolume(filename))
    {
        QString newFilePath = filename;
        newFilePath.append(".dicom");
        if (!m_volumes->importVolume(newFilePath, true))
            return false;
    }
    if (radioButton_10->isChecked())
        m_volumes->setShadingModel(Volumes::Standard);
    else if (radioButton_8->isChecked())
        m_volumes->setShadingModel(Volumes::MaximumIntensityProjection);
    else if (radioButton_9->isChecked())
        m_volumes->setShadingModel(Volumes::Isosurface);
    return true;
}



ImportNode::ImportNode(QWidget* parent) : QDialog(parent),  INode()
{
    m_specimens = new Specimens();
    m_currentSpecimens = new Specimens();

    setupUi(this);

    connect(pushButton,SIGNAL(clicked()), this, SLOT(browseDatafile()));
    connect(pushButton_2, SIGNAL(clicked()), this, SLOT(browseLabelsFile()));
    connect(pushButton_3, SIGNAL(clicked()), this, SLOT(browseAddVarsFile()));
    connect(pushButton_4, SIGNAL(clicked()), this, SLOT(browseLmkTagsFile()));
    connect(pushButton_8, SIGNAL(clicked()), this, SLOT(browseForLmkPairs()));
    connect(pushButton_9, SIGNAL(clicked()), this, SLOT(browseForVolume()));
    connect(pushButton_10, SIGNAL(clicked()), this, SLOT(browseForTransferFunction()));

    connect(radioButton, SIGNAL(clicked()), this, SLOT(enableRawTextOptions()));
    connect(radioButton_2, SIGNAL(clicked()), this, SLOT(disableRawTextOptions()));
    connect(radioButton_3, SIGNAL(clicked()), this, SLOT(disableRawTextOptions()));
    connect(radioButton_4, SIGNAL(clicked()), this, SLOT(disableRawTextOptions()));
    connect(radioButton_7, SIGNAL(clicked()), this, SLOT(disableRawTextOptions()));

    connect(lineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(lmkFileChanged()));
    connect(lineEdit_2, SIGNAL(textChanged(const QString&)), this, SLOT(labelsFileChanged()));
    connect(lineEdit_3, SIGNAL(textChanged(const QString&)), this, SLOT(addVarsFileChanged()));
    connect(lineEdit_4, SIGNAL(textChanged(const QString&)), this, SLOT(lmkTagsFileChanged()));
    connect(lineEdit_5, SIGNAL(textChanged(const QString&)), this, SLOT(matrixFileChanged()));
    connect(lineEdit_6, SIGNAL(textChanged(const QString&)), this, SLOT(surfaceFileChanged()));
    connect(lineEdit_7, SIGNAL(textChanged(const QString&)), this, SLOT(surfaceFileChanged()));
    connect(lineEdit_8, SIGNAL(textChanged(const QString&)), this, SLOT(lmkPairsFileChanged()));
    connect(lineEdit_9, SIGNAL(textChanged(const QString&)), this, SLOT(volumeFileChanged()));

    connect(checkBox, SIGNAL(clicked()), this, SLOT(matrixFileChanged()));
    connect(checkBox_2, SIGNAL(clicked()), this, SLOT(lmkFileChanged()));

    m_lmkFileChanged = false;
    m_labelsFileChanged = false;
    m_addVarsChanged = false;
    m_lmkTagsFileChanged = false;
    m_surfaceFileChanged = false;
    m_matrixFileChanged = false;
    m_lmkPairsFileChanged = false;
    m_volumeFileChanged = false;

    m_filePathsDisabled = false;

    enableRawTextOptions();

    spinBox->setMaximum(1000000);
    spinBox_2->setMaximum(1000000);
    spinBox_3->setMaximum(1000000);
    spinBox_3->setMinimum(1);
    spinBox_3->setValue(1);

    setModal(true);

    m_sizePoint = 0;
    m_facePoint = 0;

    m_surface = new Surface();
    m_curve = new Surface();

    connect(pushButton_6,SIGNAL(clicked()), this, SLOT(browseForSurface()));
    connect(pushButton_7,SIGNAL(clicked()), this, SLOT(browseForTexture()));
    connect(pushButton_5,SIGNAL(clicked()), this, SLOT(browseForMatrix()));

    m_matrix = new Variables();

    m_volumes = new Volumes();

}

ImportNode::~ImportNode()
{
}

void ImportNode::process()
{
    if (m_lmkFileChanged)       setLmkFileInput();
    if (m_lmkTagsFileChanged)   importLmkTags();
    if (!m_filePathsDisabled)
    {
        if (m_labelsFileChanged)    importLabels(lineEdit_2->text());
        if (m_addVarsChanged)       importLabels(lineEdit_3->text());
    }
    if (m_lmkPairsFileChanged)   importLmkPairs();

    if (m_surfaceFileChanged && lineEdit_6->text() != "")
    {
        m_surface->clear();
        bool loaded = false;

        if (lineEdit_6->text().right(4).toLower() == ".txt")
            loaded = importMorphologikaSurface(lineEdit_6->text());
        else if (lineEdit_6->text().right(4).toLower() == ".ply")
            loaded = importPly(lineEdit_6->text());
        else if (lineEdit_6->text().right(4).toLower() == ".sur")
            loaded = importSur(lineEdit_6->text());
        else if (lineEdit_6->text().right(4).toLower() == ".obj")
            loaded = importObj(lineEdit_6->text());
        else
            loaded = m_surface->initialize("Imported Surface", lineEdit_6->text());

        if (lineEdit_7->text() != "" && !lineEdit_7->text().isEmpty())
            if (!m_surface->loadTexture(lineEdit_7->text()))
                Logger::getInstance()->log("[Import Node] Failed to load texture!",Logger::WARNING);

        if (loaded)
        {
            m_surface->setSpecimenNumber(spinBox_3->value());

            setOutputPort1(m_surface);
            Logger::getInstance()->log("[Import Node] Surface Imported");
//            Logger::getInstance()->log(QString("\tVertices: %1").arg(m_surface->getVertexCount()));
//            Logger::getInstance()->log(QString("\tFaces: %1").arg(m_surface->getIndexCount()));
        }
        else Logger::getInstance()->log("[Import Node] Failed to load surface file!",Logger::RUN_ERROR);
    }

    if (m_matrixFileChanged)
    {
        QString filename = lineEdit_5->text();
        if (filename != "")
        {
            Logger::getInstance()->log("[Import Node] Importing data matrix from file: " + filename);
            int rows = 0;
            int cols = 0;
            vector<string> rlabels;
            vector<string> clabels;
            vector<vector<double> > rawdata;
            vector<vector<QString> > stringdata;

            if (filename.right(4).toLower() == ".nts")
            {
                QString missing;

                ReadMatrixFromNTSFile(filename,
                                      &rows,
                                      &cols,
                                      &rawdata,
                                      &stringdata,
                                      &rlabels,
                                      &clabels,
                                      &missing);
            }
            else
            {
                ReadMatrixFromTextFile(filename,
                                       &rows,
                                       &cols,
                                       &rawdata,
                                       &stringdata,
                                       &rlabels,
                                       &clabels,
                                       checkBox->isChecked());
            }

            m_matrix->ReSize(rows,cols);

            if (clabels.size()>0)
            {
                for (unsigned int i=0;i<clabels.size();i++)
                {
                    QString str(clabels[i].c_str());
                    m_matrix->setColumnLabel(i,str);
                }
            }
            else
            {
                for (int i=0;i<cols;i++)
                {
                    stringstream ss;
                    ss << "col" << i+1;
                    m_matrix->setColumnLabel(i,ss.str().c_str());
                }
            }

            vector<string> names;
            if ((int)names.size() == rows)
            {
                for (unsigned int i=0;i<names.size();i++)
                {
                    m_matrix->setRowLabel(i,names[i].c_str());
                }
            }
            else if (rlabels.size()>0)
            {
                for (unsigned int i=0;i<rlabels.size();i++)
                {
                    QString str(rlabels[i].c_str());
                    m_matrix->setRowLabel(i,str);
                }
            }
            else
            {
                for (int i=0;i<rows;i++)
                {
                    stringstream ss;
                    ss << i+1;
                    m_matrix->setRowLabel(i,ss.str().c_str());
                }
            }

            for (int i=0;i<rows;i++)
            {
                for (int j=0;j<cols;j++)
                {
                    (*m_matrix)[i][j] = rawdata[i][j];
                }
            }

            if (!m_matrix->IsNull())
            {
                Logger::getInstance()->log("[Import Node] Data matrix imported:");
                Logger::getInstance()->log(QString("\t\tRows: %1").arg(m_matrix->GetRows()), Logger::INFO, false);
                Logger::getInstance()->log(QString("\t\tColumns: %1").arg(m_matrix->GetCols()), Logger::INFO, false);
            }
            else
            {
                Logger::getInstance()->log("[Import Node] Failed to import data matrix.", Logger::WARNING);
            }
            setOutputPort2(m_matrix);
        }
    }
#ifdef USE_DCMTK
    if (m_volumeFileChanged)
    {
        m_volumes->clear();
        QString filename = lineEdit_9->text();
        m_volumeFileChanged = false;
        done(0);
        if (importVolume(filename))
        {
            Logger::getInstance()->log("[Import Node] Volume imported:");
            if (!lineEdit_10->text().isEmpty() )
            {
                m_volumes->setTransferFunction(lineEdit_10->text().toStdString());
            }
            m_volumes->initialize("Imported Volume");
            setOutputPort3(m_volumes);
        }
        else
        {
            Logger::getInstance()->log("[Import Node] Volume not imported", Logger::WARNING);
        }
    }
#endif

    m_lmkFileChanged = false;
    m_labelsFileChanged = false;
    m_addVarsChanged = false;
    m_lmkTagsFileChanged = false;
    m_surfaceFileChanged = false;
    m_matrixFileChanged = false;
    m_lmkPairsFileChanged = false;
}

bool ImportNode::isUpdated() const
{
    return (m_lmkFileChanged || m_labelsFileChanged || m_addVarsChanged || m_lmkTagsFileChanged || m_surfaceFileChanged || m_matrixFileChanged || m_lmkPairsFileChanged || m_volumeFileChanged);
}

//!Sets the point file with the filepath1 value
/*!
The class parameter pointFileInput is set with the value of filepath1
*/
void ImportNode::setLmkFileInput()
{
    QString lmkFileInput = lineEdit->text();

    int landmarks = 0;
    int dimensions = 0;
    int individuals = 0;

    vector<vector<double> > rawdata;
    vector<vector<QString> > stringdata;
    vector<vector<int> > pairs;

    m_specimens->clear();

    if (lmkFileInput != "")
    {
        QString msg = "[Import Node] Importing Specimens from file: " + lmkFileInput;
        Logger::getInstance()->log(msg);

        vector<string> labelnames;
        vector<vector<string> > labelvalues;
        vector<string> names;
        vector<string> groups;
        vector<vector<int> > wireframe;
        vector<vector<int> > polygons;

        if (radioButton->isChecked())
        {
            individuals = spinBox->value();
            landmarks = spinBox_2->value();

            if (individuals == 0 || landmarks == 0)
            {
                QString msg = "[Import Node] Please select number of specimens and landmarks when reading raw data";
                Logger::getInstance()->log(msg, Logger::WARNING);
                return;
            }

            // Raw text file
            QFile file(lmkFileInput);
            if (!file.exists())
            {
                QString msg = "[Import Node] File not found: " + lmkFileInput;
                Logger::getInstance()->log(msg, Logger::WARNING);
                return;
            }
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                QString msg = "[Import Node] Error opening file: " + lmkFileInput;
                Logger::getInstance()->log(msg, Logger::WARNING);
                return;
            }

            QTextStream input(&file);

            if (radioButton_5->isChecked())
            {
                dimensions = 3;
            }
            else
            {
                dimensions = 2;
            }

            QString qstr;
            vector<QString> qstrvec;
            stringdata.clear();
            qstrvec.clear();

            while (!input.atEnd())
            {
                input >> qstr;
                if (qstr != "")
                {
                    qstrvec.push_back(qstr);
                    if ((int)qstrvec.size() == dimensions)
                    {
                        stringdata.push_back(qstrvec);
                        qstrvec.clear();
                    }
                }
            }

            if (!qstrvec.empty())
            {
                msg = "\tNumber of landmarks do not match number of dimensions. Landmark data might be incorrect";
                Logger::getInstance()->log(msg, Logger::WARNING);
            }

            if ((int)stringdata.size() != individuals * landmarks)
            {
                int individuals2 = stringdata.size()/landmarks;
                if (individuals2 != individuals)
                {
                    QString num;
                    num.setNum(individuals);
                    msg =  "\tNumber of individuals expected: " + num;
                    Logger::getInstance()->log(msg, Logger::WARNING);
                    num.setNum(stringdata.size()/(double)landmarks);
                    msg =  "\tNumber of individuals found: " + num;
                    Logger::getInstance()->log(msg, Logger::WARNING);
                    individuals = individuals2;
                }
                else
                {
                    msg =  "\tIncomplete landmark set";
                    Logger::getInstance()->log(msg, Logger::WARNING);
                }
            }

            for (int i=0;i<individuals;i++)
            {
                QString value;
                value.setNum(i+1);
                m_specimens->setLabelValue(i, "ID", value);
            }

            if (labelnames.size()>0)
            {
                for (unsigned int i=0;i<labelnames.size();i++)
                {
                    QString name(labelnames[i].c_str());
                    for (unsigned int j= 0;j<labelvalues.size();j++)
                    {
                        QString value((labelvalues[j][i]).c_str());
                        m_specimens->setLabelValue(i, name, value);
                    }
                }
            }
        }
        else if (radioButton_2->isChecked())
        {
            // Morphologika file
            ReadFromMorphologikaFile(lmkFileInput,
                                     &individuals,
                                     &landmarks,
                                     &dimensions,
                                     &rawdata,
                                     &stringdata,
                                     &groups,
                                     &names,
                                     &labelnames,
                                     &labelvalues,
                                     &wireframe,
                                     &polygons,
                                     &pairs);

            // set labels
            m_specimens->clearLabels();

            if (names.empty())
            {
                for (int i=0;i<individuals;i++)
                {
                    QString value;
                    value.setNum(i+1);
                    m_specimens->setLabelValue(i, "ID", value);
                }
            }
            else
            {
                for (unsigned int i = 0;i<names.size();i++)
                {
                    QString value(names[i].c_str());
                    m_specimens->setLabelValue(i, "ID", value);
                }
            }

            if (!groups.empty() && (int)groups.size() != individuals)
            {
                Logger::getInstance()->log("Group labels don´t match number of individuals", Logger::WARNING);
            }
            else
            {
                for (unsigned int i= 0;i<groups.size();i++)
                {
                    QString value(groups[i].c_str());
                    m_specimens->setLabelValue(i, "Group", value);
                }
            }

            for (unsigned int i= 0;i<labelnames.size();i++)
            {
                QString name(labelnames[i].c_str());
                for (unsigned int j= 0;j<labelvalues.size();j++)
                {
                    QString value((labelvalues[j][i]).c_str());
                    m_specimens->setLabelValue(j, name, value);
                }
            }
        }
        else if (radioButton_3->isChecked())
        {

            // NTS file
            vector<string> rowlabels;
            vector<string> collabels;
            QString missingstr;

            ReadFromNTSFile(lmkFileInput,
                            &individuals,
                            &landmarks,
                            &dimensions,
                            &rawdata,
                            &stringdata,
                            &rowlabels,
                            &collabels,
                            &missingstr);
            if (rowlabels.empty())
            {
                for (int i=0;i<individuals;i++)
                {
                    QString value;
                    value.setNum(i+1);
                    m_specimens->setLabelValue(i, "ID", value);
                }
            }
            else
            {
                names.clear();
                for (unsigned int i = 0;i<rowlabels.size();i++)
                {
                    QString value(rowlabels[i].c_str());
                    m_specimens->setLabelValue(i, "ID", value);
                    names.push_back(rowlabels[i]);
                }
            }
        }
        else if (radioButton_4->isChecked())
        {
            // TPS file
            vector<string> names;
            vector<string> imagepaths;
            QStringList scales;

            ReadFromTPSFile(lmkFileInput,
                            &individuals,
                            &landmarks,
                            &dimensions,
                            &rawdata,
                            &stringdata,
                            &scales,
                            &names,
                            &imagepaths);

            if (names.empty())
            {
                for (int i=0;i<individuals;i++)
                {
                    QString value;
                    value.setNum(i+1);
                    m_specimens->setLabelValue(i, "ID", value);
                }
            }
            else
            {
                for (unsigned int i = 0;i<names.size();i++)
                {
                    QString value(names[i].c_str());
                    m_specimens->setLabelValue(i, "ID", value);
                }
            }
        }
        else if (radioButton_7->isChecked())
        {
            QString fileName = lmkFileInput;
            QFile file(lmkFileInput);
            if (!file.exists())
            {
                Logger::getInstance()->log(QString("[Import Node] Failed to load tableau! File '%1' does not exist!").arg(fileName), Logger::RUN_ERROR);
                return;
            }

            std::vector<ew::Dig3Tableau> tempTableaus;
            std::vector<ew::Dig3Tableau> tableauList;

#ifdef _WIN32
            // for windows change the path slashes from forward to backward
            fileName = fileName.replace( '/', '\\' );
#else
            fileName = fileName.replace( '\\', '/' );
#endif

            ew::Dig3Tableau::read_file(&tableauList, fileName.toStdString().c_str());


            dimensions = 3;
            individuals = tableauList.size();
            landmarks = 0;

            for (unsigned int i=0;i<tableauList.size();i++)
            {

                QString formFile = tableauList[i].space[1].form_filename.c_str();
                if (!formFile.contains('\\') && !formFile.contains('/')) //Relative form path
                    formFile = fileName.left(fileName.lastIndexOf(QRegExp("[\\/]"))+1) + formFile;

                QFile file(formFile);
                if (!file.exists())
                {
                    Logger::getInstance()->log(QString("[Import Node] Failed to load form! File '%1' does not exist!").arg(formFile), Logger::RUN_ERROR);
                    return;
                }
                else
                {
                    Logger::getInstance()->log(QString("\tLoading form: %1").arg(formFile));
                }

#ifdef _WIN32
                // for windows change the path slashes from forward to backward
                QString str = formFile.replace( '/', '\\' );
#else
                QString str = formFile.replace( '\\', '/' );
#endif

                ew::Form3 form;
                form.read_file(str.toStdString().c_str());

                if (form.pointsets.size()>0)
                {
                    unsigned int lmks = 0;
                    for (unsigned int j=0; j<form.pointsets.size(); ++j)
                    {
                        vector<double> tempvec(3);
                        vector<QString> strvec(3);
                        if (form.pointsets[j].type == ew::Form3::TYPE_LANDMARK)
                        {
                            tempvec[0] = form.pointsets[j].locations[0];
                            tempvec[1] = form.pointsets[j].locations[1];
                            tempvec[2] = form.pointsets[j].locations[2];

                            QString x,y,z;
                            x.setNum(tempvec[0]);
                            y.setNum(tempvec[1]);
                            z.setNum(tempvec[2]);
                            strvec[0] = x;
                            strvec[1] = y;
                            strvec[2] = z;

                            rawdata.push_back(tempvec);
                            stringdata.push_back(strvec);
                            lmks++;
                        }
                        else if (form.pointsets[j].type == ew::Form3::TYPE_SEMI_LANDMARK)
                        {
                            // fill in the semi landmarks location in array
                            for ( int k = 0; k < form.pointsets[j].n; k++ )
                            {
                                tempvec[0] = form.pointsets[j].locations[3*k];
                                tempvec[1] = form.pointsets[j].locations[3*k+1];
                                tempvec[2] = form.pointsets[j].locations[3*k+2];

                                QString x,y,z;
                                x.setNum(tempvec[0]);
                                y.setNum(tempvec[1]);
                                z.setNum(tempvec[2]);
                                strvec[0] = x;
                                strvec[1] = y;
                                strvec[2] = z;

                                rawdata.push_back(tempvec);
                                stringdata.push_back(strvec);
                                lmks++;
                            }
                        }
                    }
                    if (landmarks == 0)
                    {
                        landmarks = lmks;
                    }
                    else if (landmarks != (int) lmks)
                    {
                        Logger::getInstance()->log("[Import Node] Number of landmarks has to be the same for all forms", Logger::RUN_ERROR);
                        return;
                    }

                }

                for (int i=0;i<individuals;i++)
                {
                    QString value;
                    value.setNum(i+1);
                    m_specimens->setLabelValue(i, "ID", value);
                }
            }
        }

        for (int k=0;k<individuals;k++)
        {
            LandmarkSet * lmkOutput = new LandmarkSet(landmarks,dimensions);

            for (int i=0;i<landmarks;i++)
            {
                for (int j=0;j<dimensions;j++)
                {
                    bool ok;
                    QString qstr = stringdata[k*landmarks+i][j];
                    double value = qstr.toDouble(&ok);
                    if (ok)
                    {
                        (*lmkOutput)[i][j] = value;
                    }
                    else
                    {
                        (*lmkOutput)[i][j] = QString("NaN").toDouble();
                        lmkOutput->addMissing(i);
                    }
                }
            }
            m_specimens->addLandmarkSet(lmkOutput);
        }

        if (!pairs.empty())
        {
            QPair<int,int> pair;
            for (unsigned int i=0;i<pairs.size();i++)
            {
                pair.first = pairs[i][0];
                pair.second = pairs[i][1];
                m_specimens->addLandmarkPair(pair);
            }
        }
        else
        {
            m_lmkPairsFileChanged = true;
        }
        if (checkBox_2->isChecked())
        {
            for (unsigned int k=0;k<m_specimens->getSize();k++)
            {
                LandmarkSet * lmkOutput = m_specimens->getLandmarkSet(k);

                for (unsigned int i=0;i<m_specimens->getLandmarkCount();i++)
                {
                    double temp = (*lmkOutput)[i][2];
                    (*lmkOutput)[i][2] = (*lmkOutput)[i][1];
                    (*lmkOutput)[i][1] = -temp;
                }
            }
        }

        if (m_specimens->isValid())
        {
            m_currentSpecimens->clear();

            // set labels
            m_currentSpecimens->clearLabels();

            if (names.empty())
            {
                for (int i=0;i<individuals;i++)
                {
                    QString value;
                    value.setNum(i+1);
                    m_currentSpecimens->setLabelValue(i, "ID", value);
                }
            }
            else
            {
                for (unsigned int i = 0;i<names.size();i++)
                {
                    QString value(names[i].c_str());
                    m_currentSpecimens->setLabelValue(i, "ID", value);
                }
            }

            if (!groups.empty() && (int)groups.size() != individuals)
            {
                Logger::getInstance()->log("Group labels don´t match number of individuals", Logger::WARNING);
            }
            else
            {
                for (unsigned int i= 0;i<groups.size();i++)
                {
                    QString value(groups[i].c_str());
                    m_currentSpecimens->setLabelValue(i, "Group", value);
                }
            }

            for (unsigned int i= 0;i<labelnames.size();i++)
            {
                QString name(labelnames[i].c_str());
                for (unsigned int j= 0;j<labelvalues.size();j++)
                {
                    QString value((labelvalues[j][i]).c_str());
                    m_currentSpecimens->setLabelValue(j, name, value);
                }
            }

            SpecimenGroup* defaultGroup = new SpecimenGroup;
            for (int k=0;k<individuals;k++)
            {
                LandmarkSet * lmkOutput = new LandmarkSet(landmarks,dimensions);

                for (int i=0;i<landmarks;i++)
                {
                    for (int j=0;j<dimensions;j++)
                    {
                        bool ok;
                        QString qstr = stringdata[k*landmarks+i][j];
                        double value = qstr.toDouble(&ok);
                        if (ok)
                        {
                            (*lmkOutput)[i][j] = value;
                        }
                        else
                        {
                            (*lmkOutput)[i][j] = QString("NaN").toDouble();
                            lmkOutput->addMissing(i);
                        }
                    }
                }

                m_currentSpecimens->addLandmarkSet(lmkOutput);
                defaultGroup->addMember(lmkOutput, m_currentSpecimens->getSpecimenLabelValue(k, "ID"));
            }
            m_currentSpecimens->addGroup("Ungrouped", defaultGroup);

            if((int)groups.size() == individuals)
            {
                unsigned int l=0;
                unsigned int m=0;
                unsigned int n=0;
                while(m < groups.size())
                {
                    QString groupname = groups[m].c_str();
                    while(m < groups.size() && groupname == groups[m].c_str())
                    {
                        m++;
                    }
                    SpecimenGroup* group = new SpecimenGroup;

                    for (unsigned int k=l;k<m;k++)
                    {
                        LandmarkSet * lmkOutput = m_currentSpecimens->getLandmarkSet(k);
                        group->addMember(lmkOutput, m_currentSpecimens->getSpecimenLabelValue(k, "ID"));
                    }
                    QStringList colors = QColor::colorNames();
                    group->setGroupColor(colors[n++]);
                    m_currentSpecimens->addGroup(groupname, group);
                    l = m;
                }
            }

            if (!pairs.empty())
            {
                QPair<int,int> pair;
                for (unsigned int i=0;i<pairs.size();i++)
                {
                    pair.first = pairs[i][0];
                    pair.second = pairs[i][1];
                    m_currentSpecimens->addLandmarkPair(pair);
                }
            }

            m_currentSpecimens->initialize("Imported Specimens");
            setOutputPort0(m_currentSpecimens);
        }
        Logger::getInstance()->log("[Import Node] Specimens Imported:");
        Logger::getInstance()->log(QString("\t\tIndividuals: %1").arg(individuals), Logger::INFO, false);
        Logger::getInstance()->log(QString("\t\tLandmarks: %1").arg(landmarks), Logger::INFO, false);
        Logger::getInstance()->log(QString("\t\tDimensions: %1").arg(dimensions), Logger::INFO, false);
    }
}

void ImportNode::importLabels(QString filename)
{
    QSettings settings("Evan");
    QStringList fileSettings = settings.value("lastDirectory").toStringList();
    QString fsLastDir = "";
    if( fileSettings.size() > 0 )
        fsLastDir = fileSettings[0];

    if (filename == "")
    {
        return;
    }

    QFile file(filename);
    if (!file.exists())
    {
        QString msg = "[Import Node] File not found: " + filename;
        Logger::getInstance()->log(msg, Logger::WARNING);
        return;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString msg = "[Import Node] Error opening file: " + filename;
        Logger::getInstance()->log(msg, Logger::WARNING);
        return;
    }
    QTextStream input(&file);

    if (input.atEnd())
    {
        QString msg = "[Import Node] No data in file: " + filename;
        Logger::getInstance()->log(msg, Logger::WARNING);
        return;
    }

    Logger::getInstance()->log("[Import Node] Importing Specimen Labels ...");

    QStringList labelNames;
    int labelCount = 0;

    m_currentSpecimens->clearLabels();

    if (filename.right(4).toLower() == ".lbl")
    {
        QString keyword;

        while (!input.atEnd())
        {
            input >> keyword;
            if (keyword.toLower() == "[names]")
            {
                input >> labelCount;
                for (int i=0; i<labelCount; ++i)
                {
                    input >> keyword;
                    labelNames.push_back(keyword);
                }
            }
            else if (keyword.toLower() == "[values]")
            {
                int specimenCount = 0;
                QString value = "";
                input >> specimenCount;
                if (specimenCount && labelCount)
                {
                    for (int i=0; i<specimenCount; ++i)
                    {
                        for (int j=0; j<labelCount; ++j)
                        {
                            input >> value;
                            m_currentSpecimens->setLabelValue(i, labelNames[j], value);
                        }
                    }
                }
            }
        }
    }
    else
    {
        QString seperator = "";
        QString line = input.readLine();

        while ((line.left(1) == "'" || line.left(1) == "\"" || line == "" ) && !input.atEnd())
        {
            // ignore lines beginning with ' or " or empty lines
            line = input.readLine();
        }
        if (line.split("\t").size() > 1)
        {
            seperator = "\t";
        }
        else if (line.split(";").size() > 1)
        {
            seperator = ";";
        }
        else if (line.split(",").size() > 1)
        {
            seperator = ",";
        }
        else if (line.split(" ").size() > 1)
        {
            seperator = " ";
            while (line.contains("  "))
            {
                line.replace("  "," ");
            }
        }

        if (seperator != "")
        {
            labelNames = line.split(seperator);
            labelCount = labelNames.size();
        }
        else
        {
            labelNames.push_back(line);
            labelCount = 1;
        }

        int row=0;

        while (!input.atEnd())
        {
            line = input.readLine();
            // ignore lines beginning with ' or " or empty lines
            while ((line.left(1) == "'" || line.left(1) == "\"" || line == "") && !input.atEnd())
            {
                line = input.readLine();
            }
            if (labelCount == 1)
            {
                m_currentSpecimens->setLabelValue(row, labelNames[0], line);
                row++;
            }
            else
            {
                if (seperator == ";")
                {
                    // replace , with .
                    line.replace(",", ".");
                }
                else if (seperator == " ")
                {
                    // eliminate double spaces
                    while (line.contains("  "))
                    {
                        line.replace("  "," ");
                    }
                }
                QStringList values = line.split(seperator);
                for (int i=0; i<values.size(); i++)
                {
                    if (values[i]!="")
                    {
                        m_currentSpecimens->setLabelValue(row, labelNames[i], values[i]);
                    }
                }
                row++;
            }
        }
        Logger::getInstance()->log(QString("[Import Node] Imported (%1) Specimen Labels").arg(labelCount));
        setOutputPort0(m_currentSpecimens);
    }
}

void ImportNode::importLmkTags()
{
    QSettings settings("Evan");
    QStringList fileSettings = settings.value("lastDirectory").toStringList();
    QString fsLastDir = "";
    if( fileSettings.size() > 0 )
        fsLastDir = fileSettings[0];

    QFile file(lineEdit_4->text());
    if (!file.exists())
        return;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream input(&file);

    //REDO:m_specimens->clearLmkTags();
    QString keyword;
    int tagCount = 0;
    Logger::getInstance()->log("[Import Node] Importing Landmark tags...");
    while (!input.atEnd())
    {
        input >> keyword;
        if (keyword.toLower() == "[names]")
        {
            input >> tagCount;
            for (int i=0; i<tagCount; ++i)
            {
                input >> keyword;
                //REDO:m_specimens->addLmkTagName(keyword);
            }
        }
        else if (keyword.toLower() == "[values]")
        {
            int specimenCount = 0, value = 0, lmkCount = 0;
            input >> specimenCount >> lmkCount;
            if (specimenCount && tagCount && lmkCount)
            {
                Matrix<bool> tagValues(lmkCount,tagCount);
                for (int i=0; i<specimenCount; ++i)
                {
                    input >> keyword; //For the Specimen Name (UNUSED)
                    for (int j=0; j<lmkCount; ++j)
                        for (int k=0; k<tagCount; ++k)
                        {
                            input >> value;
                            tagValues[j][k] = value;
                        }
                    //REDO: m_specimens->setTagValues(i, tagValues);
                }
            }
        }
    }
    Logger::getInstance()->log(QString("[Import Node] Imported (%1) Landmark tags").arg(tagCount));
    setOutputPort0(m_specimens);
}

void ImportNode::importLmkPairs()
{
    QSettings settings("Evan");
    QStringList fileSettings = settings.value("lastDirectory").toStringList();
    QString fsLastDir = "";
    if( fileSettings.size() > 0 )
        fsLastDir = fileSettings[0];

    QString filename = lineEdit_8->text();
    if (filename == "")
    {
        return;
    }

    QFile file(filename);
    if (!file.exists())
    {
        QString msg = "[Import Node] File not found: " + filename;
        Logger::getInstance()->log(msg, Logger::WARNING);
        return;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString msg = "[Import Node] Error opening file: " + filename;
        Logger::getInstance()->log(msg, Logger::WARNING);
        return;
    }
    QTextStream input(&file);

    if (input.atEnd())
    {
        QString msg = "[Import Node] No data in file: " + filename;
        Logger::getInstance()->log(msg, Logger::WARNING);
        return;
    }

    Logger::getInstance()->log("[Import Node] Importing landmark pairs ...");

    vector<int> pairs;
    int x;
    while (!input.atEnd())
    {
        input >> x;
        pairs.push_back(x);
    }
    if (!pairs.empty())
    {
        QPair<int,int> pair;
        for (unsigned int i=0;i<pairs.size()/2;i++)
        {
            pair.first = pairs[i*2];
            pair.second = pairs[i*2+1];

            m_currentSpecimens->addLandmarkPair(pair);
        }
    }

    m_currentSpecimens->initialize("Imported Specimens");
    setOutputPort0(m_currentSpecimens);
}

void ImportNode::browseDatafile()
{
    QSettings settings("Evan");
    QStringList fileSettings = settings.value("lastDirectory").toStringList();
    QString fsLastDir = "";
    if( fileSettings.size() > 0 )
        fsLastDir = fileSettings[0];

    QString str;
    if (radioButton->isChecked())
    {
        // Raw text file
        str = QFileDialog::getOpenFileName(this, tr("Import data from file"),
                                           fsLastDir,
                                           tr("Raw text file (*.txt ; *.csv)"));
    }
    else if (radioButton_2->isChecked())
    {
        // Morphologika file
        str = QFileDialog::getOpenFileName(this, tr("Import data from file"),
                                           fsLastDir,
                                           tr("Morphologika file (*.txt)"));
    }
    else if (radioButton_3->isChecked())
    {
        // NTS file
        str = QFileDialog::getOpenFileName(this, tr("Import data from file"),
                                           fsLastDir,
                                           tr("NTSYSpc file (*.nts)"));
    }
    else if (radioButton_4->isChecked())
    {
        // TPS file
        str = QFileDialog::getOpenFileName(this, tr("Import data from file"),
                                           fsLastDir,
                                           tr("TPS file (*.tps)"));
    }
    else
    {
        str = QFileDialog::getOpenFileName(this, tr("Import data from file"),
                                           fsLastDir,
                                           tr("All files (*)\nRaw text (*.txt ; *.csv)\nMorphologika (*.txt)\nNTSYSpc (*.nts)\nTSP Series (*.tps)\nTempland tableau (*.ltb)"));
    }

    if (str != "")
    {
        lineEdit->setText(str);

        if(str.contains("/"))
        {
            QString dirName = str.left(str.lastIndexOf("/"));
            settings.setValue("lastDirectory",dirName);
        }
        else if(str.contains("\\"))
        {
            QString dirName = str.left(str.lastIndexOf("\\"));
            settings.setValue("lastDirectory",dirName);
        }
    }
}

void ImportNode::browseLabelsFile()
{
    QSettings settings("Evan");
    QStringList fileSettings = settings.value("lastDirectory").toStringList();
    QString fsLastDir = "";
    if( fileSettings.size() > 0 )
        fsLastDir = fileSettings[0];

    QString str = QFileDialog::getOpenFileName(this, tr("Import a Labels File"),
                  fsLastDir,
                  tr("Labels (*.txt ; *.csv ; *.nts ; *.lbl)"));

    if (str != "")
    {
        lineEdit_2->setText(str);

        if(str.contains("/"))
        {
            QString dirName = str.left(str.lastIndexOf("/"));
            settings.setValue("lastDirectory",dirName);
        }
        else if(str.contains("\\"))
        {
            QString dirName = str.left(str.lastIndexOf("\\"));
            settings.setValue("lastDirectory",dirName);
        }
    }
}

void ImportNode::browseAddVarsFile()
{
    QSettings settings("Evan");
    QStringList fileSettings = settings.value("lastDirectory").toStringList();
    QString fsLastDir = "";
    if( fileSettings.size() > 0 )
        fsLastDir = fileSettings[0];

    QString str = QFileDialog::getOpenFileName(this,
                  tr("Import Additional Variables"),
                  fsLastDir,
                  tr("Additional variables (*.txt ; *.csv ; *.nts)"));

    if (str != "")
    {
        lineEdit_3->setText(str);

        if(str.contains("/"))
        {
            QString dirName = str.left(str.lastIndexOf("/"));
            settings.setValue("lastDirectory",dirName);
        }
        else if(str.contains("\\"))
        {
            QString dirName = str.left(str.lastIndexOf("\\"));
            settings.setValue("lastDirectory",dirName);
        }
    }
}

void ImportNode::browseLmkTagsFile()
{
    QSettings settings("Evan");
    QStringList fileSettings = settings.value("lastDirectory").toStringList();
    QString fsLastDir = "";
    if( fileSettings.size() > 0 )
        fsLastDir = fileSettings[0];

    QString str = QFileDialog::getOpenFileName(this, tr("Import Landmark Tags File"),
                  fsLastDir,
                  tr("Landmark tags (*.tag)"));

    if (str != "")
    {
        lineEdit_4->setText(str);
        m_lmkTagsFileChanged = lineEdit_4->text() != "";

        if(str.contains("/"))
        {
            QString dirName = str.left(str.lastIndexOf("/"));
            settings.setValue("lastDirectory",dirName);
        }
        else if(str.contains("\\"))
        {
            QString dirName = str.left(str.lastIndexOf("\\"));
            settings.setValue("lastDirectory",dirName);
        }
    }
}

void ImportNode::accept()
{
    if (isUpdated())
        emit dataReady();
    QDialog::accept();
}

void ImportNode::reject()
{
    fromString(m_settings);
    QDialog::reject();
}

QString ImportNode::toString() const
{
    QString nodeData = lineEdit->text() + "$$"
                       + lineEdit_2->text() + "$$"
                       + lineEdit_3->text() + "$$"
                       + lineEdit_4->text() + "$$";

    if (radioButton->isChecked())
        nodeData += "txt$$";
    else if (radioButton_2->isChecked())
        nodeData += "morphologika$$";
    else if (radioButton_3->isChecked())
        nodeData += "nts$$";
    else if (radioButton_4->isChecked())
        nodeData += "tps$$";
    else if (radioButton_7->isChecked())
        nodeData += "tableau$$";

    QString val;
    val.setNum(spinBox->value());
    nodeData += val + "$$";
    val.setNum(spinBox_2->value());
    nodeData += val + "$$";

    if (radioButton_5->isChecked())
    {
        nodeData += "3dim$$";
    }
    else
    {
        nodeData += "2dim$$";
    }
    nodeData += lineEdit_6->text();
    nodeData += "$$";
    nodeData += lineEdit_7->text();
    nodeData += "$$";
    nodeData += lineEdit_5->text();
    nodeData += "$$";
    if (checkBox_2->isChecked())
    {
        nodeData += "checked";
    }
    else
    {
        nodeData += "unchecked";
    }
    nodeData += "$$";
    val.setNum(spinBox_3->value());
    nodeData += val;
    nodeData += "$$";
    nodeData += lineEdit_8->text();
    nodeData += "$$";
    if (checkBox->isChecked())
    {
        nodeData += "checked";
    }
    else
    {
        nodeData += "unchecked";
    }
    nodeData += "$$";
    nodeData += lineEdit_9->text();
    nodeData += "$$";
    nodeData += lineEdit_10->text();
    nodeData += "$$";
    if (radioButton_10->isChecked())
        nodeData += "standard";
    else if (radioButton_8->isChecked())
        nodeData += "mip";
    else if (radioButton_9->isChecked())
        nodeData += "iso";

    return  nodeData;
}

void ImportNode::fromString(const QString& params)
{
    QStringList filePaths = params.split("$$");

    if (filePaths.count() > 14)
    {
        lineEdit->setText(filePaths[0]);
        lineEdit_2->setText(filePaths[1]);
        lineEdit_3->setText(filePaths[2]);
        lineEdit_4->setText(filePaths[3]);
        if (filePaths[4] == "txt")
        {
            radioButton->setChecked(true);
            enableFilePaths();
        }
        else if (filePaths[4] == "morphologika")
        {
            radioButton_2->setChecked(true);
            enableFilePaths();
        }
        else if (filePaths[4] == "nts")
        {
            radioButton_3->setChecked(true);
            enableFilePaths();
        }
        else if (filePaths[4] == "tps")
        {
            radioButton_4->setChecked(true);
            enableFilePaths();
        }
        else if (filePaths[4] == "tableau")
        {
            radioButton_7->setChecked(true);
            enableFilePaths();
        }

        m_lmkFileChanged = filePaths[0] != "";
        m_labelsFileChanged = filePaths[1] != "";
        m_addVarsChanged = filePaths[2] != "";
        m_lmkTagsFileChanged = filePaths[3] != "";

        spinBox->setValue(filePaths[5].toInt());
        spinBox_2->setValue(filePaths[6].toInt());

        if (filePaths[7] == "3dim")
        {
            radioButton_5->setChecked(true);
        }
        else
        {
            radioButton_6->setChecked(true);
        }

        if (radioButton->isChecked())
        {
            enableRawTextOptions();
        }
        else
        {
            disableRawTextOptions();
        }
        lineEdit_6->setText(filePaths[8]);
        lineEdit_7->setText(filePaths[9]);

        lineEdit_5->setText(filePaths[10]);
        checkBox_2->setChecked(filePaths[11] == "checked");
        spinBox_3->setValue(filePaths[12].toInt());

        m_surfaceFileChanged = (filePaths[8] != "");
        m_matrixFileChanged = (filePaths[10] != "");

        lineEdit_8->setText(filePaths[13]);
        m_lmkPairsFileChanged = filePaths[13] != "";
        checkBox->setChecked(filePaths[14] == "checked");

        if (filePaths.count() > 16)
        {
            lineEdit_9->setText(filePaths[15]);
            lineEdit_10->setText(filePaths[16]);
            if (filePaths[17] == "standard")
                radioButton_10->setChecked(true);
            else if (filePaths[17] == "mip")
                radioButton_8->setChecked(true);
            else if (filePaths[17] == "iso")
                radioButton_9->setChecked(true);
        }
    }
}

void ImportNode::enableFilePaths()
{
    label_2->setDisabled(false);
    label_3->setDisabled(false);
    lineEdit_2->setDisabled(false);
    lineEdit_3->setDisabled(false);
    pushButton_2->setDisabled(false);
    pushButton_3->setDisabled(false);
    m_filePathsDisabled = false;
}

void ImportNode::disableFilePaths()
{
    label_2->setDisabled(true);
    label_3->setDisabled(true);
    lineEdit_2->setDisabled(true);
    lineEdit_3->setDisabled(true);
    pushButton_2->setDisabled(true);
    pushButton_3->setDisabled(true);
    m_filePathsDisabled = true;
}

void ImportNode::enableRawTextOptions()
{
    groupBox_2->setDisabled(false);

    m_lmkFileChanged = lineEdit->text() != "";
    m_labelsFileChanged = lineEdit_2->text() != "";
    m_addVarsChanged = lineEdit_3->text() != "";
    m_lmkTagsFileChanged = lineEdit_4->text() != "";
    m_lmkPairsFileChanged = lineEdit_8->text() != "";
}

void ImportNode::disableRawTextOptions()
{
    groupBox_2->setDisabled(true);

    m_lmkFileChanged = lineEdit->text() != "";
    m_labelsFileChanged = lineEdit_2->text() != "";
    m_addVarsChanged = lineEdit_3->text() != "";
    m_lmkTagsFileChanged = lineEdit_4->text() != "";
    m_lmkPairsFileChanged = lineEdit_8->text() != "";
}

void ImportNode::browseForSurface()
{
    QString extensions;
    extensions = "All formats (*.*)\n";
    extensions += "3D Studio (*.3ds)\n";
    extensions += "LightWave Object (*.lwo)\n";
    extensions += "LightWave 3D Scene (*.lws)\n";
    extensions += "MD2 file (*.md2)\nMDL file (*.mdl)\n";
    extensions += "OpenSceneGraph (*.osg)\n";
    extensions += "Wavefront (*.obj)\n";
    extensions += "Polygon file (*.ply)\n";
    extensions += "Stereolithography (*.stl)\n";
    extensions += "Edgewarp Surface (*.sur)\n";
    extensions += "Morphologika Polygons (*.txt)\n";
    extensions += "x (*.x)";

    QSettings settings("Evan");
    QStringList fileSettings = settings.value("lastDirectory").toStringList();
    QString fsLastDir = "";
    if( fileSettings.size() > 0 )
        fsLastDir = fileSettings[0];

    QString fileName = QFileDialog::getOpenFileName(this, tr("Locate a surface file"),
                       fsLastDir,
                       extensions);
    if (fileName != "" && !fileName.isEmpty())
    {
        lineEdit_6->setText(fileName);

        if(fileName.contains("/"))
        {
            QString dirName = fileName.left(fileName.lastIndexOf("/"));
            settings.setValue("lastDirectory",dirName);
        }
        else if(fileName.contains("\\"))
        {
            QString dirName = fileName.left(fileName.lastIndexOf("\\"));
            settings.setValue("lastDirectory",dirName);
        }
    }
}

void ImportNode::browseForTexture()
{
    QSettings settings("Evan");
    QStringList fileSettings = settings.value("lastDirectory").toStringList();
    QString fsLastDir = "";
    if( fileSettings.size() > 0 )
        fsLastDir = fileSettings[0];

    QString extensions;
    extensions = "All formats (*.*)\n";
    extensions += "Portable Network Graphics (*.png)\n";
    extensions += "Joint Photographic Experts Group (*.jpg *.jpeg)\n";
    extensions += "Windows Bitmap (*.bmp)";

    QString fileName = QFileDialog::getOpenFileName(this, tr("Locate a texture file"),
                       fsLastDir,
                       extensions);
    if (fileName != "" && !fileName.isEmpty())
    {
        lineEdit_7->setText(fileName);

        if(fileName.contains("/"))
        {
            QString dirName = fileName.left(fileName.lastIndexOf("/"));
            settings.setValue("lastDirectory",dirName);
        }
        else if(fileName.contains("\\"))
        {
            QString dirName = fileName.left(fileName.lastIndexOf("\\"));
            settings.setValue("lastDirectory",dirName);
        }
    }
}

void ImportNode::browseForMatrix()
{
    QSettings settings("Evan");
    QStringList fileSettings = settings.value("lastDirectory").toStringList();
    QString fsLastDir = "";
    if( fileSettings.size() > 0 )
        fsLastDir = fileSettings[0];

    QString fileName = QFileDialog::getOpenFileName(this, tr("Import data matrix from file"),
                       fsLastDir,
                       tr("All files (*)\nText files (*.txt ; *.csv) \nNTSYSpc files (*.nts)"));

    if (fileName != "" && !fileName.isEmpty())
    {
        lineEdit_5->setText(fileName);

        if(fileName.contains("/"))
        {
            QString dirName = fileName.left(fileName.lastIndexOf("/"));
            settings.setValue("lastDirectory",dirName);
        }
        else if(fileName.contains("\\"))
        {
            QString dirName = fileName.left(fileName.lastIndexOf("\\"));
            settings.setValue("lastDirectory",dirName);
        }
    }
}

void ImportNode::browseForLmkPairs()
{
    QSettings settings("Evan");
    QStringList fileSettings = settings.value("lastDirectory").toStringList();
    QString fsLastDir = "";
    if( fileSettings.size() > 0 )
        fsLastDir = fileSettings[0];

    QString fileName = QFileDialog::getOpenFileName(this, tr("Import landmark pairs from file"),
                       fsLastDir,
                       tr("All files (*)\nText files (*.txt)"));

    if (fileName != "" && !fileName.isEmpty())
    {
        lineEdit_8->setText(fileName);

        if(fileName.contains("/"))
        {
            QString dirName = fileName.left(fileName.lastIndexOf("/"));
            settings.setValue("lastDirectory",dirName);
        }
        else if(fileName.contains("\\"))
        {
            QString dirName = fileName.left(fileName.lastIndexOf("\\"));
            settings.setValue("lastDirectory",dirName);
        }
    }
}

void ImportNode::browseForVolume()
{
    QSettings settings("Evan");
    QStringList fileSettings = settings.value("lastDirectory").toStringList();
    QString fsLastDir = "";
    if( fileSettings.size() > 0 )
        fsLastDir = fileSettings[0];

    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),fsLastDir);

    if (dir != "" && !dir.isEmpty())
    {
        lineEdit_9->setText(dir);

        if(dir.contains("/"))
        {
            QString dirName = dir.left(dir.lastIndexOf("/"));
            settings.setValue("lastDirectory",dirName);
        }
        else if(dir.contains("\\"))
        {
            QString dirName = dir.left(dir.lastIndexOf("\\"));
            settings.setValue("lastDirectory",dirName);
        }
    }
}

void ImportNode::browseForTransferFunction()
{
    QSettings settings("Evan");
    QStringList fileSettings = settings.value("lastDirectory").toStringList();
    QString fsLastDir = "";
    if( fileSettings.size() > 0 )
        fsLastDir = fileSettings[0];

    QString fileName = QFileDialog::getOpenFileName(this, tr("Import transfer function from file"),
                       fsLastDir,
                       tr("All files (*)\nText files (*.txt)"));

    if (fileName != "" && !fileName.isEmpty())
    {
        lineEdit_10->setText(fileName);

        if(fileName.contains("/"))
        {
            QString dirName = fileName.left(fileName.lastIndexOf("/"));
            settings.setValue("lastDirectory",dirName);
        }
        else if(fileName.contains("\\"))
        {
            QString dirName = fileName.left(fileName.lastIndexOf("\\"));
            settings.setValue("lastDirectory",dirName);
        }
    }
}

