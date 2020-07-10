NOTIFY sip:nm@ring2.com SIP/2.0
Via: SIP/2.0/tcp localhost:8443
Via: SIP/2.0/tcp localhost:8443;branch=nm@ring2.com__eDial_sep__nm@ring2.com
Call-ID: 46570689829320il-ed-mara-01
CSeq: 10 NOTIFY
From: sip:nm@ring2.com;pool=il-ed-mara;box=il-ed-mara-11.ring2.com;tag=12345678
To: "nm" <sip:nm@ring2.com>
Contact: <sip:localhost:8443;transport=ssl>
X-CallEvent: LegAdd
X-slave-site: localhost
X-last-change: 1593722333
X-drop-with-leader: false
X-Conf_no: 42121
X-no-unmute: false
X-rollcall: disabled
X-Call-URL: true
X-control-master: nm@ring2.com
X-domain: DEFAULT
X-client-address: 127.0.0.1
X-From: nm@ring2.com
X-legs-on-server: 243
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
Content-Length: 741
Content-type: application/sdp

v=0
o=sip:nm@ring2.com 1445714250 1445714364 IN IP4 il-ed-mara-01.ring2.com
s=Playback-46570689829320il-ed-mara-01
i=PlaybackLeg (target-legid 1) CallByPhone
c=IN IP4 127.0.0.1
t=3802711133 0
m=audio 1 voice x-play
a=status:(203) answered  playback
a=mediastatus:sendonly
a=cdr_start_time:1593722333.0
a=leg_no:2
a=flags:1049088
a=xflags:1
a=new_change
a=server:il-ed-mara-11.ring2.com
a=clir:false
a=fmtp:x-play uri:http://10.254.254.2/slides/ring2sys_USA_msg_DialinDropParticipant_403/recording.wav
a=machine_leg:1
a=conference:1
a=heard_in_leg:1
a=acs_guid:002075087451
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:nm@ring2.com
a=heard_by_leg:1
a=x-ring2-smartproxy:USCHEQ-ASRTA01.ring2.com
a=x-ring2-UF1:1
NOTIFY sip:nm@ring2.com SIP/2.0
Via: SIP/2.0/tcp localhost:8443
Via: SIP/2.0/tcp localhost:8443;branch=nm@ring2.com__eDial_sep__nm@ring2.com
Call-ID: 46570689829320il-ed-mara-01
CSeq: 11 NOTIFY
From: sip:nm@ring2.com;pool=il-ed-mara;box=il-ed-mara-11.ring2.com;tag=12345678
To: "nm" <sip:nm@ring2.com>
Contact: <sip:localhost:8443;transport=ssl>
X-CallEvent: LegDrop
X-slave-site: localhost
X-last-change: 1593722334
X-drop-with-leader: false
X-Conf_no: 42121
X-no-unmute: false
X-rollcall: disabled
X-Call-URL: true
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
Content-Length: 784
Content-type: application/sdp

v=0
o=sip:nm@ring2.com 1445714250 1445714375 IN IP4 il-ed-mara-01.ring2.com
s=Playback-46570689829320il-ed-mara-01
i=PlaybackLeg (target-legid 1) CallByPhone
c=IN IP4 127.0.0.1
t=3802711133 3802711134
m=audio 1 voice x-play
a=status:(4) dropped
a=mediastatus:sendonly
a=cdr_start_time:1593722333.0
a=cause: (20025) Recording playback completed
a=leg_no:2
a=flags:1049090
a=xflags:1
a=new_change
a=server:il-ed-mara-11.ring2.com
a=clir:false
a=fmtp:x-play uri:http://10.254.254.2/slides/ring2sys_USA_msg_DialinDropParticipant_403/recording.wav
a=machine_leg:1
a=conference:1
a=heard_in_leg:1
a=acs_guid:002075087451
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:nm@ring2.com
a=heard_by_leg:1
a=x-ring2-smartproxy:USCHEQ-ASRTA01.ring2.com
a=x-ring2-UF1:1
