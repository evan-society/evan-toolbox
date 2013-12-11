#ifndef IMPORT_H_
#define IMPORT_H_

#include <vector>
#include <complex>
#include <iostream>
#include <fstream>

#include <Matrix.h>


using namespace std;

//! The GMMData class reads in data from a file and stores
class GMMData
{

public:

    //! Constructor
    GMMData();

    //! Destructor
    ~GMMData();

    //! Read data from a raw text file
    //! \param filename name of file
    int ReadRawTextFile(const char * filename);

    //! Read data from a Morphologika file
    //! \param filename name of file
    int ReadMorphologikaFile(const char * filename);

    //! Read data from an NTSYSpc file
    //! \param filename name of file
    int ReadNTSFile(const char * filename);

    //! Read data from a TPS file
    //! \param filename name of file
    int ReadTPSFile(const char * filename);

    //! Read data from a MorphoJ file
    //! \param filename name of file
    int ReadMorphoJFile(const char * filename);

    //! Get number of specimens
    //! \return the number of specimens
    int GetNumberOfSpecimens();

    //! Get number of landmarks
    //! \return the number of landmarks
    int GetNumberOfLandmarks();

    //! Get number of dimensions
    //! \return the number of dimensions
    int GetNumberOfDimensions();

    void GetRawData(std::vector<std::vector<double> > *rawData)
    {
        *rawData = m_rawdata;
    }

    void GetRawData(Matrix<double> *rawData);

    void GetWireframe(std::vector<std::vector<int> > *wireframe)
    {
        *wireframe = m_wireframe;
    }
    void GetPolygons(std::vector<std::vector<int> > *polygons)
    {
        *polygons = m_polygons;
    }
    void GetGroups(std::vector<string> *groups)
    {
        *groups = m_groups;
    }
    void GetNames(std::vector<string> *names)
    {
        *names = m_names;
    }
    void GetComments(std::vector<string> *comments)
    {
        *comments = m_comments;
    }
    void GetLabelnames(std::vector<string> *labelnames)
    {
        *labelnames = m_labelnames;
    }
    void GetLabelvalues(std::vector<std::vector<string> > *labelvalues)
    {
        *labelvalues = m_labelvalues;
    }
    void SetGroups(std::vector<string> groups)
    {
        m_groups = groups;
    }
    void SetNames(std::vector<string> names)
    {
        m_names = names;
    }
    void SetComments(std::vector<string> comments)
    {
        m_comments = comments;
    }
    void SetLabelnames(std::vector<string> labelnames)
    {
        m_labelnames = labelnames;
    }
    void SetLabelvalues(std::vector<std::vector<string> > labelvalues)
    {
        m_labelvalues = labelvalues;
    }

private:

    int m_dimensions;       //!< Number of dimensions
    int m_landmarks;        //!< Number of landmarks
    int m_individuals;      //!< Number of specimens

    std::vector<std::vector<double> > m_rawdata;  //!< Raw data points
    std::vector<Matrix<double> > m_coordinates;

    std::vector<std::vector<int> > m_wireframe;   //!< Wireframe
    std::vector<std::vector<int> > m_polygons;    //!< Polygons

    std::vector<string> m_groups;                //!< Name of group for each individul
    std::vector<string> m_names;                 //!< One name for each specimen
    std::vector<string> m_comments;              //!< Comments from project file
    std::vector<string> m_labelnames;            //!< E.g. sex, age, ...
    std::vector<std::vector<string> > m_labelvalues;  //!< E.g. male, female, adult, ...
    std::vector<string> m_images;                //!< Image of specimen
    std::vector<string> m_rowLabels;             //!< Row labels of NTS file
    std::vector<string> m_colLabels;             //!< Column labels of NTS file
};



#endif // IMPORT_H

