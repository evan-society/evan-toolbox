#include "import.h"
#include <algorithm>

//#include <ew/dataflow_form3.h>

//static EW_Dataflow nw;

// routine to split a string into vector of strings
inline std::vector<string> split( const string& s, const string& f )
{
    std::vector<string> temp;
    if ( f.empty() )
    {
        temp.push_back( s );
        return temp;
    }
    typedef string::const_iterator iter;
    const iter::difference_type f_size(distance( f.begin(), f.end()));
    iter i(s.begin());
    for ( iter pos; (pos = search( i , s.end(), f.begin(), f.end())) != s.end(); )
    {
        if (!string(i, pos).empty())
        {
            temp.push_back(string(i, pos));
        }
        advance(pos, f_size);
        i = pos;
    }
    if (!string(i, s.end()).empty())
    {
        temp.push_back(string(i, s.end()));
    }
    return temp;
}

GMMData::GMMData()
{
}

GMMData::~GMMData()
{
}

int ReadXMLFile(const char * filename)
{
    /*
        EW_Dataflow_Form3 *node = new EW_Dataflow_Form3(&nw);
        node->read_file(filename);
        node->delete_when_unreferenced();
    */
    // not finished yet
    // needs linking with libxml2

    return 0;
}

int GMMData::ReadRawTextFile(const char * filename)
{
    // get the data from file
    ifstream datafile (filename);

    // check if file exists
    if (datafile.fail())
    {
        cout << "Error: file """ << filename << """ not found" << endl;
        return 1;
    }

    // clear existing data
    m_rawdata.clear();
    m_groups.clear();
    m_names.clear();
    m_labelnames.clear();
    m_labelvalues.clear();
    m_polygons.clear();
    m_wireframe.clear();

    // we read dimension count from length of lines
    m_dimensions = 0;

    // we read landmark count from first line
    m_landmarks = 0;

    string strline;

    // first find number of landmarks and dimensions
    while (m_dimensions == 0)
    {
        // read next line
        getline(datafile,strline);
        if (strline.substr(0,1) != "\"" && strline.substr(0,1) != "'")
        {
            // seperator can be space or tab
            std::vector<string> strvec1 = split(strline," ");
            std::vector<string> strvec2 = split(strline,"\t");
            if (strvec1.size()<strvec2.size())
            {
                strvec1 = strvec2;
            }
            if (strvec1.size() == 1 && m_landmarks == 0)
            {
                m_landmarks = atoi(strvec1[0].c_str());
            }
            else if (strvec1.size() == 2 || strvec1.size() == 3)
            {
                m_dimensions = (int)strvec1.size();
                std::vector<double> tempvec;
                for (int i=0;i<m_dimensions;i++)
                {
                    tempvec.push_back(atof(strvec1[i].c_str()));
                }
                m_rawdata.push_back(tempvec);
            }
        }
    }

    // read raw data into matrix
    while (!datafile.eof())
    {
        getline(datafile,strline);
        if (strline.substr(0,1) != "\"" && strline.substr(0,1) != "'")
        {
            std::vector<string> strvec1 = split(strline," ");
            std::vector<string> strvec2 = split(strline,"\t");
            if (strvec1.size()<strvec2.size())
            {
                strvec1 = strvec2;
            }
            if ((int)strvec1.size() == m_dimensions)
            {
                std::vector<double> tempvec;
                for (int i=0;i<m_dimensions;i++)
                {
                    tempvec.push_back(atof(strvec1[i].c_str()));
                }
                m_rawdata.push_back(tempvec);
            }
        }
    }

    // find number of individuals
    if (m_landmarks == 0)
    {
        m_landmarks = (int)m_rawdata.size();
        m_individuals = 1;
    }
    else
    {
        m_individuals = (int)m_rawdata.size() / m_landmarks;
    }
    datafile.close();
    return 0;
}

int GMMData::ReadMorphologikaFile(const char * filename)
{
    // get the data from file
    ifstream datafile (filename);

    // check if file exists
    if (datafile.fail())
    {
        cout << "Error: file """ << filename << """ not found" << endl;
        return 1;
    }

    int i=0;
    int j=0;
    string str;

    // clear existing data
    m_rawdata.clear();
    m_groups.clear();
    m_names.clear();
    m_labelnames.clear();
    m_labelvalues.clear();
    m_polygons.clear();
    m_wireframe.clear();

    std::vector<double> tempraw(3);
    std::vector<int> temppoly(3);
    std::vector<int> tempwire(2);

    // put data into data matrix
    while (datafile >> str)
    {
startloop:

        if (str.substr(0,1) == "\"" || str.substr(0,1) == "'")
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
            m_comments.push_back(str.substr(startspace,str.length()));
        }
        else
        {
            transform(str.begin(), str.end(), str.begin(), ::tolower);
        }
        if (str.substr(0,13) == "[individuals]")
        {
            datafile >> m_individuals;
        }
        if (str.substr(0,11) == "[landmarks]")
        {
            datafile >> m_landmarks;
        }
        if (str.substr(0,12) == "[dimensions]")
        {
            datafile >> m_dimensions;
        }
        if (str.substr(0,7) == "[names]")
        {
            while (datafile >> str)
            {
                if (str.substr(0,1) == "[" || datafile.eof())
                {
                    if (m_names.size() != (unsigned int)m_individuals)
                    {
                        cout << "Error: number of names have to be as many as individuals" << endl;
                    }
                    // stop if we have reached next section
                    goto startloop;
                }
                if (str.substr(0,1) != "'")
                {
                    string strline;
                    getline(datafile,strline);
                    str.append(strline);
                    m_names.push_back(str);
                }
            }
        }
        if (str.substr(0,8) == "[labels]")
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
                    m_labelnames.push_back(str);
                }
            }
        }
        if (str.substr(0,13) == "[labelvalues]")
        {
            std::vector<string> tmpstrvec;

            while (datafile >> str)
            {
                if (str.substr(0,1) != "'" && str.substr(0,1) != "[")
                {
                    tmpstrvec.push_back(str);
                }
                if (str.substr(0,1) == "[" || datafile.eof())
                {
                    int k=0;
                    int numlabels = tmpstrvec.size()/m_individuals;

                    if (tmpstrvec.size() != (unsigned int)(m_individuals * numlabels))
                    {
                        cout << "Error: incorrect number of labelvalues" << endl;
                        goto startloop;
                    }
                    for (int m=0;m<m_individuals;m++)
                    {
                        std::vector<string> tmpvec;
                        for (int l=0;l<numlabels;l++)
                        {
                            tmpvec.push_back(tmpstrvec[k++]);
                        }
                        m_labelvalues.push_back(tmpvec);
                    }

                    // stop if we have reached next section
                    goto startloop;
                }
            }
        }
        if (str.substr(0,8) == "[groups]")
        {
            while (datafile >> str)
            {
                if (str.substr(0,1) == "[")
                {
                    // stop if we have reached next section
                    goto startloop;
                }
                int groupsize;
                datafile >> groupsize;
                for(int ii=0;ii<groupsize;ii++)
                {
                    m_groups.push_back(str);
                }
            }
        }
        if (str.substr(0,11) == "[rawpoints]")
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
                    tempraw[j] = atof(str.c_str());
                    j++;
                    if (j== m_dimensions)
                    {
                        j=0;
                        i++;
                        m_rawdata.push_back(tempraw);
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
                        m_wireframe.push_back(tempwire);
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
                        m_polygons.push_back(temppoly);
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

    return 0;
}

int GMMData::ReadTPSFile(const char * filename)
{
    // get the data from file
    ifstream datafile (filename);

    // check if file exists
    if (datafile.fail())
    {
        cout << "Error: file """ << filename << """ not found" << endl;
        return 1;
    }

    // clear existing data
    m_rawdata.clear();
    m_groups.clear();
    m_names.clear();
    m_labelnames.clear();
    m_labelvalues.clear();
    m_polygons.clear();
    m_wireframe.clear();

    Matrix<double> tempmat;
    std::vector<Matrix<double> > tempvec;
    m_landmarks = 0;

    while (!datafile.eof())
    {
        string strline;
        getline(datafile,strline);
        transform(strline.begin(), strline.end(), strline.begin(), ::tolower);

        int eq = strline.find("=");
        if (eq>0)
        {
            if (strline.substr(0,eq)=="lm")
            {
                string tmpstr = strline.substr(eq+1);

                double tmpdbl;
                int landmarks = atoi(tmpstr.c_str());
                if (m_landmarks == 0)
                {
                    m_landmarks = landmarks;
                }
                else if (landmarks != m_landmarks)
                {
                    cout << "Error: number of landmarks have to be the same for all specimens" << endl;
                    return 1;
                }

                m_landmarks = landmarks;
                tempmat.ReSize(landmarks,2);

                for (int i=0;i<landmarks;i++)
                {
                    datafile >> tmpdbl;
                    tempmat[i][0] = tmpdbl;
                    datafile >> tmpdbl;
                    tempmat[i][1] = tmpdbl;
                }
            }
            if (strline.substr(0,eq)=="image")
            {
                m_images.push_back(strline.substr(eq+1));
            }
            if (strline.substr(0,eq)=="scale")
            {
                double scale = atof(strline.substr(eq+1).c_str());
                if (scale > 0.0)
                {
                    tempmat *= scale;
                }
                tempvec.push_back(tempmat);
            }
            if (strline.substr(0,eq)=="id")
            {
                m_names.push_back(strline.substr(eq+1));
            }
        }
    }

    m_dimensions = 2;
    m_individuals = tempvec.size();
    m_rawdata.resize(m_individuals*m_landmarks);
    int k=0;
    for (unsigned int i=0;i<tempvec.size();i++)
    {
        tempmat = tempvec[i];
        for (int j=0;j<m_landmarks;j++)
        {
            m_rawdata[k*m_landmarks+j].push_back(tempmat[j][0]);
            m_rawdata[k*m_landmarks+j].push_back(tempmat[j][1]);
        }
        k++;
    }
    datafile.close();
    return 0;
}

int GMMData::ReadNTSFile(const char * filename)
{
    // get the data from file
    ifstream datafile (filename);

    // check if file exists
    if (datafile.fail())
    {
        cout << "Error: file """ << filename << """ not found" << endl;
        return 1;
    }

    // clear existing data
    m_rawdata.clear();
    m_groups.clear();
    m_names.clear();
    m_labelnames.clear();
    m_labelvalues.clear();
    m_polygons.clear();
    m_wireframe.clear();

    Matrix<double> tempmat;
    std::vector<Matrix<double> > tempvec;
    string strline;

    // put data into data matrix
    while (!datafile.eof())
    {
        getline(datafile,strline);
        if (strline.substr(0,1) == "\"" || strline.substr(0,1) == "'")
        {
            // get comments at the top of the file

            // ignore spaces and tabs in the beginning of the string
            unsigned int startspace = 1;
            while ( startspace < strline.length() )
            {
                if ( (strline[ startspace ] == ' ') || (strline[ startspace ] == '\t') )
                {
                    startspace++;
                }
                else
                {
                    break;
                }
            }
            m_comments.push_back(strline.substr(startspace));
        }
        else if (strline.substr(0,1) == "1")
        {
            transform(strline.begin(), strline.end(), strline.begin(), ::tolower);

            // info from first line
            stringstream sstr(strline);
            string tmpstr;
            int rowLabels = 0;
            int colLabels = 0;

            // stream line
            sstr >> tmpstr; // 1
            if (tmpstr != "1")
            {
                cout<<"first line must be of the form:\n1 10L 20L 0"<<endl;
                return 1;
            }

            sstr >> tmpstr; // number of individuals
            if (tmpstr.substr(tmpstr.length()-1) == "l")
            {
                rowLabels = atoi(tmpstr.substr(0,tmpstr.length()-1).c_str());
                m_individuals = rowLabels;
            }
            else
            {
                m_individuals = atoi(tmpstr.c_str());
            }
            sstr >> tmpstr; // number of landmarks times dimensions
            if (tmpstr.substr(tmpstr.length()-1) == "l")
            {
                colLabels = atoi(tmpstr.substr(0,tmpstr.length()-1).c_str());
                m_landmarks = colLabels;
            }
            else
            {
                m_landmarks = atoi(tmpstr.c_str());
            }

            sstr >> tmpstr; // 0
            if (tmpstr != "0")
            {
                cout<<"first line must be of the form:\n1 10L 20L 0"<<endl;
                return 1;
            }

            // remove spaces from line
            string::iterator end_pos = remove(strline.begin(), strline.end(), ' ');
            strline.erase(end_pos, strline.end());

            // find if this is 3 dimensons
            int dim3 = strline.find("dim=3");
            if (dim3>0)
            {
                m_dimensions = 3;
            }
            else
            {
                m_dimensions = 2;
            }

            m_landmarks /= m_dimensions;

            for (int i=0;i<rowLabels;i++)
            {
                datafile>>tmpstr;
                m_rowLabels.push_back(tmpstr);
                m_names.push_back(tmpstr);
            }

            for (int i=0;i<colLabels;i++)
            {
                datafile>>tmpstr;
                m_colLabels.push_back(tmpstr);
            }

            m_rawdata.resize(m_individuals*m_landmarks);
            int i=0;
            int j=0;

            double tempdbl;

            tempmat.ReSize(m_landmarks,m_dimensions);

            while (datafile>>tempdbl)
            {
                tempmat[i][j] = tempdbl;
                j++;
                if (j==m_dimensions)
                {
                    i++;
                    j=0;
                    if (i==m_landmarks)
                    {
                        tempvec.push_back(tempmat);
                        tempmat.Zeros();
                        i=0;
                    }
                }
            }
        }
    }

    m_rawdata.resize(m_individuals*m_landmarks);
    int k=0;
    for (unsigned int i=0;i<tempvec.size();i++)
    {
        tempmat = tempvec[i];
        for (int j=0;j<m_landmarks;j++)
        {
            for (int dim=0;dim<m_dimensions;dim++)
            {
                m_rawdata[k*m_landmarks+j].push_back(tempmat[j][dim]);
            }
        }
        k++;
    }
    datafile.close();
    return 0;
}

int GMMData::ReadMorphoJFile(const char * filename)
{
    // not implemented yet
    return 0;
}

int
GMMData::GetNumberOfSpecimens()
{
    return m_individuals;
}

int
GMMData::GetNumberOfLandmarks()
{
    return m_landmarks;
}

int
GMMData::GetNumberOfDimensions()
{
    return m_dimensions;
}

void
GMMData::GetRawData(Matrix<double> *rawData)
{
    rawData->ReSize(m_landmarks*m_individuals,m_dimensions);
    for (int i=0;i<m_individuals;i++)
    {
        for (int j=0;j<m_landmarks;j++)
        {
            for (int k=0;k<m_dimensions;k++)
            {
                (*rawData)[i*m_landmarks+j][k] = m_rawdata[i*m_landmarks+j][k];
            }
        }
    }
}
