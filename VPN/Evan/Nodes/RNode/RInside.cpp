#ifdef USE_R_

// -*- mode: C++; c-indent-level: 4; c-basic-offset: 4; tab-width: 8 -*-
//
// RInside.cpp: Easier R embedding into C++
//
// Copyright (C) 2009 Dirk Eddelbuettel and GPL'ed


#include "RInside.h"
#include <sys/time.h>		// gettimeofday

#include "../Utilities/Logger.h"

bool verbose = false;
const char *programName = "RInside";

RInside::~RInside()
{
    Rf_endEmbeddedR(0);
}

RInside::RInside()
{

    verbose_m = false; 		// Default is false

    const char *R_argv[] = {"GMMVisR", "--gui=none", "--vanilla", "--no-readline", "--silent", "--slave"};
    int R_argc = (sizeof(R_argv)) / sizeof(R_argv[0]);
    Rf_initEmbeddedR(R_argc, (char**)R_argv);

    R_ReplDLLinit(); 		// this is to populate the repl console buffers
    autoloads();    		// Force all default package to be dynamically required */
    init_rand();    			// for tempfile() to work correctly */
}


void RInside::init_rand(void)
{	/* set seed for tempfile()  */
    unsigned int seed;
    struct timeval tv;
//    gettimeofday (&tv, NULL);
    // changed uint64_t to unsigned int. Need to figure out why BDR used that instead.
    seed = ((unsigned int) tv.tv_usec << 16) ^ tv.tv_sec;
//    srand(seed);

    srand(time(0));
}

void RInside::autoloads()
{

    #include "RInsideAutoloads.h"

    /* Autoload default packages and names from autoloads.h
     *
     * This function behaves in almost every way like
     * R's autoload:
     * function (name, package, reset = FALSE, ...)
     * {
     *     if (!reset && exists(name, envir = .GlobalEnv, inherits = FALSE))
     *        stop("an object with that name already exists")
     *     m <- match.call()
     *     m[[1]] <- as.name("list")
     *     newcall <- eval(m, parent.frame())
     *     newcall <- as.call(c(as.name("autoloader"), newcall))
     *     newcall$reset <- NULL
     *     if (is.na(match(package, .Autoloaded)))
     *        assign(".Autoloaded", c(package, .Autoloaded), env = .AutoloadEnv)
     *     do.call("delayedAssign", list(name, newcall, .GlobalEnv,
     *                                                         .AutoloadEnv))
     *     invisible()
     * }
     *
     * What's missing is the updating of the string vector .Autoloaded with
     * the list of packages, which by my code analysis is useless and only
     * for informational purposes.
     *
     */
    //void autoloads(void){

    SEXP da, dacall, al, alcall, AutoloadEnv, name, package;
    int i,j, idx=0, errorOccurred, ptct;

    /* delayedAssign call*/
    PROTECT(da = Rf_findFun(Rf_install("delayedAssign"), R_GlobalEnv));
    PROTECT(AutoloadEnv = Rf_findVar(Rf_install(".AutoloadEnv"), R_GlobalEnv));
    if (AutoloadEnv == R_NilValue)
    {
        //fprintf(stderr,"%s: Cannot find .AutoloadEnv!\n", programName);
        exit(1);
    }
    PROTECT(dacall = allocVector(LANGSXP,5));
    SETCAR(dacall,da);
    /* SETCAR(CDR(dacall),name); */          /* arg1: assigned in loop */
    /* SETCAR(CDR(CDR(dacall)),alcall); */  /* arg2: assigned in loop */
    SETCAR(CDR(CDR(CDR(dacall))),R_GlobalEnv); /* arg3 */
    SETCAR(CDR(CDR(CDR(CDR(dacall)))),AutoloadEnv); /* arg3 */

    /* autoloader call */
    PROTECT(al = Rf_findFun(Rf_install("autoloader"), R_GlobalEnv));
    PROTECT(alcall = allocVector(LANGSXP,3));
    SET_TAG(alcall, R_NilValue); /* just like do_ascall() does */
    SETCAR(alcall,al);
    /* SETCAR(CDR(alcall),name); */          /* arg1: assigned in loop */
    /* SETCAR(CDR(CDR(alcall)),package); */  /* arg2: assigned in loop */

    ptct = 5;
    for(i = 0; i < packc; i++)
    {
        idx += (i != 0)? packobjc[i-1] : 0;
        for (j = 0; j < packobjc[i]; j++)
        {
            /*printf("autload(%s,%s)\n",packobj[idx+j],pack[i]);*/

            PROTECT(name = NEW_CHARACTER(1));
            PROTECT(package = NEW_CHARACTER(1));
            SET_STRING_ELT(name, 0, COPY_TO_USER_STRING(packobj[idx+j]));
            SET_STRING_ELT(package, 0, COPY_TO_USER_STRING(pack[i]));

            /* Set up autoloader call */
            PROTECT(alcall = allocVector(LANGSXP,3));
            SET_TAG(alcall, R_NilValue); /* just like do_ascall() does */
            SETCAR(alcall,al);
            SETCAR(CDR(alcall),name);
            SETCAR(CDR(CDR(alcall)),package);

            /* Setup delayedAssign call */
            SETCAR(CDR(dacall),name);
            SETCAR(CDR(CDR(dacall)),alcall);

            R_tryEval(dacall,R_GlobalEnv,&errorOccurred);
            if (errorOccurred)
            {
                //fprintf(stderr,"%s: Error calling delayedAssign!\n",programName);
//                exit(1);
            }

            ptct += 3;
        }
    }
    UNPROTECT(ptct);
}

int RInside::parseEval(const std::string & line, SEXP & ans)
{
    //membuf_t mb = pmb_m;
    ParseStatus status;
    SEXP cmdSexp, cmdexpr;// = R_NilValue;
    int i, errorOccurred, result = 0;

    //mb = pmb_m = add_to_membuf(&pmb_m, (char*)line.c_str());
    mb_m.add((char*)line.c_str());

    PROTECT(cmdSexp = allocVector(STRSXP, 1));
    //SET_STRING_ELT(cmdSexp, 0, mkChar((char*)mb->buf));
    SET_STRING_ELT(cmdSexp, 0, mkChar((char*)mb_m.getBufPtr()));

    cmdexpr = PROTECT(R_ParseVector(cmdSexp, -1, &status, R_NilValue));
    switch (status)
    {
    case PARSE_OK:
        /* Loop is needed here as EXPSEXP might be of length > 1 */
        for(i = 0; i < length(cmdexpr); i++)
        {
            ans = R_tryEval(VECTOR_ELT(cmdexpr, i),NULL,&errorOccurred);
            if (errorOccurred)
            {
                result = 2;
                break;
            }
        }
        //mb = pmb_m = rewind_membuf(&pmb_m);
	break;
    case PARSE_INCOMPLETE:
        result = 4;
	break;
    case PARSE_NULL:
        result = 3;
	break;
    case PARSE_ERROR:
        result = 1;
	break;
    case PARSE_EOF:
	break;
    default:
        result = 5;
	break;
    }
    mb_m.rewind();
    UNPROTECT(2);
    return result;
}

int RInside::parseEvalQ(const std::string & line)
{
    SEXP ans;
    int rc = parseEval(line, ans);
    return rc;
}

void RInside::assign(const MatrixD& mat, const std::string & nam)
{
    int nx = mat.GetRows();
    int ny = mat.GetCols();
    SEXP sexpmat = PROTECT(allocMatrix(REALSXP, nx, ny));

    for(int i = 0; i < nx; i++)
        for(int j = 0; j < ny; j++)
            REAL(sexpmat)[i + nx*j] = mat[i][j];
    assign(sexpmat, nam);
}

void RInside::assign(SEXP var, const std::string & nam, bool isProtected)
{
    setVar(install((char*) nam.c_str()), var, R_GlobalEnv);  // now set it
    if(isProtected)
        UNPROTECT(1);
}


void RInside::assign(const std::string & txt, const std::string & nam)
{
    SEXP value = PROTECT(allocVector(STRSXP, 1));
    SET_STRING_ELT(value, 0, mkChar(txt.c_str()));
    assign(value, nam);
}
#endif
