REGISTER sip:subscribe_to_call_events@example.com SIP/2.0
Via: SIP/2.0/TCP il-ed-mara-01.example.net:8443
To: sip:subscribe_to_call_events@example.com
From: sip:subscribe_to_call_events@example.com
Call-ID: 00000001-0000-4000-8000-000000000000
CSeq: 65 REGISTER
Contact: sip:subscribe_to_call_events@example.com;expires=300;tag=8176__PMD4-US
Authorization: X-Signed start="1593567777",expire="1593568137",user="subscribe_to_call_events@example.com",confwiz="my string",nsadrs="il-ed-mara-01.example.net",signed="2c7c7247d66565c8f41985a234b5fe79"
User-Agent: r2ediallib-192.0.2.1/webclient.URL
Content-Type: application/sdp
Expires: 300
X-subscribe-to-leg-events: true
Content-Length: 0

