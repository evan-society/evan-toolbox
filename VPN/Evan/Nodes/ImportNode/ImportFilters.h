

void find_and_replace( string &source, const string find, string replace )
{
    size_t j;
    for ( ; (j = source.find( find )) != string::npos ; )
    {
        source.replace( j, find.length(), replace );
    }
}

int ReadFromRawTextFile( QString filename,
                         int *individuals,
                         int *landmarks,
                         int *dimensions,
                         vector<vector<double> > *rawdata,
                         vector<vector<QString> > *stringdata)
{
    QFile file(filename);
    if (!file.exists())
    {
        QString msg = "[Import Node] File not found: " + filename;
        Logger::getInstance()->log(msg, Logger::WARNING);
        return 1;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString msg = "[Import Node] Error opening file: " + filename;
        Logger::getInstance()->log(msg, Logger::WARNING);
        return 1;
    }
    QTextStream input(&file);

    if (input.atEnd())
    {
        QString msg = "[Import Node] No data in file: " + filename;
        Logger::getInstance()->log(msg, Logger::WARNING);
        return 1;
    }

    // we read dimension count from length of lines
    int idimensions = 0;

    // we read landmark count from first line
    int ilandmarks = 0;

    string strline;

    QString seperator = "";
    QString line;// = input.readLine();

    // first find number of landmarks and dimensions
    while (idimensions == 0 && !input.atEnd())
    {
        // read next line
        line = input.readLine();

        if (line.left(1) != "\"" && line.left(1) != "'")
        {
            if (ilandmarks == 0)
            {
                bool success;
                ilandmarks = line.toInt(&success);
                if (!success)
                {
                    QString msg = "[Import Node] Error in file: " + filename;
                    Logger::getInstance()->log(msg, Logger::WARNING);
                    msg = "[Import Node] First line should contain number of landmarks";
                    Logger::getInstance()->log(msg, Logger::WARNING);
                    return 1;
                }
            }
            else if (idimensions == 0)
            {
                // seperator can be space or tab
                while (line.contains("  "))
                {
                    line.replace("  ", " ");
                }

                QStringList strvec1 = line.split(" ");
                QStringList strvec2 = line.split("\t");
                if (strvec1.size()<strvec2.size())
                {
                    strvec1 = strvec2;
                }
                if (strvec1.size() == 2 || strvec1.size() == 3)
                {
                    idimensions = (int)strvec1.size();
                    vector<double> tempvec;
                    vector<QString> qstrvec;
                    for (int i=0;i<idimensions;i++)
                    {
                        tempvec.push_back(strvec1[i].toDouble());
                        qstrvec.push_back(strvec1[i]);
                    }
                    rawdata->push_back(tempvec);
                    stringdata->push_back(qstrvec);
                }
            }
        }
    }

    if (idimensions == 0)
    {
        QString msg = "[Import Node] Error in file: " + filename;
        Logger::getInstance()->log(msg, Logger::WARNING);
        Logger::getInstance()->log("No landmark data found", Logger::WARNING);
        return 1;
    }

    // read raw data into matrix
    while (!input.atEnd())
    {
        // read next line
        line = input.readLine();

        if (line.left(1) != "\"" && line.left(1) != "'")
        {
            while (line.contains("  "))
            {
                line.replace("  ", " ");
            }
            QStringList strvec1 = line.split(" ");
            QStringList strvec2 = line.split("\t");
            if (strvec1.size()<strvec2.size())
            {
                strvec1 = strvec2;
            }
            if ((int)strvec1.size() == idimensions)
            {
                vector<double> tempvec;
                vector<QString> qstrvec;
                for (int i=0;i<idimensions;i++)
                {
                    tempvec.push_back(strvec1[i].toDouble());
                    qstrvec.push_back(strvec1[i]);
                }
                rawdata->push_back(tempvec);
                stringdata->push_back(qstrvec);
            }
        }
    }

    // find number of individuals
    if (ilandmarks == 0)
    {
        Logger::getInstance()->log("[Import Node] Number of landmarks not known", Logger::WARNING);
        *individuals = 0;
    }
    else
    {
        *individuals = (int)rawdata->size() / ilandmarks;
    }
    *landmarks = ilandmarks;
    *dimensions = idimensions;

    return 0;
}

int ReadFromRawTextFileRows( QString filename,
                             int *individuals,
                             int *landmarks,
                             int *dimensions,
                             vector<vector<double> > *rawdata,
                             vector<vector<QString> > *stringdata,
                             vector<string> *labelnames,
                             vector<vector<string> > *labelvalues,
                             bool firstLineHeaders)
{
    QFile file(filename);
    if (!file.exists())
    {
        QString msg = "[Import Node] File not found: " + filename;
        Logger::getInstance()->log(msg, Logger::WARNING);
        return 1;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString msg = "[Import Node] Error opening file: " + filename;
        Logger::getInstance()->log(msg, Logger::WARNING);
        return 1;
    }
    QTextStream input(&file);

    *individuals = 0;
    *landmarks = 0;

    if (input.atEnd())
    {
        QString msg = "[Import Node] No data in file: " + filename;
        Logger::getInstance()->log(msg, Logger::WARNING);
        return 1;
    }

    QString seperator = "";
    QString line = input.readLine();
    QStringList colLabels;

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

    int row=0;

    if (firstLineHeaders)
    {
        if (seperator != "")
        {
            colLabels = line.split(seperator);
            if (colLabels.size() < 6 )
            {
                QString msg = "[Import Node] Error in file: " + filename;
                Logger::getInstance()->log(msg, Logger::WARNING);
                Logger::getInstance()->log("data not recognised as landmark data", Logger::WARNING);
                *dimensions = 0;
                return 1;
            }
        }
        *landmarks = ( colLabels.size() / (*dimensions) );
    }
    else
    {
        QStringList values = line.split(seperator);
        if (values.size() < 6 )
        {
            QString msg = "[Import Node] Error in file: " + filename;
            Logger::getInstance()->log(msg, Logger::WARNING);
            Logger::getInstance()->log("data not recognised as landmark data", Logger::WARNING);
            *dimensions = 0;
            return 1;
        }
        vector<double> tempvec;
        vector<QString> strvec;
        int counter=0;
        for (int i=0; i<values.size(); i++)
        {
            bool success;

            values[i].toDouble(&success);

            if (values[i] != "")
            {
                tempvec.push_back(values[i].toDouble());
                strvec.push_back(values[i]);
                counter++;
            }
            if (counter == (*dimensions))
            {
                rawdata->push_back(tempvec);
                stringdata->push_back(strvec);
                tempvec.clear();
                strvec.clear();
                counter=0;
            }
        }
        row++;
        *landmarks = ( values.size() / (*dimensions) );
    }

    while (!input.atEnd())
    {
        line = input.readLine();

        // ignore lines beginning with ' or " or empty lines
        while ((line.left(1) == "'" || line.left(1) == "\"" || line == "") && !input.atEnd())
        {
            line = input.readLine();
        }
        if (seperator == " ")
        {
            // eliminate double spaces
            while (line.contains("  "))
            {
                line.replace("  "," ");
            }
        }
        QStringList values = line.split(seperator);

        int counter = 0;
        vector<double> tempvec;
        vector<QString> strvec;
        vector<string> templabels;
        bool foundDouble = false;

        for (int i=0; i<(int)values.size(); i++)
        {
            bool success;
            values[i].toDouble(&success);

            // if not success, assume first columns are labels
            if (success || !firstLineHeaders)
            {
                tempvec.push_back(values[i].toDouble());
                strvec.push_back(values[i]);
                counter++;

                if (counter == (*dimensions))
                {
                    rawdata->push_back(tempvec);
                    stringdata->push_back(strvec);
                    tempvec.clear();
                    strvec.clear();
                    counter=0;
                }
                foundDouble = true;
            }
            else if (!foundDouble)
            {
                templabels.push_back(values[i].toStdString());
            }
        }

        if (!templabels.empty())
        {
            labelvalues->push_back(templabels);
        }
        row++;
    }
    if (labelvalues->size() > 0)
    {
        for (int i=0;i<(int)(*labelvalues)[0].size();i++)
        {
            if(i<colLabels.size())
            {
                labelnames->push_back(colLabels[i].toStdString());
            }
        }
        *landmarks = (colLabels.size() - labelnames->size()) / (*dimensions);
    }

    (*individuals) = row;

    return 0;
}

int ReadFromMorphologikaFile( QString filename,
                              int *individuals,
                              int *landmarks,
                              int *dimensions,
                              vector<vector<double> > *rawdata,
                              vector<vector<QString> > *stringdata,
                              vector<string> *groups,
                              vector<string> *names,
                              vector<string> *labelnames,
                              vector<vector<string> > *labelvalues,
                              vector<vector<int> > *wireframe,
                              vector<vector<int> > *polygons,
                              vector<vector<int> > *pairs )
{
    // get the data from file
    ifstream datafile (filename.toAscii());

    // check if file exists
    if (datafile.fail())
    {
        QString msg = "[Import Node] File not found: " + filename;
        Logger::getInstance()->log(msg, Logger::WARNING);
        return 1;
    }

    int i=0;
    int j=0;
    string str;

    // clear existing data
    rawdata->clear();
    groups->clear();
    names->clear();
    labelnames->clear();
    labelvalues->clear();
    polygons->clear();
    wireframe->clear();
    pairs->clear();

    vector<double> tempraw(3);
    vector<QString> tempqstring(3);
    vector<int> temppoly(3);
    vector<int> tempwire(2);
    vector<int> temppair(2);

    int iindividuals = 0;
    int ilandmarks = 0;
    int idimensions = 0;

    // put data into data matrix
    while (datafile >> str && !datafile.eof())
    {
startloop:

        if (str.substr(0,1) == "\"" || str.substr(0,1) == "'")
        {
            // ignore comments at the top of the file
            string strline;
            getline(datafile,strline);
        }
        else
        {
            transform(str.begin(), str.end(), str.begin(), ::tolower);
        }
        if (str.substr(0,13) == "[individuals]")
        {
            datafile >> iindividuals;
        }
        if (str.substr(0,11) == "[landmarks]")
        {
            datafile >> ilandmarks;
        }
        if (str.substr(0,12) == "[dimensions]")
        {
            datafile >> idimensions;
        }
        if (str.substr(0,7) == "[names]")
        {
            while (datafile >> str)
            {
                if (str.substr(0,1) == "[")
                {
                    if (names->size() != (unsigned int)iindividuals)
                    {
                        QString msg = "[Import Node] Error in Morphologika file: " + filename;
                        Logger::getInstance()->log(msg, Logger::WARNING);
                        Logger::getInstance()->log("number of names have to be as many as individuals", Logger::WARNING);
                    }
                    // stop if we have reached next section
                    goto startloop;
                }
                if (str.substr(0,1) != "'")
                {
                    string strline;
                    getline(datafile,strline);
                    str.append(strline);
                    find_and_replace( str, "\n", "" );
                    find_and_replace( str, "\r", "" );
                    names->push_back(str);
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
                    find_and_replace( str, "\n", "" );
                    find_and_replace( str, "\r", "" );
                    labelnames->push_back(str);
                }
            }
        }
        if (str.substr(0,13) == "[labelvalues]")
        {
            vector<string> tmpstrvec;
            if ( labelnames->size() == 1)
            {
                while (getline(datafile,str))
                {
                    if (str.substr(0,1) == "[" || datafile.eof())
                    {
                        int k=0;
                        for (int m=0;m<iindividuals;m++)
                        {
                            vector<string> tmpvec;
                            tmpvec.push_back(tmpstrvec[k++]);
                            labelvalues->push_back(tmpvec);
                        }
                        goto startloop;
                    }
                    find_and_replace( str, "\n", "" );
                    find_and_replace( str, "\r", "" );
                    if(str != "" && str.substr(0,1) != "'")
                    {
                        tmpstrvec.push_back(str);
                    }
                }
            }
            else
            {
                while (datafile >> str)
                {
                    if (str.substr(0,1) == "[" || datafile.eof())
                    {
                        int k=0;
                        for (int m=0;m<iindividuals;m++)
                        {
                            vector<string> tmpvec;
                            for (unsigned int l=0;l<labelnames->size();l++)
                            {
                                tmpvec.push_back(tmpstrvec[k++]);
                            }
                            labelvalues->push_back(tmpvec);
                        }
                        // stop if we have reached next section
                        goto startloop;
                    }
                    find_and_replace( str, "\n", "" );
                    find_and_replace( str, "\r", "" );
                    if(str != "" && str.substr(0,1) != "'")
                    {
                        tmpstrvec.push_back(str);
                    }
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

                find_and_replace( str, "\n", "" );
                find_and_replace( str, "\r", "" );
                for (int ii=0;ii<groupsize;ii++)
                {
                    groups->push_back(str);
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
                    QString qstr(str.c_str());
                    tempqstring[j] = qstr;
                    tempraw[j] = qstr.toDouble();
                    j++;
                    if (j== idimensions)
                    {
                        j=0;
                        i++;
                        rawdata->push_back(tempraw);
                        stringdata->push_back(tempqstring);
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
                        wireframe->push_back(tempwire);
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
                        polygons->push_back(temppoly);
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
        if (str.substr(0,7) == "[pairs]")
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
                    temppair[j] = atoi(str.c_str());
                    j++;
                    if (j == 2)
                    {
                        j=0;
                        i++;
                        pairs->push_back(temppair);
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
    }
    datafile.close();

    *individuals = iindividuals;
    *landmarks = ilandmarks;
    *dimensions = idimensions;

    if( (iindividuals == 0) && (ilandmarks == 0) && (idimensions == 0) )
    {
        QString msg = "[Import Node] Error in file: " + filename;
        Logger::getInstance()->log(msg, Logger::WARNING);
        Logger::getInstance()->log("File contains no data or not a Morphologika file",Logger::WARNING);
    }

    return 0;
}

int ReadFromTPSFile( QString filename,
                     int *individuals,
                     int *landmarks,
                     int *dimensions,
                     vector<vector<double> > *rawdata,
                     vector<vector<QString> > *stringdata,
                     QStringList *stringscales,
                     vector<string> *names,
                     vector<string> *imagepaths)
{
    // get the data from file
    ifstream datafile (filename.toStdString().c_str());

    // check if file exists
    if (datafile.fail())
    {
        QString msg = "[Import Node] File not found: " + filename;
        Logger::getInstance()->log(msg, Logger::WARNING);
        return 1;
    }

    int lmks = 0;
    int indiv = 0;

    vector<double> scales;
    vector<QString> templist;

    rawdata->clear();
    stringdata->clear();
    stringscales->clear();
    names->clear();
    imagepaths->clear();

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
                int lm = atoi(tmpstr.c_str());
                if (lmks == 0)
                {
                    lmks = lm;
                }
                else if (lmks != lm)
                {
                    QString msg = "[Import Node] Error in TPS file: " + filename;
                    Logger::getInstance()->log(msg, Logger::WARNING);
                    Logger::getInstance()->log("number of landmarks have to be the same for all specimens", Logger::WARNING);
                    return 1;
                }

                for (int i=0;i<lmks;i++)
                {
                    datafile >> tmpstr;
                    QString qstr1(tmpstr.c_str());
                    templist.push_back(qstr1);

                    datafile >> tmpstr;
                    QString qstr2(tmpstr.c_str());
                    templist.push_back(qstr2);
                }
            }
            if (strline.substr(0,eq)=="image")
            {
                imagepaths->push_back(strline.substr(eq+1));
            }
            if (strline.substr(0,eq)=="scale")
            {
                QString scalestr(strline.substr(eq+1).c_str());
                stringscales->push_back(scalestr);
                bool success;
                double scale = scalestr.toDouble(&success);
                if (!success)
                {
                    QString msg = "[Import Node] Error in TPS file: " + filename;
                    Logger::getInstance()->log(msg,Logger::WARNING);
                    msg = "Unrecognized scale number: " + QString(strline.substr(eq+1).c_str());
                    Logger::getInstance()->log(msg,Logger::WARNING);
                }
                scales.push_back(scale);
            }
            if (strline.substr(0,eq)=="id")
            {
                names->push_back(strline.substr(eq+1));
            }
        }
    }

    indiv = names->size();

    if (scales.size() > 0)
    {
        if (scales.size() == 1)
        {
            if (scales[0] > 0.0)
            {
                for (unsigned int i=0;i<templist.size();i++)
                {
                    QString tmp;
                    tmp.setNum(templist[i].toDouble()*scales[0]);
                    templist[i] = tmp;
                }
            }
        }
        else if (templist.size() / lmks <= scales.size())
        {
            for (unsigned int i=0;i<templist.size();i++)
            {
                int j = i / lmks;
                if (scales[j] > 0.0)
                {
                    QString tmp;
                    tmp.setNum(templist[i].toDouble()*scales[j]);
                    templist[i] = tmp;
                }
            }
        }
        else if (templist.size() / lmks > scales.size())
        {
            double factor = 0.0;
            for (unsigned int i=0;i<templist.size();i++)
            {
                unsigned int j = i / lmks;
                if(j<scales.size())
                {
                    if (scales[j] > 0.0)
                    {
                        factor = scales[j];
                        QString tmp;
                        tmp.setNum(templist[i].toDouble()*factor);
                        templist[i] = tmp;
                    }
                }
                else
                {
                    if (factor > 0.0)
                    {
                        QString tmp;
                        tmp.setNum(templist[i].toDouble()*factor);
                        templist[i] = tmp;
                    }
                }
            }
        }
    }

    rawdata->clear();
    stringdata->clear();

    for (unsigned int i=0;i<templist.size();i+=2)
    {
        vector<QString> str_row;
        str_row.push_back(templist[i]);
        str_row.push_back(templist[i+1]);
        stringdata->push_back(str_row);

        vector<double> double_row;
        double_row.push_back(templist[i].toDouble());
        double_row.push_back(templist[i+1].toDouble());
        rawdata->push_back(double_row);
    }

    *landmarks = lmks;
    *individuals = indiv;
    *dimensions = 2;

    if (lmks == 0 && indiv == 0)
    {
        QString msg = "[Import Node] Error in TPS file: " + filename;
        Logger::getInstance()->log(msg,Logger::WARNING);
        Logger::getInstance()->log("File contains no data or not a TPS file",Logger::WARNING);
        *dimensions = 0;
    }

    datafile.close();

    return 0;
}

int ReadFromNTSFile( QString filename,
                     int *individuals,
                     int *landmarks,
                     int *dimensions,
                     vector<vector<double> > *rawdata,
                     vector<vector<QString> > *stringdata,
                     vector<string> *rowLabels,
                     vector<string> *colLabels,
                     QString *missing)
{

    // get the data from file
    ifstream datafile (filename.toStdString().c_str());

    // check if file exists
    if (datafile.fail())
    {
        QString msg = "[Import Node] File not found: " + filename;
        Logger::getInstance()->log(msg, Logger::WARNING);
        return 1;
    }

    string strline;
    QList<QString> templist;

    rawdata->clear();
    stringdata->clear();
    rowLabels->clear();
    colLabels->clear();

    // put data into data matrix
    while (!datafile.eof())
    {
        getline(datafile,strline);
        if (strline.substr(0,1) == "\"" || strline.substr(0,1) == "'" || strline == "")
        {
            // ignore comments and empty linesat the top of the file
        }
        else if (strline.substr(0,1) == "1")
        {
            transform(strline.begin(), strline.end(), strline.begin(), ::tolower);

            // info from first line
            stringstream sstr(strline);
            string tmpstr;
            int numRowLabels = 0;
            int numColLabels = 0;

            // stream line
            sstr >> tmpstr; // 1
            if (tmpstr != "1")
            {
                QString msg = "[Import Node] Error reading NTS file: " + filename;
                Logger::getInstance()->log(msg, Logger::WARNING);
                Logger::getInstance()->log("only rectangular matrices allowed for NTS files", Logger::WARNING);
                Logger::getInstance()->log("first line must be of the form: 1 10L 20 0", Logger::WARNING);

                return 1;
            }

            sstr >> tmpstr; // number of individuals
            if (tmpstr.substr(tmpstr.length()-1) == "l")
            {
                numRowLabels = atoi(tmpstr.substr(0,tmpstr.length()-1).c_str());
                *individuals = numRowLabels;
            }
            else
            {
                *individuals = atoi(tmpstr.c_str());
            }
            sstr >> tmpstr; // number of landmarks times dimensions
            if (tmpstr.substr(tmpstr.length()-1) == "l")
            {
                numColLabels = atoi(tmpstr.substr(0,tmpstr.length()-1).c_str());
                *landmarks = numColLabels;
            }
            else
            {
                *landmarks = atoi(tmpstr.c_str());
            }

            sstr >> tmpstr;
            // this is the value that is used when data is missing
            (*missing) = QString(tmpstr.c_str());

            // remove spaces from line
            string::iterator end_pos = remove(strline.begin(), strline.end(), ' ');
            strline.erase(end_pos, strline.end());

            // find if this is 3 dimensons
            int dim3 = strline.find("dim=3");
            if (dim3>0)
            {
                *dimensions = 3;
            }
            else
            {
                *dimensions = 2;
            }

            *landmarks /= (*dimensions);

            for (int i=0;i<numRowLabels;i++)
            {
                datafile>>tmpstr;
                rowLabels->push_back(tmpstr);
            }

            for (int i=0;i<numColLabels;i++)
            {
                datafile>>tmpstr;
                colLabels->push_back(tmpstr);
            }

            while (datafile>>tmpstr)
            {
                QString qstr(tmpstr.c_str());
                templist.push_back(qstr);
            }
        }
    }

    datafile.close();

    rawdata->resize((*individuals) * (*landmarks));

    int l = 0;
    while (l<templist.count())
    {
        vector<QString> string_row;
        vector<double> double_row;
        for (int k=0;k< (*dimensions);k++)
        {
            double x = templist[l].toDouble();
            double_row.push_back(x);
            string_row.push_back(templist[l]);
            l++;
        }
        rawdata->push_back(double_row);
        stringdata->push_back(string_row);
    }

    return 0;
}


int ReadMatrixFromNTSFile( QString filename,
                           int *rows,
                           int *cols,
                           vector<vector<double> > *rawdata,
                           vector<vector<QString> > *stringdata,
                           vector<string> *rowLabels,
                           vector<string> *colLabels,
                           QString *missing)
{
    // get the data from file
    ifstream datafile (filename.toAscii());

    // check if file exists
    if (datafile.fail())
    {
        QString msg = "[Import Node] File not found: " + filename;
        Logger::getInstance()->log(msg, Logger::RUN_ERROR);
        return 1;
    }

    rawdata->clear();
    stringdata->clear();
    rowLabels->clear();
    colLabels->clear();

    *rows = 0;
    *cols = 0;

    string strline;

    // put data into data matrix
    while (!datafile.eof())
    {
        getline(datafile,strline);
        if (strline.substr(0,1) == "\"" || strline.substr(0,1) == "'" || strline == "")
        {
            // ignore comments and empty linesat the top of the file
        }
        else if (strline.substr(0,1) == "1")
        {
            transform(strline.begin(), strline.end(), strline.begin(), ::tolower);

            // info from first line
            stringstream sstr(strline);
            string tmpstr;
            int numRowLabels = 0;
            int numColLabels = 0;

            // stream line
            sstr >> tmpstr; // 1
            if (tmpstr != "1")
            {
                QString msg = "[Import Node] Error reading NTS file: " + filename;
                Logger::getInstance()->log(msg, Logger::RUN_ERROR);
                Logger::getInstance()->log("only rectangular matrices allowed for NTS files", Logger::RUN_ERROR);
                return 1;
            }

            sstr >> tmpstr; // number of individuals
            if (tmpstr.substr(tmpstr.length()-1) == "l")
            {
                numRowLabels = atoi(tmpstr.substr(0,tmpstr.length()-1).c_str());
                *rows = numRowLabels;
            }
            else
            {
                *rows = atoi(tmpstr.c_str());
            }

            sstr >> tmpstr; // number of landmarks times dimensions
            if (tmpstr.substr(tmpstr.length()-1) == "l")
            {
                numColLabels = atoi(tmpstr.substr(0,tmpstr.length()-1).c_str());
                *cols = numColLabels;
            }
            else
            {
                *cols = atoi(tmpstr.c_str());
            }

            sstr >> tmpstr;
            // this is the value that is used when data is missing
            (*missing) = QString(tmpstr.c_str());

            for (int i=0;i<numRowLabels;i++)
            {
                datafile>>tmpstr;
                rowLabels->push_back(tmpstr);
            }

            for (int i=0;i<numColLabels;i++)
            {
                datafile>>tmpstr;
                colLabels->push_back(tmpstr);
            }

            int j=0;

            vector<QString> tempqstr;
            vector<double> tempdbl;
            while (datafile>>tmpstr)
            {
                bool success;
                QString qstr(tmpstr.c_str());
                double dbl = qstr.toDouble(&success);
                if (!success)
                {
                    dbl = missing->toDouble();
                }
                tempqstr.push_back(qstr);
                tempdbl.push_back(dbl);
                j++;
                if (j == (*cols))
                {
                    stringdata->push_back(tempqstr);
                    rawdata->push_back(tempdbl);
                    tempqstr.clear();
                    tempdbl.clear();
                    j=0;
                }
            }
        }
    }

    datafile.close();
    return 0;
}

int ReadMatrixFromTextFile( QString filename,
                            int *rows,
                            int *cols,
                            vector<vector<double> > *rawdata,
                            vector<vector<QString> > *stringdata,
                            vector<string> *rowLabels,
                            vector<string> *colLabels,
                            bool firstLineHeaders)
{

    QFile file(filename);
    if (!file.exists())
    {
        QString msg = "[Import Node] File not found: " + filename;
        Logger::getInstance()->log(msg, Logger::RUN_ERROR);
        return 1;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString msg = "[Import Node] Error opening file: " + filename;
        Logger::getInstance()->log(msg, Logger::RUN_ERROR);
        return 1;
    }
    QTextStream input(&file);

    if (input.atEnd())
    {
        QString msg = "[Import Node] No data in file: " + filename;
        Logger::getInstance()->log(msg, Logger::RUN_ERROR);
        return 1;
    }

    rawdata->clear();
    stringdata->clear();
    rowLabels->clear();
    colLabels->clear();
    *rows = 0;
    *cols = 0;

    QString seperator = "";
    QString line = input.readLine();
    QStringList tmpLabels;

    while ((line.left(1) == "'" || line == "" ) && !input.atEnd())
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

    int row=0;
    int col=0;

    if (firstLineHeaders)
    {
        if (seperator != "")
        {
            tmpLabels = line.split(seperator);
            for (int i=1;i<(int)tmpLabels.size();i++)
            {
                colLabels->push_back(tmpLabels[i].toStdString());
            }
        }
        else
        {
            colLabels->push_back(line.toStdString());
        }
    }
    else
    {
        QStringList values = line.split(seperator);
        vector<double> tempvec;
        vector<QString> strvec;
        for (int i=0; i<values.size(); i++)
        {
            bool success;

            values[i].toDouble(&success);

            if (values[i] != "")
            {
                tempvec.push_back(values[i].toDouble());
                strvec.push_back(values[i]);
            }
        }
        rawdata->push_back(tempvec);
        stringdata->push_back(strvec);
        row++;
        col = (int) values.size();
    }

    if (seperator == "")
    {
        col = 1;
        while (!input.atEnd())
        {
            line = input.readLine();
            // ignore lines beginning with ' or empty lines
            while ((line.left(1) == "'" || line == "") && !input.atEnd())
            {
                line = input.readLine();
            }

            vector<double> tempvec(1);
            vector<QString> strvec(1);
            strvec[0] = line;
            tempvec[0] = line.toDouble();
            rawdata->push_back(tempvec);
            stringdata->push_back(strvec);
            tempvec.clear();
            strvec.clear();
            row++;
        }
    }
    else
    {
        while (!input.atEnd())
        {
            line = input.readLine();

            // ignore lines beginning with ' or empty lines
            while ((line.left(1) == "'" || line == "") && !input.atEnd())
            {
                line = input.readLine();
            }
            if (seperator == " ")
            {
                // eliminate double spaces
                while (line.contains("  "))
                {
                    line.replace("  "," ");
                }
            }

            QStringList values = line.split(seperator);

            vector<double> tempvec;
            vector<QString> strvec;

            if (col == 0)
            {
                col = (int) values.size();
            }
            else if(values.size() != col)
            {
                QString msg = "[Import Node] Error reading file: " + filename;
                Logger::getInstance()->log(msg, Logger::RUN_ERROR);
                Logger::getInstance()->log("All rows must have the same number of columns", Logger::RUN_ERROR);

                rawdata->clear();
                stringdata->clear();
                rowLabels->clear();
                colLabels->clear();
                return 1;
            }

            if (firstLineHeaders)
            {
                if(values.size() > 0)
                {
                    rowLabels->push_back(values[0].toStdString());
                }
                for (int i=1; i<values.size(); i++)
                {
                    tempvec.push_back(values[i].toDouble());
                    strvec.push_back(values[i]);
                }
            }
            else
            {
                for (int i=0; i<values.size(); i++)
                {
                    tempvec.push_back(values[i].toDouble());
                    strvec.push_back(values[i]);
                }
            }

            rawdata->push_back(tempvec);
            stringdata->push_back(strvec);
            tempvec.clear();
            strvec.clear();
            row++;
        }
    }
    if(firstLineHeaders && col > 1)
    {
        col--;
    }

    (*rows) = row;
    (*cols) = col;

    return 0;
}
