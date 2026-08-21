SIP/2.0 200 OK
Via: SIP/2.0/TCP il-ed-mara-01.example.net:8443
Call-ID: 00000001-0000-4000-8000-000000000000
CSeq: 65 REGISTER
From: sip:subscribe_to_call_events@example.com
To: <sip:subscribe_to_call_events@example.com>;tag=12345678
Contact: <sip:192.0.2.37:8443;transport=ssl>;expires=300;tag=8176__PMD4-US
Expires: 300
Authorization: X-Signed start="1593567777",expire="1593568137",user="subscribe_to_call_events@example.com",confwiz="my string",nsadrs="il-ed-mara-01.example.net",signed="2c7c7247d66565c8f41985a234b5fe79"
User-Agent: Example eDial ACS 9.1.0b8067
X-Seamless: PRESENCE-ALT
X-domain: DEFAULT
X-subscribe-to-leg-events: true
Content-Length: 0

