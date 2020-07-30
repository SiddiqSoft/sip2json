NOTIFY sip:subscribe_to_call_events@loopup.com;machine SIP/2.0
Via: SIP/2.0/tcp localhost:8443
Via: SIP/2.0/UDP localhost:780;branch=hbcdhvbdfh67@loopup.co__eDial_sep__subscribe_to_call_events@loopup.com;received=localhost
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__hbcdhvbdfh67@loopup.co;received=127.0.0.1:51204:1107030160
Via: SIP/2.0/TCP 127.0.0.1:33942;received=127.0.0.1:33942:12280832
Call-ID: 7241263816357il-ed-aras-01
CSeq: 1 NOTIFY
From: sip:conference.wizard@local.host;pool=il-ed-aras;box=il-ed-aras-05.ring2-corp.com
To: "hbcdhvbdfh67" <sip:hbcdhvbdfh67@loopup.co>
Contact: <sip:localhost:8443;transport=ssl>
X-CallEvent: CallStart
X-slave-site: localhost
X-last-change: 1596099474
X-drop-with-leader: false
X-Conf_no: 922
X-no-unmute: false
X-rollcall: disabled
X-Call-URL: true
X-control-master: hbcdhvbdfh67@loopup.co
X-domain: DEFAULT
X-client-address: 127.0.0.1
X-From: hbcdhvbdfh67@loopup.co
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
X-Call-Instance-ID: NzI0MTI2MzgxNjM1N2lsLWVkLWFyYXMtMDE6MTU5NjA5OTQ3NDo2NDE0NTM=
X-Route-ID: 2
Content-Length: 790
Content-type: application/sdp

v=0
o=hbcdhvbdfh67@loopup.co 95023660363472 846985136 IN IP4 il-ed-aras-01.ring2-corp.com
s=
i=+19253230928 (+19253230928) CallByPhone-URL
c=TN RFC2543 +19253230928
t=3805088241 0
m=audio 1 voice -
a=status:(2) starting
a=mediastatus:sendrecv
a=leg_no:1
a=remote:67.131.238.73:12000
a=flags:1053728
a=new_change
a=server:il-ed-aras-05.ring2-corp.com
a=clir:false
a=far_end:216.111.92.3:35186
a=acs_guid:000923892239
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:14154498508@67.131.238.73
a=audio_payload:PCMU
a=local_mos:42
a=cdr_start_time:1596099441.799922
a=access_code:2331231
a=privs:leader
a=ivr:dialin
a=creator:hbcdhvbdfh67@loopup.co
a=dialin:2331231:6660016205@216.111.92.3-$$-1
a=dest_phone:14154498508
a=user-agent:Cisco-SIPGateway/IOS-15.5.3.S5
