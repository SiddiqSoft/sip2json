NOTIFY sip:subscribe_to_call_events@example.com;machine SIP/2.0
Via: SIP/2.0/tcp localhost:8443
Via: SIP/2.0/UDP localhost:780;branch=jrbirge@nscorp.com__eDial_sep__subscribe_to_call_events@example.com;received=localhost
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__jrbirge@nscorp.com;received=127.0.0.1:56466:1752919488
Via: SIP/2.0/TCP 127.0.0.1:52936;received=127.0.0.1:52936:28537344
Call-ID: callid00210000000000000000
CSeq: 267 NOTIFY
From: sip:conference.wizard@local.host;pool=il-ed-mara;box=il-ed-mara-13.example.net
To: "jrbirge" <sip:jrbirge@nscorp.com>
Contact: <sip:localhost:8443;transport=ssl>
X-CallEvent: LegDrop
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1593546087
X-drop-with-leader: false
X-Conf_no: 20232
X-no-unmute: false
X-rollcall: disabled
X-Call-URL: true
X-control-master: jrbirge@nscorp.com
X-domain: DEFAULT
X-client-address: 127.0.0.1
X-From: jrbirge@nscorp.com
X-legs-on-server: 411
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
X-Billing-code-required: false
X-recording-enabled: true
X-leader-required: true
X-no-entrance-tones: true
X-Video-UsingMCU: false
X-subject: Operations Call
X-reserved-ports: 0
X-restrict-participants: false
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMTAuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 2
X-Call-Start-Time: 1593545065.804480
Content-Length: 835
Content-type: application/sdp

v=0
o=jrbirge@nscorp.com 148492049389635 847595153 IN IP4 il-ed-mara-01.example.net
s=
i="Cell Phone   GA" (+4555000001) CallByPhone-URL
c=TN RFC2543 +4555000001
t=3802534341 3802534887
m=audio 1 voice -
a=status:(4) dropped
a=mediastatus:sendrecv
a=cdr_start_time:1593545529.0
a=cause: (20001) Normal Hangup - Received SIP BYE From: 4044166441@192.0.2.73
a=leg_no:24
a=remote:192.0.2.9:12806
a=flags:1049634
a=new_change
a=server:il-ed-mara-13.example.net
a=clir:false
a=far_end:192.0.2.73:27164
a=acs_guid:001339764317
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:8666726771@192.0.2.9
a=audio_payload:PCMU
a=access_code:0000016
a=privs:participant
a=ivr:dialin
a=creator:jrbirge@nscorp.com
a=dialin:0000016:6660018247@192.0.2.73-$$-1
a=dest_phone:8666726771
a=user-agent:Cisco-SIPGateway/IOS-16.3.3
