NOTIFY sip:subscribe_to_call_events@example.com;machine SIP/2.0
Via: SIP/2.0/tcp localhost:8443
Via: SIP/2.0/UDP localhost:780;branch=hbcdhvbdfh67@example.org__eDial_sep__subscribe_to_call_events@example.com;received=localhost
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__hbcdhvbdfh67@example.org;received=127.0.0.1:51204:1107030160
Via: SIP/2.0/TCP 127.0.0.1:33942;received=127.0.0.1:33942:12280832
Call-ID: callid00220000000000000000
CSeq: 1 NOTIFY
From: sip:conference.wizard@local.host;pool=il-ed-aras;box=il-ed-aras-05.example-corp.com
To: "hbcdhvbdfh67" <sip:hbcdhvbdfh67@example.org>
Contact: <sip:localhost:8443;transport=ssl>
X-CallEvent: CallStart
X-slave-site: localhost
X-last-change: 1596099474
X-drop-with-leader: false
X-Conf_no: 922
X-no-unmute: false
X-rollcall: disabled
X-Call-URL: true
X-control-master: hbcdhvbdfh67@example.org
X-domain: DEFAULT
X-client-address: 127.0.0.1
X-From: hbcdhvbdfh67@example.org
X-legs-on-server: 1
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
X-subject: hdsbfsjh jcbdhv's Meeting Room
X-project-code: None
X-department-code: retail
X-reserved-ports: 0
X-restrict-participants: false
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMTIuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 2
Content-Length: 792
Content-type: application/sdp

v=0
o=hbcdhvbdfh67@example.org 95023660363472 846985136 IN IP4 il-ed-aras-01.example-corp.com
s=
i=+15550000020 (+15550000020) CallByPhone-URL
c=TN RFC2543 +15550000020
t=3805088241 0
m=audio 1 voice -
a=status:(2) starting
a=mediastatus:sendrecv
a=leg_no:1
a=remote:198.51.100.73:12000
a=flags:1053728
a=new_change
a=server:il-ed-aras-05.example-corp.com
a=clir:false
a=far_end:192.0.2.7:35186
a=acs_guid:000923892239
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:14155500008@198.51.100.73
a=audio_payload:PCMU
a=local_mos:42
a=cdr_start_time:1596099441.799922
a=access_code:0000011
a=privs:leader
a=ivr:dialin
a=creator:hbcdhvbdfh67@example.org
a=dialin:0000011:6660016205@192.0.2.7-$$-1
a=dest_phone:14155500008
a=user-agent:Cisco-SIPGateway/IOS-15.5.3.S5
