// Module:  Log4CPLUS
// File:    appender.h
// Created: 6/2001
// Author:  Tad E. Smith
//
//
// Copyright 2001-2009 Tad E. Smith
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/** @file */

#ifndef DCMTK_LOG4CPLUS_APPENDER_HEADER_
#define DCMTK_LOG4CPLUS_APPENDER_HEADER_

#include "dcmtk/oflog/config.h"
#include "dcmtk/oflog/layout.h"
#include "dcmtk/oflog/loglevel.h"
#include "dcmtk/oflog/tstring.h"
#include "dcmtk/oflog/helpers/lloguser.h"
#include "dcmtk/oflog/helpers/pointer.h"
#include "dcmtk/oflog/helpers/property.h"
#include "dcmtk/oflog/spi/filter.h"

//#include <memory>
#include "dcmtk/ofstd/ofaptr.h"


namespace dcmtk {
namespace log4cplus {

    /**
     * This class is used to "handle" errors encountered in an {@link
     * dcmtk::log4cplus::Appender}.
     */
    class DCMTK_LOG4CPLUS_EXPORT ErrorHandler {
    public:
        virtual ~ErrorHandler();
        virtual void error(const tstring& err) = 0;
        virtual void reset() = 0;
    };



    class DCMTK_LOG4CPLUS_EXPORT OnlyOnceErrorHandler : public ErrorHandler,
                                                  protected helpers::LogLogUser
    {
    public:
      // Ctor
        OnlyOnceErrorHandler() : firstTime(true){}

        virtual void error(const tstring& err);
        virtual void reset();

    private:
        bool firstTime;
    };


    /**
     * Extend this class for implementing your own strategies for printing log
     * statements.
     */
    class DCMTK_LOG4CPLUS_EXPORT Appender
        : public virtual helpers::SharedObject
        , protected helpers::LogLogUser

    {
    public:
      // Ctor
        Appender();
        Appender(const helpers::Properties properties);

      // Dtor
        virtual ~Appender();

        void destructorImpl();

      // Methods
        /**
         * Release any resources allocated within the appender such as file
         * handles, network connections, etc.
         *
         * It is a programming error to append to a closed appender.
         */
        virtual void close() = 0;

        /**
         * This method performs threshold checks and invokes filters before
         * delegating actual logging to the subclasses specific {@link
         * #append} method.
         */
        void doAppend(const spi::InternalLoggingEvent& event);

        /**
         * Get the name of this appender. The name uniquely identifies the
         * appender.
         */
        virtual tstring getName();

        /**
         * Set the name of this appender. The name is used by other
         * components to identify this appender.
         */
        virtual void setName(const tstring& name);

        /**
         * Set the {@link ErrorHandler} for this Appender.
         */
        virtual void setErrorHandler(OFauto_ptr<ErrorHandler> eh);

        /**
         * Return the currently set {@link ErrorHandler} for this
         * Appender.
         */
        virtual ErrorHandler* getErrorHandler();

        /**
         * Set the layout for this appender. Note that some appenders have
         * their own (fixed) layouts or do not use one. For example, the
         * SocketAppender ignores the layout set here.
         */
        virtual void setLayout(OFauto_ptr<Layout> layout);

        /**
         * Returns the layout of this appender. The value may be NULL.
         *
         * This class owns the returned pointer.
         */
        virtual Layout* getLayout();

        /**
         * Set the filter chain on this Appender.
         */
        void setFilter(spi::FilterPtr f) { filter = f; }

        /**
         * Get the filter chain on this Appender.
         */
        spi::FilterPtr getFilter() const { return filter; }

        /**
         * Returns this appenders threshold LogLevel. See the {@link
         * #setThreshold} method for the meaning of this option.
         */
        LogLevel getThreshold() const { return threshold; }

        /**
         * Set the threshold LogLevel. All log events with lower LogLevel
         * than the threshold LogLevel are ignored by the appender.
         *
         * In configuration files this option is specified by setting the
         * value of the <b>Threshold</b> option to a LogLevel
         * string, such as "DEBUG", "INFO" and so on.
         */
        void setThreshold(LogLevel th) { threshold = th; }

        /**
         * Check whether the message LogLevel is below the appender's
         * threshold. If there is no threshold set, then the return value is
         * always <code>true</code>.
         */
        bool isAsSevereAsThreshold(LogLevel ll) const {
            return ((ll != NOT_SET_LOG_LEVEL) && (ll >= threshold));
        }

    protected:
      // Methods
        /**
         * Subclasses of <code>Appender</code> should implement this
         * method to perform actual logging.
         * @see doAppend method.
         */
        virtual void append(const spi::InternalLoggingEvent& event) = 0;

      // Data
        /** The layout variable does not need to be set if the appender
         *  implementation has its own layout. */
        OFauto_ptr<Layout> layout;

        /** Appenders are named. */
        tstring name;

        /** There is no LogLevel threshold filtering by default.  */
        LogLevel threshold;

        /** The first filter in the filter chain. Set to <code>null</code>
         *  initially. */
        spi::FilterPtr filter;

        /** It is assumed and enforced that errorHandler is never null. */
        OFauto_ptr<ErrorHandler> errorHandler;

        /** Is this appender closed? */
        bool closed;
    };

    /** This is a pointer to an Appender. */
    typedef helpers::SharedObjectPtr<Appender> SharedAppenderPtr;

} // end namespace log4cplus
} // end namespace dcmtk

#endif // DCMTK_LOG4CPLUS_APPENDER_HEADER_

