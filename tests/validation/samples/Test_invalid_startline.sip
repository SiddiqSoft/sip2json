NOTiFY sip:subscribe_to_call_events@example.com;machine SIP/2.1
Via: SIP/2.0/tcp localhost:8443
Via: SIP/2.0/UDP localhost:780;branch=user.ak@example.com__eDial_sep__subscribe_to_call_events@example.com;received=localhost
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__user.ak@example.com;received=127.0.0.1:57400:1752919488
Via: SIP/2.0/TCP 127.0.0.1:46942;received=127.0.0.1:46942:36406400
Call-ID: callid001200000000000000000
CSeq: 2 NOTIFY
From: sip:conference.wizard@local.host;pool=hk-ed-pearl;box=hk-ed-pearl-01.example.net
To: "akirmayer" <sip:user.ak@example.com>
Contact: <sip:localhost:8443;transport=ssl>
X-CallEvent: LegAdd
X-slave-site: localhost
X-last-change: 1593567745
X-drop-with-leader: false
X-Conf_no: 22977
X-no-unmute: false
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.ak@example.com
X-domain: DEFAULT
X-client-address: 127.0.0.1
X-From: user.ak@example.com
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
X-subject: User One's Meeting Room
X-reserved-ports: 0
X-restrict-participants: false
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDUuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 2
X-Call-Start-Time: 1593564167.284484
Content-Length: 781
Content-type: application/sdp

v=0
o=user.ak@example.com 55706299459030 847687142 IN IP4 il-ed-mara-01.example.net
s=
i=+6555000003 (+6555000003) CallByPhone-URL
c=TN RFC2543 +6555000003
t=3802556545 0
m=audio 1 voice -
a=status:(203) answered  
a=mediastatus:sendrecv
a=cdr_start_time:1593567734.0
a=leg_no:12
a=remote:198.51.100.71:12084
a=flags:1049632
a=new_change
a=server:hk-ed-pearl-01.example.net
a=clir:false
a=far_end:198.51.100.66:16224
a=conference:1
a=acs_guid:000799073288
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:6531583706@198.51.100.71
a=audio_payload:PCMA
a=access_code:0000014
a=privs:participant
a=ivr:dialin
a=creator:user.ak@example.com
a=dialin:0000014:6660014385@198.51.100.66-$$-1
a=dest_phone:6531583706
a=user-agent:Cisco-SIPGateway/IOS-15.5.2.S4
