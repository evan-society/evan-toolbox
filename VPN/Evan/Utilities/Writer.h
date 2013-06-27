#ifndef WRITER_H
#define WRITER_H

#include <fstream>

class Writer
{
    public:
        static Writer& Instance()
        {
            static Writer _write;
            return _write;
        }

        bool writeTofile() { return _writeTofile; }
        void writeTofile( bool write ) { _writeTofile = write; }

        void writeline( const char* str );
        void write( char c );
        void write( int i );
        void write( float f );
        void write( double d );

    private:
        Writer();
        Writer(Writer&) {}
        Writer& operator=(const Writer&);

        void open();
        void close();

        bool _writeTofile;
        std::ofstream _file;
};

/*Writer& operator<<( Writer& write, const char* t )
{
    write.writeline( t );
    return write;
}

Writer& operator<<( Writer& write, int i )
{
    write.write( i );
    return write;
}

Writer& operator<<( Writer& write, float f )
{
    write.write( f );
    return write;
}

Writer& operator<<( Writer& write, double d )
{
    write.write( d );
    return write;
}*/

#endif // WRITER_H
