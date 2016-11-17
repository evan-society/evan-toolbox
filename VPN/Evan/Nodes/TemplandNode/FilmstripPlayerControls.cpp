#include "FilmstripPlayerControls.h"

FilmstripPlayerControls::FilmstripPlayerControls() : m_dig3(0)
{
    m_assigned = false;
    m_playing = false;
    m_paused = false;
    m_stopped = true;
    m_direction = 0;
    m_pausedTime = 0;
    m_timerID = 0;
    m_playlastms = 0;
    m_playlastframe = -1;
    m_startoffset = 0;
    m_secPerFrame = 1.0f;
    m_stepPerFrame = 10.0f;
    m_stepIndex = 0;
    m_playlastframems = 0;
    m_speed = 1.0f;
}

FilmstripPlayerControls::~FilmstripPlayerControls()
{
    m_assigned = false;
    m_playing = false;

    if( m_timerID )
        killTimer(m_timerID);
}

void FilmstripPlayerControls::AssignContent( ew::Dig3* dig, std::vector<ew::Dig3Tableau>& tableaus )
{
    m_dig3 = dig;
    m_playtableaus = tableaus;

    if( !m_stopped )
        stop(false);

    m_playing = false;
    m_paused = false;
    m_stopped = false;

    m_playlastframe = 0;

    m_assigned = true;

    int n = m_playtableaus.size();
    m_playforms1.resize(n);
    m_playforms2.resize(n);

    for (int i = 0; i < n; i++)
    {
        m_playforms1[i].read_file( m_playtableaus[i].space[0].form_filename.c_str() );

        m_playforms2[i].read_file( m_playtableaus[i].space[1].form_filename.c_str() );
    }
}

void FilmstripPlayerControls::play()
{
    if( !m_assigned || m_playtableaus.size() < 2 || m_playing )
        return;

    if( m_paused )
    {
        m_paused = !m_paused;

        m_playing = true;

        emit statusChanged( PLAYING );

        return;
    }

    m_playing = true;
    m_paused = false;
    m_stopped = false;
    m_timer.start();
    m_playlastms = 0;

    m_timerID = startTimer(0);
    if (m_timerID == 0)
    {
        //throw std::runtime_error("could not start qt play timer");
    }

    emit statusChanged( PLAYING );
}

void FilmstripPlayerControls::pause()
{
    if( m_stopped || m_paused)
        return;

    m_paused = true;
    m_stopped = false;
    m_playing = false;
    m_pausedTime = 0;

    emit statusChanged( PAUSED );
}

void FilmstripPlayerControls::stop( bool notifyAndClear )
{
    m_stopped = true;
    m_paused = false;
    m_playing = false;

    if( m_timerID )
    {
        killTimer(m_timerID);
        m_timerID = 0;
    }

    m_pausedTime = 0;
    m_startoffset = 0;
    m_stepIndex = 0;

    if( notifyAndClear )
    {
        m_dig3->load_tableau(&m_playtableaus[0], 0, ew::Dig3::TABLEAU_SETTINGS);
        m_dig3->load_tableau(&m_playtableaus[0], 1, ew::Dig3::TABLEAU_SETTINGS);
        m_dig3->load_tableau(&m_playtableaus[0], 2, ew::Dig3::TABLEAU_SETTINGS);
        m_dig3->load_tableau(&m_playtableaus[0], 3, ew::Dig3::TABLEAU_SETTINGS);
    }

    m_playlastframems = 0;
    m_playlastframe = 0;

    if( notifyAndClear )
    {
        emit frameIndexChange( 1 );
        emit frameNumberChange( 1.0f );
        emit statusChanged( STOPPED );
    }
}

void FilmstripPlayerControls::do_prevNextFrame( bool prev )
{
    if( m_playlastframe == -1 )
        m_playlastframe = 0;
    int temp = m_playlastframe;
    if( !m_stopped )
        stop();
    if( prev )
    {
        if( temp - 1 < 0 )
            temp = m_playtableaus.size() - 1;
        else
            --temp;
    }
    else
    {
        if( temp + 1 >= (int)m_playtableaus.size() )
            temp = 0;
        else
            ++temp;
    }

    m_playlastframe = temp;

    m_dig3->load_tableau(&m_playtableaus[m_playlastframe], 0, ew::Dig3::TABLEAU_SETTINGS);
    m_dig3->load_tableau(&m_playtableaus[m_playlastframe], 1, ew::Dig3::TABLEAU_SETTINGS);
    m_dig3->load_tableau(&m_playtableaus[m_playlastframe], 2, ew::Dig3::TABLEAU_SETTINGS);
    m_dig3->load_tableau(&m_playtableaus[m_playlastframe], 3, ew::Dig3::TABLEAU_SETTINGS);

    m_playlastframems = m_startoffset = m_playlastframe * get_secPerFrameInMS();

    m_pausedTime = 0;

    emit frameIndexChange( m_playlastframe + 1 );

    emit frameNumberChange( m_playlastframe + 1.0f );
}

void FilmstripPlayerControls::prevFrame()
{
    do_prevNextFrame( true );
}

void FilmstripPlayerControls::nextFrame()
{
    do_prevNextFrame( false );
}

void FilmstripPlayerControls::prevFrameStep()
{
    int msperframe = get_secPerFrameInMS();
    int value = (int)( msperframe / m_stepPerFrame );

    if( !m_playing )
    {
        int n = m_playtableaus.size();
        int ms = 0;
        if( !m_paused ) // if it was stopped
        {
            m_playlastframems -= value;
            if( m_playlastframems < 0 )
                m_playlastframems = ( n * msperframe ) - value;
            ms = m_playlastframems;
            m_startoffset = m_playlastframems;
        }
        else // if it was paused
        {
            if( m_direction == 1 )
            {
                m_stepIndex -= value;
                if( m_playlastframems + m_stepIndex < 0 )
                    ms = n * msperframe + m_stepIndex;
                else
                    ms = m_playlastframems + m_stepIndex;
            }
            else
            {
                int t = m_playlastframems;
                if( t > n * msperframe )
                    t %= n * msperframe;

                m_stepIndex -= value;

                ms = ( n * msperframe - t ) + m_stepIndex;// + rem;
            }
        }

        int frame0 = (ms / msperframe) % n;
        int frame1 = (frame0 + 1) % n;

        double msf = (ms % msperframe)  / (double)(msperframe);
        int framenear;
        if (msf < 0.5)
            framenear = frame0;
        else
            framenear = frame1;

        emit frameNumberChange( (float)(frame0) + msf + 1.0f );

        framenear = frame0;

        Interpolate( framenear, frame0, frame1, msf );
    }
}

void FilmstripPlayerControls::startFrame()
{
    stop();
}

void FilmstripPlayerControls::endFrame()
{
    stop();
    prevFrame();
}

void FilmstripPlayerControls::nextFrameStep()
{
    int msperframe = get_secPerFrameInMS();
    int value = (int)( msperframe / m_stepPerFrame );

    if( !m_playing )
    {
        int n = m_playtableaus.size();
        int ms = 0;
        if( !m_paused ) // if it was stopped
        {
            m_playlastframems += value;
            if( m_playlastframems > n * msperframe )
                m_playlastframems %= (n * msperframe );
            ms = m_playlastframems;
            m_startoffset = ms;
        }
        else // if it was paused
        {
            if( m_direction == 1 )
            {
                m_stepIndex += value;
                ms = m_playlastframems + m_stepIndex;
            }
            else
            {
                int t = m_playlastframems;
                if( t > n * msperframe )
                    t %= n * msperframe;

                m_stepIndex += value;
                ms = ( n * msperframe - t ) + m_stepIndex;
            }
        }

        int frame0 = (ms / msperframe) % n;
        int frame1 = (frame0 + 1) % n;

        double msf = (ms % msperframe)  / (double)(msperframe);
        int framenear;
        if (msf < 0.5)
            framenear = frame0;
        else
            framenear = frame1;

        emit frameNumberChange( (float)(frame0 + 1) + msf );

        framenear = frame0;

        Interpolate( framenear, frame0, frame1, msf );
    }
    else
        m_stepIndex = value;
}

void FilmstripPlayerControls::moveToFrame( int index )
{
    if( m_playlastframe != index - 1 )
    {
        m_playlastframe = index - 1;
        m_startoffset = m_playlastframe * get_secPerFrameInMS();

        emit frameNumberChange( (float)(index) );
    }
}

void FilmstripPlayerControls::playForward()
{
    if( m_direction == 2 )
    {
        if( m_stepIndex )
        {
            int temp = (int)(m_playtableaus.size() * get_secPerFrameInMS()) - m_playlastframems;
            temp += m_stepIndex;
            stop( false );
            if( temp > (int)(m_playtableaus.size() * get_secPerFrameInMS()) ) // make sure it doesnt go past last frame
                temp %= m_playtableaus.size() * get_secPerFrameInMS();
            if( temp < 0 ) // make sure it doesnt go below 1 frame
                temp += m_playtableaus.size() * get_secPerFrameInMS();
            m_startoffset = temp;
        }
        else
        {
            int temp = m_playlastframems;
            if( temp > (int)(m_playtableaus.size() * get_secPerFrameInMS()) )
                temp %= m_playtableaus.size() * get_secPerFrameInMS();
            temp = ( m_playtableaus.size() * get_secPerFrameInMS() ) - temp;// + ( get_secPerFrameInMS() - temp );
            stop( false );
            m_startoffset = temp % ( m_playtableaus.size() * get_secPerFrameInMS() );
        }
    }
    else
    {
        if( m_stepIndex )
        {
            int temp = m_playlastframems;
            temp += m_stepIndex;
            stop( false );
            if( temp > (int)(m_playtableaus.size() * get_secPerFrameInMS()) ) // make sure it doesnt go past last frame
                temp %= m_playtableaus.size() * get_secPerFrameInMS();
            if( temp < 0 ) // make sure it doesnt go below 1 frame
                temp += m_playtableaus.size() * get_secPerFrameInMS();
            m_startoffset = temp;
        }
    }
    m_direction = 1;
}

void FilmstripPlayerControls::playBackward()
{
    if( m_direction == 1 )
    {
        if( m_stepIndex )
        {
            int temp = m_playlastframems;
            temp += m_stepIndex;
            stop( false );
            if( temp > (int)(m_playtableaus.size() * get_secPerFrameInMS()) )
                temp %= m_playtableaus.size() * get_secPerFrameInMS();
            if( temp < 0 ) // make sure it doesnt go below 1 frame
                temp += m_playtableaus.size() * get_secPerFrameInMS();
            m_startoffset = temp;
        }
        else
        {
            int temp = m_playlastframems;
            if( temp > (int)(m_playtableaus.size() * get_secPerFrameInMS()) )
                temp %= m_playtableaus.size() * get_secPerFrameInMS();
            stop( false );
            m_startoffset = temp % ( m_playtableaus.size() * get_secPerFrameInMS() );
        }
    }
    else
    {
        if( m_stepIndex )
        {
            int temp = m_playlastframems;
            temp -= m_stepIndex;
            stop( false );
            if( temp > (int)(m_playtableaus.size() * get_secPerFrameInMS()) )
            {
                temp %= m_playtableaus.size() * get_secPerFrameInMS();
                temp = m_playtableaus.size() * get_secPerFrameInMS() - temp;
            }
            else if( temp < 0 ) // make sure it doesnt go below 1 frame
                temp += m_playtableaus.size() * get_secPerFrameInMS();
            else
                temp = m_playtableaus.size() * get_secPerFrameInMS() - temp;
            m_startoffset = temp;
        }
    }
    m_direction = 2;
}

void FilmstripPlayerControls::timerEvent(QTimerEvent *ev )
{
    if( ev->timerId() != m_timerID || !m_assigned )
        return;


    int ms = m_timer.elapsed();
    int ms_copy = ms;

    if( m_paused )
    {
        m_pausedTime = ms - m_playlastms;
        return;
    }
    if( m_stopped )
        return;

    ms -= m_pausedTime;

    if( m_stepIndex != 0 )
    {
        if( ms + m_stepIndex < 0 )
        {
            ms += m_stepIndex;
            ms = m_playtableaus.size() * get_secPerFrameInMS() + ms;
        }
        else
        {
            ms += m_stepIndex;
        }
        m_stepIndex = 0;
    }

    if( m_direction == 1 ) // forward
        ms += m_startoffset;
    else if( m_direction == 2 ) // backward
    {
        if( m_startoffset > 0 )
            ms += ( m_playtableaus.size() * get_secPerFrameInMS() - m_startoffset );
    }

    m_playlastframems = ms;
    m_playlastms = ms_copy;
    int n = m_playtableaus.size();
    int msperframe = (int)( m_secPerFrame * 1000 );
    int frame0 = (ms / msperframe) % n;
    int frame1 = (frame0 + 1) % n;

    double msf = (ms % msperframe)  / (double)(msperframe);
    int framenear;
    float fnc = 0.0f;

    if( m_direction  == 1) // forward
    {
        fnc = (float)(frame0) + (float)msf + 1.0f;
    }
    else if( m_direction  == 2) // backward
    {
        frame0 = n - frame0 - 1;
        frame1 = n - frame1 - 1;

        fnc = (float)(frame0) + ( 1.0 - msf ) + 1.0f;
    }

    framenear = frame0;

    emit frameNumberChange( fnc );

    Interpolate( framenear, frame0, frame1, msf );
}

void FilmstripPlayerControls::Interpolate( int framenear, int frame0, int frame1, double msf )
{
    int n = (int)m_playtableaus.size();

    // check if we have valid values
    if( framenear < 0 || frame0 < 0 || frame1 < 0 || framenear > n || frame0 > n || frame1 > n )
        return;

    if (framenear != m_playlastframe)
    {
        for (int i = 0; i < m_dig3->get_n_views(); i++)
        {
            m_dig3->get_views()[i]->clear_highlight();
        }
        if (m_playlastframe == -1 || m_playforms1[framenear] != m_playforms1[m_playlastframe])
        {
            m_dig3->get_spaces()[0]->set_form_data(&m_playforms1[framenear]);
        }
        if (m_playlastframe == -1 || m_playforms2[framenear] != m_playforms2[m_playlastframe])
        {
            m_dig3->get_spaces()[1]->set_form_data(&m_playforms2[framenear]);
        }
        m_dig3->load_tableau(&m_playtableaus[framenear], 0, ew::Dig3::TABLEAU_SETTINGS);
        m_dig3->load_tableau(&m_playtableaus[framenear], 1, ew::Dig3::TABLEAU_SETTINGS);
        m_dig3->load_tableau(&m_playtableaus[framenear], 2, ew::Dig3::TABLEAU_SETTINGS);
        m_dig3->load_tableau(&m_playtableaus[framenear], 3, ew::Dig3::TABLEAU_SETTINGS);
        m_playlastframe = framenear;
        emit frameIndexChange( m_playlastframe + 1 );
    }
    m_dig3->interpolate_tableau(&m_playtableaus[frame0], &m_playtableaus[frame1], msf, 0);
    m_dig3->interpolate_tableau(&m_playtableaus[frame0], &m_playtableaus[frame1], msf, 1);
    m_dig3->interpolate_tableau(&m_playtableaus[frame0], &m_playtableaus[frame1], msf, 2);
    m_dig3->interpolate_tableau(&m_playtableaus[frame0], &m_playtableaus[frame1], msf, 3);
}
