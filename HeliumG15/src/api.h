#define WM_HMM_IPC WM_USER

class SLISTITEM {
	public:
	tstring strArtist;
	tstring strTitle;
	tstring strPath;
	int iLength;
	int iRating;
	int iPos;

	bool operator==(SLISTITEM item) {
		if(strArtist != item.strArtist ||
			strTitle != item.strTitle ||
			strPath != item.strPath ||
			iLength != item.iLength) {
			return false;
		} 
		return true;
	}
};

class SPLAYINGITEM {
public:
	bool bEmpty;

	tstring strArtist;
	tstring strAlbum;
	tstring strTitle;
	tstring strGenre;
	tstring strComment;
	tstring strPath;
	tstring strComposer;
	tstring strLyricist;
	tstring strConductor;
	tstring strProducer;
	tstring strCopyright;

	int iYear;
	int iTrack;
	int iLength;
	int iRating;

	tstring strStreamTitle;
	bool bStream;
	
	bool operator==(SPLAYINGITEM item) {
		if(strArtist != item.strArtist ||
			strAlbum != item.strAlbum ||
			strTitle != item.strTitle ||
			strGenre != item.strGenre ||
			strComment != item.strComment ||
			iYear != item.iYear ||
			iTrack != item.iTrack ||
			strPath != item.strPath ||
			iLength != item.iLength) {
			return false;
		} 
		return true;
	}

	bool operator!=(SPLAYINGITEM item) {
		return !operator==(item);
	}
	
};

/*
This message will start to play the active track in Helium Music Manager’s Active Playlist
Parameters: None
Result: None
*/
#define IPC_PLAY 100

/*
This message will pause the playback (if playing), otherwise it will resume the playback (if paused)
Parameters: None
Result: None
*/
#define IPC_PLAYPAUSE 101

/*
This message will force the playback to pause
Parameters: None
Result: None
*/
#define IPC_FORCEPAUSE 102

/*
This message will stop the playback
Parameters: None
Result: None
*/
#define IPC_STOP 103

/*
This message will skip to the next track in the Active Playlist and start to play this track
Parameters: None
Result: None
*/
#define IPC_NEXT 104

/*
This message will skip to the previous track in the Active Playlist and start to play this track
Parameters: None
Result: None
*/
#define IPC_PREVIOUS 105
 
/*
This message will set the volume of the playback in Helium Music Manager
Parameters: Volume (0-100) in wParam of the message
Result: None
*/
#define IPC_SET_VOLUME 108
 
/*
This message will retrieve the current output volume in Helium Music Manager
Parameters: None
Result: The current volume (0-100)
*/
#define IPC_GET_VOLUME 109
  
/*
This message will retrieve information about the track that is currently being played.
Parameters: None
Result: None
Remarks: See the callback topic above
Details: The information is in the following form:
    * Artist (including secondary artist if available)
    * Album
    * Title (including subtitle and remix if available)
    * Reserved for future use
    * The filename including the full path for this file
    * The length of this file in seconds
    * The rating of this file (0-255)
All fields are separated by a tab character.
*/
#define IPC_GET_CURRENT_TRACK 110

/*
This message will retrieve the duration for the track currently being played
Parameters: None
Result: The duration (in seconds)
*/
#define IPC_GET_DURATION 113
 
/*
This message will set the position for the track currently being played
Parameters: The new position in wParam of the message. (In seconds)
*/
#define IPC_SET_POSITION 114
 
/*
This message will let you know if Helium Music Manager is currently playing a song.
Parameters: None
Result: 1 if playing, 0 otherwise
*/
#define IPC_IS_PLAYING 115
 
/*
This message will let you know if Helium Music Manager currently is paused.
Parameters: None
Result: 1 if paused, 0 otherwise.
*/
#define IPC_IS_PAUSED 116
 
/*
This message will tell you have many entries there are in the Active Playlist in Helium Music Manager.
Parameters: None
Result: The number of entries in the Active Playlist.
*/
#define IPC_GET_LIST_LENGTH 117

/*
This message will select a specific position in the Active Playlist and start to play this track.
Parameters: The desired position in wParam of the message.
Result: None
*/
#define IPC_SET_LIST_POS 118
 
/*
This message will retrieve information about a specific entry in the active playlist.
Parameters: Entry number in wParam of the message.
Result: None
Remarks: See the callback topic above
Details: The information is in the following form:

    * Artist (including secondary artist if available)
    * Title (including subtitle and remix if available)
    * Length in seconds
    * The filename including full path
    * Rating (0-255)
All fields are separated by a tab character.
*/
#define IPC_GET_LIST_ITEM 119 

/*
This message will set the callback window handle used for callback messages.
Parameters: Your window handle in wParam of the message
Result: None
Remarks: See the callback topic above
*/
#define IPC_SET_CALLBACK_HWND 120

/*
This message will return the active position in the Active Playlist
Parameters: None
Result: Active position of the Active Playlist
*/
#define IPC_GET_LIST_POS 121


/*
This message will return the current playback position
Parameters: None
Result: playback position
*/
#define IPC_GET_POSITION 122


/*
If the client has registered itself with a callback to Helium Music Manager,
Helium Music Manager will notify the client using this message each time a new track is played
Parameters: None
Result: None
*/
#define IPC_TRACK_CHANGED_NOTIFICATION 123

/*
This message will show Helium Music Managers main window.
Parameters: None
Result: None
*/
#define IPC_SHOW_HELIUM_WINDOW 124

/*
This message will return the current state of the player.
Parameters: None
Result:
0.     Player is stopped
1.     Player is playing
2.     Player is paused
*/
#define IPC_GET_PLAYER_STATE 125

/*
When the playing state in Helium Music Manager changes, this message will be sent to a client.
Parameters: The new player state (as described for IPC_GET_PLAYER_STATE) in wParam
Result: None
*/
#define IPC_PLAYER_STATE_CHANGED_NOTIFICATION 126

/*
Opens the Auto-enqueue options dialog.
Parameters: None
Result: None
*/
#define IPC_AUTOENQUEUE_OPTIONS 127

/*
Call IPC_SET_REPEAT with 0 in wParam to set Auto-enqueue to stop.
Call IPC_SET_REPEAT with 1 in wParam to set Auto-enqueue to repeat.
Parameters: Mode in wParam:
0.     Set Auto-Enqueue to Stop
1.     Set Auto-Enqueue to Repeat
Result: None
*/
#define IPC_SET_REPEAT 128

/*
This message will be sent to a client when Helium Music Manager shuts down.
Parameters: None
Result: None
*/
#define IPC_SHUTDOWN_NOTIFICATION 129

/*
This message will return the current repeat state
Parameters: None
Result: 1 if Repeat is set, 0 otherwise
*/
#define IPC_GET_REPEAT 130

/*
Send this message to closedown Helium Music Manager.
Please note that a confirmation message may be shown to the user, before Helium Music Manager can close.
Parameters: None
Result: None
*/
#define IPC_CLOSE_HELIUM 131

/*
Send this message to set the rating for the current file
Parameters: Rating in wParam (0-10)
Result: None
*/
#define IPC_SET_RATING 132

/*
When the rating is changed in Helium Music Manager for the file currently being played,
Helium Music Manager will send a notification message (similar to IPC_TRAC_CHANGED_NOTIFICATION) to a registered client, containing the new rating (0-10).
Parameters: The new rating value in wParam
Result: None
*/
#define IPC_RATING_CHANGED_NOTIFICATION 639

/*
When a new album cover is either added/updated or removed from Helium Music Managers Internal Player,
a connected client will be notified with this message.
The full path and filename to the new cover is contained in the <COPYDATA_STRUCT>.
*/
#define IPC_NEW_COVER_NOTIFICATION 800



