#include "JetiBase.hpp"

JetiBase::JetiBase()
{
    msg = 0;
    start = 0;
    msgb = 0;
    recieving = 0;
    sendDone = 0;
    buttonL = 0;
    buttonR = 0;
    buttonD = 0;
    buttonU = 0;
    sendb = 0;
    content = "";
    newMsgString = "";
    newMsg = 0;
    adding = 0;
}

void JetiBase::loop()
{
    if (msgb)
    {
        if (!start)
        {
            if (adding)
            {
                _delay_ms(3); // remove delay
                newMsg = 1;
                newMsgString = content;
                recieving = false;
            }
            content = "";
            adding = false;
        }
        else
        {
            content += (char)msg;
            adding = true;
        }
        msgb = false;
    }
    if (sendb && !recieving)
    {
        csend();
        _delay_ms(4); // remove delay
        //sendb = false;
    }
    if (sendDone)
    {
        actualsendDone();
        sendDone = false;
    }
}

void JetiBase::send(bool l, bool r, bool u, bool d)
{
    send(l, r, u, d, 1);
}

void JetiBase::send(bool l, bool r, bool u, bool d, int count)
{
    buttonL = l;
    buttonR = r;
    buttonU = u;
    buttonD = d;
    sendb = ((count > 0) ? (count) : 1);
}

bool JetiBase::isNewMsg()
{
    return newMsg;
}

String JetiBase::getMsg()
{
    newMsg = 0;
    return newMsgString;
}
