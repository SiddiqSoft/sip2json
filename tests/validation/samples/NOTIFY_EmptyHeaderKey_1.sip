NOTIFY sip:user.nm@example.net SIP/2.0
Via: SIP/2.0/tcp localhost:8443
Via: SIP/2.0/tcp localhost:8443;branch=user.nm@example.net__eDial_sep__user.nm@example.net
Call-ID: callid002100000000000000000
CSeq: 8 NOTIFY
From: sip:user.nm@example.net;pool=il-ed-mara;box=il-ed-mara-11.example.net;tag=12345678
To: "nm" <sip:user.nm@example.net>
Contact: <sip:localhost:8443;transport=ssl>
X-slave-site: localhost
X-last-change: 1593722124
X-drop-with-leader: false
X-Conf_no: 42121
X-no-unmute: false
X-rollcall: disabled
X-Call-URL: true
Warning: X-200 From SUBSCRIBE
X-control-master: user.nm@example.net
X-domain: DEFAULT
X-client-address: 127.0.0.1
X-From: user.nm@example.net
X-legs-on-server: 242
X-Sticky: 1
X-rss-id: 
X-start-muted: false
X-Video-SingleView: -1
X-suppress-system-im: false
X-restrict-notify: false
X-notify-im: false
X-no-audio: false
X-no-video: false
X-dialout: allowed
X-Billing-code-required: true
X-recording-enabled: true
X-Video-UsingMCU: false
X-subject: Test ops' Meeting Room
X-project-code: None
X-reserved-ports: 0
X-restrict-participants: false
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDcuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 2
X-Call-Start-Time: 1593722118.303436
Content-Length: 735
Content-type: application/sdp

v=0
o=user.nm@example.net 119331994474585 848356909 IN IP4 il-ed-mara-01.example.net
s=
i=+4555000002 (+4555000002) CallByPhone-URL
c=TN RFC2543 +4555000002
t=3802710924 0
m=audio 1 voice -
a=status:(203) answered  
a=mediastatus:sendrecv
a=cdr_start_time:1593722110.0
a=leg_no:1
a=remote:192.0.2.21:12128
a=flags:1049634
a=server:il-ed-mara-11.example.net
a=clir:false
a=far_end:192.0.2.73:11352
a=acs_guid:002075087451
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:8777464263@192.0.2.21
a=audio_payload:PCMU
a=access_code:0000001
a=privs:participant
a=ivr:dialin
a=creator:user.nm@example.net
a=dialin:0000001:6660016859@192.0.2.73-$$-1
a=dest_phone:8777464263
a=user-agent:Cisco-SIPGateway/IOS-16.3.3
