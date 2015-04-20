
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include "OnvifSDK.h"
#include "OnvifTestServer.h"
#include "EventsEmitter.h"

const std::string scopes =
"onvif://www.onvif.org/name/LprBox "
"onvif://www.onvif.org/hardware/LprBoxLittle "
"onvif://www.onvif.org/type/NetworkVideoTransmitter "
//"onvif://www.onvif.org/type/video_encoder "
//"onvif://www.onvif.org/type/audio_encoder "
"onvif://www.onvif.org/type/lpr "
"onvif://www.onvif.org/location/Hungary";

IOnvifServer * srv;
EventsEmitter evEmm;
OnvifTestServer handler;

void sig_handler(int signo)
{
    evEmm.stop();
    deleteOnvifServer(srv);
    exit(0);
}

int main()
{
    signal(SIGKILL, sig_handler);
    signal(SIGSTOP, sig_handler);
    signal(SIGINT,  sig_handler);
    signal(SIGTERM, sig_handler);

    srv = getOnvifServer();
    if( srv->SetDeviceInfo( OnvifDevice::NVT, "WBS",
                            "Model No 011", "1.0", "734145",
                            "1.1", scopes, "eth0", 8080 ) != 0 )
        return -1;

    if( !evEmm.run() )
        return -1;

    OnvifHandlers providedHandlers;
    memset( providedHandlers.h_, 0, sizeof(providedHandlers.h_) );
    providedHandlers.h_[OnvifService::DEV] = &handler;
    providedHandlers.h_[OnvifService::EVNT] = &handler;


    if( srv->Init( providedHandlers ) != 0 )
        return -1;

    if( srv->Run() != 0 )
        return -1;

    return 0;
}
