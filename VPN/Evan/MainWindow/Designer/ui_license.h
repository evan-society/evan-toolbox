/********************************************************************************
** Form generated from reading UI file 'license.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LICENSE_H
#define UI_LICENSE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPlainTextEdit>

QT_BEGIN_NAMESPACE

class Ui_LicenseDialog
{
public:
    QGridLayout *gridLayout;
    QPlainTextEdit *plainTextEdit;

    void setupUi(QDialog *LicenseDialog)
    {
        if (LicenseDialog->objectName().isEmpty())
            LicenseDialog->setObjectName(QString::fromUtf8("LicenseDialog"));
        LicenseDialog->resize(518, 329);
        gridLayout = new QGridLayout(LicenseDialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        plainTextEdit = new QPlainTextEdit(LicenseDialog);
        plainTextEdit->setObjectName(QString::fromUtf8("plainTextEdit"));
        plainTextEdit->setReadOnly(true);

        gridLayout->addWidget(plainTextEdit, 0, 0, 1, 1);


        retranslateUi(LicenseDialog);

        QMetaObject::connectSlotsByName(LicenseDialog);
    } // setupUi

    void retranslateUi(QDialog *LicenseDialog)
    {
        LicenseDialog->setWindowTitle(QApplication::translate("LicenseDialog", "EVAN Toolbox License", 0, QApplication::UnicodeUTF8));
        plainTextEdit->setPlainText(QApplication::translate("LicenseDialog", "Copyright \302\251 2014\n"
"\n"
"The copyright of the EVAN Toolbox belongs to the University of Hull, the University of York\n"
"and the University of Vienna.\n"
"The copyright of the GMM Library belongs to the University of Hull, the University of York\n"
"and the University of Vienna.\n"
"The copyright of the Edgewarp Library belongs to William D K Green and the University of Vienna.\n"
"\n"
"All rights reserved.\n"
"\n"
"EVAN Toolbox, TempLand are trademarks of the EVAN Society (http://www.evan-society.org/)\n"
"\n"
"All other trademarks and trade names used here are acknowledged to belong to\n"
"their respective owners.\n"
"\n"
"The EVAN Toolbox is provided 'as-is', without any express or implied warranty.\n"
"In no event will the authors of the software be held liable for any damages \n"
"arising from the use of this software.\n"
"\n"
"The EVAN Toolbox is provided here under the terms of the GNU Public License (version 2).\n"
"\n"
"A copy of the GNU Public License is available from the license page of \n"
""
                        "the www.evan-society.org website.\n"
"\n"
"_________________________________________________________________\n"
"The EVAN Toolbox software uses the OpenGL \302\256 graphics library.\n"
"\n"
"_________________________________________________________________\n"
"OpenSceneGraph Copyright \302\251 2002 Robert Osfield.\n"
"This software uses OpenSceneGraph. OpenScenGraph is provided here under the \n"
"terms of the OpenSceneGraph Public License.\n"
"A copy of this license is available from the license page of www.evan-society.org\n"
"http://www.openscenegraph.org/\n"
"\n"
"_________________________________________________________________\n"
"R Copyright \302\251 \n"
"The EVAN Toolbox uses R. R is provided here under the terms of the GNU Public license.\n"
"A copy of this license is available from the license page of www.evan-society.org\n"
"http://www.r-project.org/\n"
"\n"
"_________________________________________________________________\n"
"Qwt Copyright \302\251 \n"
"The EVAN Toolbox is based in part on the wor"
                        "k of the Qwt project (http://qwt.sf.net).\n"
"Qwt. Qwt is provided under the terms of the Qwt license which is based on the LGPL license.\n"
"\n"
"_________________________________________________________________\n"
"Qwtplot3d   Copyright \302\251  2003-2005 Michael Bieber\n"
"\n"
"The EVAN Toolbox uses Qwtplot3d which is provided under the terms of the zlib license below. \n"
"\n"
" QwtPlot3D license -- January 5th, 2005\n"
"\n"
"  This software is provided 'as-is', without any express or implied\n"
"  warranty.  In no event will the author be held liable for any damages\n"
"  arising from the use of this software.\n"
"\n"
"  Permission is granted to anyone to use this software for any purpose,\n"
"  including commercial applications, and to alter it and redistribute it\n"
"  freely, subject to the following restrictions:\n"
"\n"
"  1. The origin of this software must not be misrepresented; you must not\n"
"     claim that you wrote the original software. If you use this software\n"
"     in a product, an ack"
                        "nowledgment in the product documentation would be\n"
"     appreciated but is not required.\n"
"  2. Altered source versions must be plainly marked as such, and must not be\n"
"     misrepresented as being the original software.\n"
"  3. This notice may not be removed or altered from any source distribution.\n"
"\n"
"_________________________________________________________________\n"
"rply Copyright \302\251 2003-2005 Diego Nehab.\n"
"\n"
"The EVAN Toolbox uses rply which is provided under the terms of the MIT license below. \n"
"\n"
"The MIT License\n"
"\n"
"Permission is hereby granted, free of charge, to any person obtaining a copy\n"
"of this software and associated documentation files (the \"Software\"), to deal\n"
"in the Software without restriction, including without limitation the rights\n"
"to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n"
"copies of the Software, and to permit persons to whom the Software is\n"
"furnished to do so, subject to the following conditions:\n"
""
                        "\n"
"The above copyright notice and this permission notice shall be included in\n"
"all copies or substantial portions of the Software.\n"
"\n"
"THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
"IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
"FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n"
"AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n"
"LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"
"OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN\n"
"THE SOFTWARE.\n"
"\n"
"_________________________________________________________________\n"
"\n"
"libxml2 Copyright \302\251 Daniel Veillard\n"
"\n"
"The EVAN Toolbox uses libxml2 which is provided under the terms of the MIT license below. \n"
"\n"
"The MIT License\n"
"\n"
"Permission is hereby granted, free of charge, to any person obtaining a copy\n"
"of this software and associated documentation files"
                        " (the \"Software\"), to deal\n"
"in the Software without restriction, including without limitation the rights\n"
"to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n"
"copies of the Software, and to permit persons to whom the Software is\n"
"furnished to do so, subject to the following conditions:\n"
"\n"
"The above copyright notice and this permission notice shall be included in\n"
"all copies or substantial portions of the Software.\n"
"\n"
"THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
"IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
"FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n"
"AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n"
"LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"
"OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN\n"
"THE SOFTWARE.\n"
"\n"
"_________________________________________________________"
                        "________\n"
"Lapack  Copyright \302\251 1992-2008 The University of Tennessee.  All rights reserved.\n"
"\n"
"The EVAN Toolbox uses Lapack which is provided under the terms of the license below. \n"
"\n"
"$COPYRIGHT$\n"
"\n"
"Additional copyrights may follow\n"
"\n"
"$HEADER$\n"
"\n"
"Redistribution and use in source and binary forms, with or without\n"
"modification, are permitted provided that the following conditions are\n"
"met:\n"
"\n"
"- Redistributions of source code must retain the above copyright\n"
"  notice, this list of conditions and the following disclaimer. \n"
"  \n"
"- Redistributions in binary form must reproduce the above copyright\n"
"  notice, this list of conditions and the following disclaimer listed\n"
"  in this license in the documentation and/or other materials\n"
"  provided with the distribution.\n"
"  \n"
"- Neither the name of the copyright holders nor the names of its\n"
"  contributors may be used to endorse or promote products derived from\n"
"  this software without specific "
                        "prior written permission.\n"
"  \n"
"THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS\n"
"\"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT  \n"
"LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR\n"
"A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT \n"
"OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,\n"
"SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT\n"
"LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,\n"
"DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY\n"
"THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT  \n"
"(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE\n"
"OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. \n"
"\n"
"_________________________________________________________________\n"
"gdtoa\n"
"\n"
"The EVAN Toolbox uses a modified version of the gdtoa libary. \n"
"\n"
"Below is the copyri"
                        "ght notice from gdtoa.tgz:\n"
"\n"
"/****************************************************************\n"
"Copyright \302\251 1997-2001 Lucent Technologies\n"
"All Rights Reserved\n"
"\n"
"Permission to use, copy, modify, and distribute this software and\n"
"its documentation for any purpose and without fee is hereby\n"
"granted, provided that the above copyright notice appear in all\n"
"copies and that both that the copyright notice and this\n"
"permission notice and warranty disclaimer appear in supporting\n"
"documentation, and that the name of Lucent or any of its entities\n"
"not be used in advertising or publicity pertaining to\n"
"distribution of the software without specific, written prior\n"
"permission.\n"
"\n"
"LUCENT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,\n"
"INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.\n"
"IN NO EVENT SHALL LUCENT OR ANY OF ITS ENTITIES BE LIABLE FOR ANY\n"
"SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES\n"
"WHATSOEVER RESULTING FROM L"
                        "OSS OF USE, DATA OR PROFITS, WHETHER\n"
"IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,\n"
"ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF\n"
"THIS SOFTWARE.\n"
"****************************************************************/\n"
"\n"
"http://cvsweb.netbsd.org/bsdweb.cgi/src/lib/libc/gdtoa/\n"
"\n"
"_________________________________________________________________\n"
"DCMTK\n"
"\n"
"The EVAN Toolbox uses the DCMTK software package. \n"
"\n"
"COPYRIGHT\n"
"\n"
"Unless otherwise specified, the DCMTK software package has the\n"
"following copyright:\n"
"\n"
"/*\n"
" *  Copyright \302\251 1994-2005, OFFIS\n"
" *\n"
" *  This software and supporting documentation were developed by\n"
" *\n"
" *    Kuratorium OFFIS e.V.\n"
" *    Healthcare Information and Communication Systems\n"
" *    Escherweg 2\n"
" *    D-26121 Oldenburg, Germany\n"
" *\n"
" *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY\n"
" *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCH"
                        "ANTABILITY  OR\n"
" *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR\n"
" *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND\n"
" *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.\n"
" *\n"
" *  Copyright of the software  and  supporting  documentation  is,  unless\n"
" *  otherwise stated, owned by OFFIS, and free access is hereby granted as\n"
" *  a license to  use  this  software,  copy  this  software  and  prepare\n"
" *  derivative works based upon this software.  However, any  distribution\n"
" *  of this software source code or supporting documentation or derivative\n"
" *  works  (source code and  supporting documentation)  must  include  the\n"
" *  three paragraphs of this copyright notice.\n"
" *\n"
" */\n"
"\n"
"http://dicom.offis.de/dcmtk.php.en\n"
"\n"
"_________________________________________________________________\n"
"mexcel Copyright \302\251 Andrea Litvin\n"
"\n"
"The EVAN Toolbox uses code provided for saving Excel files. The web article\n"
""
                        "has no explicit license attached to it but may contain usage terms in the\n"
"article text or the download files themselves. If in doubt please contact \n"
"the author via the discussion board available via the web article.\n"
"\n"
"http://www.codeproject.com/KB/cpp/miniexcel.aspx\n"
"\n"
"_________________________________________________________________\n"
"", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class LicenseDialog: public Ui_LicenseDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LICENSE_H
