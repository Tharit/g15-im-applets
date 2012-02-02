#ifndef _CCONTACT_H_
#define _CCONTACT_H_

struct SMessage
{
	tm Time;
	bool bIsMe;
	tstring strMessage;
	int iConnectionID;
};

class CContactData
{
public:
	//int iConnectionID;
	tstring strMedium;
	tstring strDisplayName;
	tstring strID;
	tstring strStatus;
	tstring strGroup;
	tstring strSection;
	int iWindowID;
	int iConnectionID;
	bool bIsSubContact;

	list<SMessage> LMessages;
};

#endif