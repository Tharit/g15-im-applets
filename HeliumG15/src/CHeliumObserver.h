#ifndef _CHELIUMOBSERVER_H_
#define _CHELIUMOBSERVER_H_

class CHeliumObserver {
public:
	virtual void OnTrackChanged(SPLAYINGITEM currentTrack)=0;
	virtual void OnConnectionChanged(bool bConnected)=0;
	virtual void OnPlaybackStateChanged(EPlaybackState eStatus)=0;
	virtual void OnListItemReceived(SLISTITEM item) = 0;
	virtual void OnPlaylistChanged(int size) = 0;
	virtual void OnRatingChanged(int iRating) = 0;
};

#endif