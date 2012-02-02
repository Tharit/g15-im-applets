#ifndef _CEVENT_H_
#define _CEVENT_H_

#include "CContactData.h"

enum EventType {	EVENT_MSG_RECEIVED = 0,
					EVENT_MSG_SENT = 3,
					EVENT_STATUS = 1,
					
					EVENT_URL = 5,
					EVENT_FILE = 6,
					EVENT_AUTHREQUEST = 7,
					EVENT_CONTACTS = 8,
					EVENT_ADDED = 9,

					EVENT_CONTACT_ADDED = 10,
					EVENT_CONTACT_DELETED = 11,
					EVENT_CONTACT_NICK = 12,
					EVENT_CONTACT_HIDDEN = 13,
					EVENT_CONTACT_GROUP = 14,

					EVENT_SIGNED_ON = 15,
					EVENT_SIGNED_OFF = 16,

					EVENT_PROTO_STATUS = 17,
					EVENT_PROTO_CONNECTED = 18,
					EVENT_PROTO_DISCONNECTED = 19,

					EVENT_TYPING_NOTIFICATION = 20,
					EVENT_MESSAGEWINDOW = 21};

#define MSG_READ 0
#define MSG_UNREAD 1 

class CEvent 
{
public:
	CEvent()
	{
		bTime = false;

		pContact = NULL;

		dwFlags = NULL;
		
		bNotification = false;
		strDescription = _T("");
	
		iValue = NULL;
		hValue = NULL;
		strValue = _T("");
		strValue2 = _T("");
		strSection = _T("");
	}

	enum EventType	eType;
	DWORD dwFlags;
	CContactData *pContact;
	int iConnectionID;
	tstring strSection;
	tm		Time;
	bool	bTime;

	bool bNotification;
	bool bLog;
	tstring strDescription;

	HANDLE hValue;
	int iValue;
	tstring strValue;
	tstring strValue2;
	
};

#endif