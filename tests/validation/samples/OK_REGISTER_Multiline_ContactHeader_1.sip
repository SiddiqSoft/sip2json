===========================================================================================
2020-07-02 20:27:49.014  -- SmartIOGulam() [tid:5016]
---------------------------------------------------------------------------------
SIP/2.0 200 OK
Via: SIP/2.0/TCP 127.0.0.1:62338
Call-ID: callid002000000000000000000
CSeq: 1 REGISTER
From: sip:user.jc@example.com
To: <sip:user.jc@example.com>;tag=12345678
Contact: sip:user.jc@example.com;expires=1593725109;tag=sp2(263988)_IL-PS-CONGO-02.example.net,
	 sip:user.jc@example.com;expires=1593725109;tag=sp2(26392)_IL-PS-CONGO-01.example.net,
	 sip:user.jc@example.com;expires=1593725269;tag=callid002000000000000000000__sp3[USCHEQ-ASRTA01.example.net]
Authorization: X-Signed start="1593721668",expire="1593725268",user="user.jc@example.com",confwiz="my string",nsadrs="il-ed-mara-01.example.net",signed="9ecb18b94f0d25fadb4cabe9d469a5fb"
User-Agent: Example eDial ACS 9.1.0b8040
X-domain: DEFAULT
Content-Length: 0

SIP/2.0 200 OK
Via: SIP/2.0/tcp user.jc@example.com@example.com:0
Call-ID: callid002000000000000000000
CSeq: 848352898 SUBSCRIBE
From: sip:user.jc@example.com
To: <sip:conference.wizard@local.host>;tag=12345678
Contact: <sip:192.0.2.37:8443;transport=ssl>
Authorization: Basic HelloSailor
User-Agent: Example eDial ACS 9.1.0b8052
X-Seamless: SUBSCRIBE
X-domain: DEFAULT
Content-Length: 0

SIP/2.0 200 OK
Via: SIP/2.0/TCP 127.0.0.1:62338
Call-ID: callid002000000000000000000
CSeq: 1 REGISTER
From: sip:user.jc@example.com
To: <sip:user.jc@example.com>;tag=12345678
Contact: <sip:192.0.2.37:8443;transport=ssl>;expires=3600;tag=callid002000000000000000000__sp3[USCHEQ-ASRTA01.example.net],
	 
User-Agent: Example eDial ACS 9.1.0b8067
uthorization: X-Signed start="1593721668",expire="1593725268",user="user.jc@example.com",confwiz="my string",nsadrs="il-ed-mara-01.example.net",signed="9ecb18b94f0d25fadb4cabe9d469a5fb"
xpires: 360
X-Seamless: PRESENCE-ALT
X-domain: DEFAULT
Content-Length: 0

