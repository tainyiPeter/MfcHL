#pragma once

//#include "Responser.h"
//#include "libHttp/HttpLib.h"


//#include "Timer/YKTimer.h"
//#include "IWebRequestObserver.h"
//#include "YKHttpRequestCache.h"
//#include "TradeDefne.h"
//#include <vector>
//#include <tuple>
//#include "common.h"

//struct YKProxyServer;
//
//static std::string GetHttpHeadClientType();
//static std::string GetHttpHeadSoftName();

/** http ������ʵ��
*/
class YKHttpRequestHandler //: public IHttpSupport
{
public:
    void HttpReqTest();

protected:

    /** �ڲ�����
    */
    struct YKInner;
	YKInner* m_pInner;
};