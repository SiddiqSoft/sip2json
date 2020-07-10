NOTIFY sip:subscribe_to_call_events@loopup.com;machine SIP/2.0
Via: SIP/2.0/tcp localhost:8443
Via: SIP/2.0/UDP localhost:780;branch=akirmayer@sidley.com__eDial_sep__subscribe_to_call_events@loopup.com;received=localhost
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__akirmayer@sidley.com;received=127.0.0.1:57400:1752919488
Via: SIP/2.0/TCP 127.0.0.1:46942;received=127.0.0.1:46942:36406400
Call-ID: 15105076141563il-ed-mara-01
CSeq: 2 NOTIFY
From: sip:conference.wizard@local.host;pool=hk-ed-pearl;box=hk-ed-pearl-01.ring2.com
To: "akirmayer" <sip:akirmayer@sidley.com>
Contact: <sip:localhost:8443;transport=ssl>
X-CallEvent: LegAdd
X-slave-site: localhost
X-last-change: 1593567745
X-drop-with-leader: false
X-Conf_no: 22977
X-no-unmute: false
X-rollcall: disabled
X-Call-URL: true
X-control-master: akirmayer@sidley.com
X-domain: DEFAULT
X-client-address: 127.0.0.1
X-From: akirmayer@sidley.com
X-legs-on-server: 138
X-Sticky: 1
X-rss-id: 
X-start-muted: false
X-Video-SingleView: 0
X-suppress-system-im: false
X-restrict-notify: false
X-notify-im: false
X-no-audio: false
X-no-video: false
X-dialout: allowed
X-Billing-code-required: false
X-recording-enabled: true
X-leader-required: true
X-Video-UsingMCU: false
X-subject: Asi Kirmayer's Meeting Room
X-reserved-ports: 0
X-restrict-participants: false
X-Call-Instance-ID: MTUxMDUwNzYxNDE1NjNpbC1lZC1tYXJhLTAxOjE1OTM1NjQxNjc6Mjg0NDcw
X-Route-ID: 2
X-Call-Start-Time: 1593564167.284484
Content-Length: 783
Content-type: application/fail-sdp

v=0
o=akirmayer@sidley.com 55706299459030 847687142 IN IP4 il-ed-mara-01.ring2.com
s=
i=+6568898813 (+6568898813) CallByPhone-URL
c=TN RFC2543 +6568898813
t=3802556545 0
m=audio 1 voice -
a=status:(203) answered  
a=mediastatus:sendrecv
a=cdr_start_time:1593567734.0
a=leg_no:12
a=remote:205.252.237.71:12084
a=flags:1049632
a=new_change
a=server:hk-ed-pearl-01.ring2.com
a=clir:false
a=far_end:205.252.237.66:16224
a=conference:1
a=acs_guid:000799073288
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:6531583706@205.252.237.71
a=audio_payload:PCMA
a=access_code:2742801
a=privs:participant
a=ivr:dialin
a=creator:akirmayer@sidley.com
a=dialin:2742801:6660014385@205.252.237.66-$$-1
a=dest_phone:6531583706
a=user-agent:Cisco-SIPGateway/IOS-15.5.2.S4
