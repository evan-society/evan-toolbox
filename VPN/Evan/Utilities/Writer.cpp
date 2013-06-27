#include "Writer.h"


Writer::Writer()
{
    _writeTofile = true;
}

void Writer::open()
{
    if( _writeTofile )
    {
        if( !_file.is_open() )
            _file.open( "writer.log", std::ios_base::app );
    }
}

void Writer::close()
{
    if( _file.is_open() )
    {
        _file.flush();
        _file.close();
    }
}

void Writer::writeline( const char* str )
{
    if( _writeTofile )
    {
        open();
        _file << str << '\n';
        close();
    }
}

void Writer::write( char c )
{
    if( _writeTofile )
    {
        open();
        _file << c;
        close();
    }
}

void Writer::write( int i )
{
    if( _writeTofile )
    {
        open();
        _file << i;
        close();
    }
}

void Writer::write( float f )
{
    if( _writeTofile )
    {
        open();
        _file << f;
        close();
    }
}

void Writer::write( double d )
{
    if( _writeTofile )
    {
        open();
        _file << d;
        close();
    }
}
