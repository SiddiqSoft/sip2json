SIP/2.0 401 Unauthorized
Via: SIP/2.0/TCP il-ed-aras-01.example-corp.com:8443
Call-ID: call-id-0004.00000000000server01.example.net
CSeq: 1 REGISTER
From: sip:no_such_user_exists@example.com
To: <sip:no_such_user_exists@example.com>;tag=12345678
Contact: <sip:198.51.100.71:8443;transport=ssl>;expires=360;tag=14084__fenway.example-corp.com
Expires: 360
Authorization: X-Signed start="1595022925",expire="1595023285",user="no_such_user_exists@example.com",confwiz="my string",nsadrs="il-ed-aras-01.example-corp.com",signed="1d588b59eead861962c7b0d3a8ff147b"
User-Agent: Example eDial ACS 9.1.0b8071
WWW-Authenticate: Basic realm=eDial
X-domain: DEFAULT
Date: Fri, 17 Jul 2020 21:55:25 GMT
Content-Length: 0

