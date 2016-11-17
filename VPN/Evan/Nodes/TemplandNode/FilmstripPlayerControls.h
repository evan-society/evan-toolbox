#ifndef FILMSTRIPPLAYERCONTROLS_H
#define FILMSTRIPPLAYERCONTROLS_H

#include <sstream>
#include <QWidget>
#include <QTime>
#include <QTimerEvent>
#include "ew/Dig3.h"
#include "ew/Form3.h"
#include "ew/Dig3Space.h"
#include "ew/Dig3View.h"
#include "ew/View3Widget.h"
#include "ew/Dig3Tableau.h"


//! Templand FilmstripPlayerControls class
/*!
	This class handles the animation of tableau in a filmstrip sort of manner.
*/
class FilmstripPlayerControls : public QObject
{
Q_OBJECT
public:

    enum Status { PLAYING, STOPPED, PAUSED };

    FilmstripPlayerControls();
    virtual ~FilmstripPlayerControls();

    //! Assigns the new tableau to the filmstrip controller.
	/*!
		\param dig A pointer to the 3D EW digitizer.
		\param tableaus A list of Tableaus to be played.
	*/
    void AssignContent( ew::Dig3* dig, std::vector<ew::Dig3Tableau>& tableaus );

    //! Plays the filmstrip.
    void play();

    //! Pauses the filmstrip.
    void pause();

    //! Stops the filmstrip.
    void stop( bool notifyAndClear = true);

    //! Plays the filmstrip forward.
    void playForward();

    //! Plays the filmstrip backward.
    void playBackward();

    //! Displays the previous Tableau in the sequence.
    void prevFrame();

    //! Displays the next Tableau in the sequence.
    void nextFrame();

    //! Displays the previous interpolated tableau between current and previous tableau.
    void prevFrameStep();

    //! Displays the next interpolated tableau between current and next tableau.
    void nextFrameStep();

    //! Displays the first Tableau in the sequence.
    void startFrame();

    //! Displays the last Tableau in the sequence.
    void endFrame();

    //! Returns the state of the player whether its in play state or not.
	/*!
		\return Returns the state of the filmstrip.
	*/
    bool IsPlaying() { return m_playing; }

    //! Defines how many seconds should the player process each Tableau frame.
	/*!
		\param secFrame Number of seconds to interpolate a Tableau frame.
	*/
    void secPerFrame( float secFrame )
    {
        if( secFrame == 0.0f )
            return;

//        if( m_playlastframems > 0 )
//            m_playlastframems /= (int)m_secPerFrame;
//
//        if( m_startoffset > 0 )
//            m_startoffset /= (int)m_secPerFrame;

        //m_secPerFrame = secFrame > 0 ? secFrame : 1.0f;
        m_speed = secFrame > 0 ? secFrame : 1.0f;
    }

    //! Defines how many steps should the player interpolate between two Tableau frames.
	/*!
		\param stepFrame Number of steps to interpolate a Tableau frame.
	*/
    void stepPerFrame( float stepFrame ) { m_stepPerFrame = stepFrame > 0 ? stepFrame : 1.0f; }

    //! Returns how many seconds the player processes each Tableau frame.
	/*!
		\return Number of seconds to interpolate a Tableau frame.
	*/
    float secPerFrame() { return m_speed; }

    //! Returns how many steps the player interpolate between two Tableau frames.
	/*!
		\return Number of steps to interpolate a Tableau frame.
	*/
    float stepPerFrame() { return m_stepPerFrame; }

public slots:
    //! Defines which Tableau frames to display.
	/*!
		\param Tableau frame index to display.
	*/
    void moveToFrame(int);

signals:

    //! Signals when a Tableau Frame has been changed.
	/*!
		\param frameIndex New current Tableau frame on display.
	*/
    void frameIndexChange( int frameIndex );

    //! Signals when a Tableau Frame has been changed. Useful to get the specific step the filmstrip is in.
	/*!
		\param frameIndex New current Tableau frame interpolated step on display.
	*/
    void frameNumberChange( float frame );

    //! Signals when filmstrip player state has been changed.
	/*!
		\param status New current state of the player.
	*/
    void statusChanged( int status );

private:
    std::vector<ew::Dig3Tableau> m_playtableaus;
    std::vector<ew::Form3> m_playforms1;
    std::vector<ew::Form3> m_playforms2;
    ew::Dig3* m_dig3;
    QTime m_timer;
    bool m_playing;
    bool m_assigned;
    bool m_paused;
    bool m_stopped;
    int m_direction;
    int m_pausedTime;
    int m_timerID;
    int m_playlastms;
    int m_playlastframe;
    int m_startoffset;
    int m_stepIndex;
    int m_playlastframems;
    float m_secPerFrame;
    float m_stepPerFrame;
    float m_speed;

    virtual void timerEvent(QTimerEvent *);
    void Interpolate( int framenear, int frame0, int frame1, double msf );

    void do_prevNextFrame( bool prev );

    int get_secPerFrameInMS() { return (int)( m_secPerFrame * 1000 * m_speed ); }
};

#endif // FILMSTRIPPLAYERCONTROLS_H
