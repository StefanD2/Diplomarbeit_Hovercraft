#include "JetiBase.hpp"

JetiBase::JetiBase()
{
    // intialize variables
    recieve_buffer_write = 0;
    recieve_buffer_read = 0;

    recieving = false;
    sendDone = false;

    buttonL = false;
    buttonR = false;
    buttonD = false;
    buttonU = false;
    send_count = 0;

    recieving_msg = "";
    newMsgString = "";
}

void JetiBase::loop()
{
    // recieving
    for (; recieve_buffer_read != recieve_buffer_write; recieve_buffer_read = (recieve_buffer_read + 1) & (RECIEVE_BUFFER_SIZE - 1))
    {
        if (!recieve_buffer[recieve_buffer_read].bit9 && recieving_msg != "")
        { // end of msg
            newMsgString = recieving_msg;
            recieving_msg = "";
            recieving = false;
        }
        else
        {
            recieving_msg += (char)recieve_buffer[recieve_buffer_read].databyte;
            recieving = true;
        }
    }

    // sending
    if (send_count && !recieving)
    {
        send_init();
        _delay_ms(4); // delay for stability reasons
    }
    if (sendDone)
    {
        send_done();
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
    send_count = ((count > 0) ? (count) : 1); // send_count must be 1 ore higher
}

bool JetiBase::isNewMsg()
{
    return (newMsgString != "");
}

String JetiBase::getMsg()
{
    String ret = newMsgString;
    newMsgString = "";
    return ret;
}

jetiTelemetry_t JetiBase::getTelemetry(String msg)
{
	String voltage=msg.substring(17,22);
    voltage.replace(",",".");
    return {.mode = (char)msg.charAt(1), .percent = (int)msg.substring(2, 5).toInt(), .rpm = msg.substring(10, 14).toInt(), .voltage = voltage.toDouble(), .temperature = (int)msg.substring(29, 31).toInt()};
}
