#ifdef USE_R_

// Commented to prevent Link Errors
// -*- mode: C++; c-indent-level: 4; c-basic-offset: 4; tab-width: 8 -*-
//
// RInside.cpp: Easier R embedding into C++
//
// Copyright (C) 2009 Dirk Eddelbuettel and GPL'ed

#include "../../DataTypes/MatrixD.h"
#include <string>
#include <vector>
#include <iostream>

#include <R.h>
#include <Rembedded.h>
#include <Rversion.h>
#include <Rdefines.h>
//#define R_INTERFACE_PTRS
//#include <Rinterface.h>
#include <R_ext/Parse.h>

#include "MemBuf.h"

class RInside
{
private:
    MemBuf mb_m;

    bool verbose_m;				// private switch

    //void init_tempdir(void);
    void init_rand(void);
    void autoloads(void);

public:
    int parseEval(const std::string & line, SEXP &ans);
    int parseEvalQ(const std::string & line);

    void assign(const MatrixD& mat, const std::string & nam);
    void assign(const std::string & txt, const std::string & nam);
    void assign(SEXP var, const std::string & nam, bool isProtected=true);

    RInside();
    ~RInside();
};

#endif
