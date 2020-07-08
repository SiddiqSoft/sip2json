NOTIFY sip:nm@ring2.com SIP/2.0
Via: SIP/2.0/tcp localhost:8443
Via: SIP/2.0/tcp localhost:8443;branch=nm@ring2.com__eDial_sep__nm@ring2.com
Call-ID: 46570689829320il-ed-mara-01
CSeq: 8 NOTIFY
From: sip:nm@ring2.com;pool=il-ed-mara;box=il-ed-mara-11.ring2.com;tag=12345678
To: "nm" <sip:nm@ring2.com>
Contact: <sip:localhost:8443;transport=ssl>
X-slave-site: localhost
X-last-change: 1593722124
X-drop-with-leader: false
X-Conf_no: 42121
X-no-unmute: false
X-rollcall: disabled
X-Call-URL: true
Warning: X-200 From SUBSCRIBE
X-control-master: nm@ring2.com
X-domain: DEFAULT
X-client-address: 127.0.0.1
X-From: nm@ring2.com
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
X-Call-Instance-ID: NDY1NzA2ODk4MjkzMjBpbC1lZC1tYXJhLTAxOjE1OTM3MjIxMTg6MzAzNDA2
X-Route-ID: 2
X-Call-Start-Time: 1593722118.303436
Content-Length: 729
Content-type: application/sdp

v=0
o=nm@ring2.com 119331994474585 848356909 IN IP4 il-ed-mara-01.ring2.com
s=
i=+4159905606 (+4159905606) CallByPhone-URL
c=TN RFC2543 +4159905606
t=3802710924 0
m=audio 1 voice -
a=status:(203) answered  
a=mediastatus:sendrecv
a=cdr_start_time:1593722110.0
a=leg_no:1
a=remote:216.111.92.21:12128
a=flags:1049634
a=server:il-ed-mara-11.ring2.com
a=clir:false
a=far_end:216.111.92.73:11352
a=acs_guid:002075087451
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:8777464263@216.111.92.21
a=audio_payload:PCMU
a=access_code:0249716
a=privs:participant
a=ivr:dialin
a=creator:nm@ring2.com
a=dialin:0249716:6660016859@216.111.92.73-$$-1
a=dest_phone:8777464263
a=user-agent:Cisco-SIPGateway/IOS-16.3.3
