NOTIFY sip:subscribe_to_call_events@loopup.com;machine SIP/2.0
Via: SIP/2.0/tcp localhost:8443
Via: SIP/2.0/TCP localhost:780;branch=matthew.gabbard@stblaw.com__eDial_sep__subscribe_to_call_events@loopup.com;received=127.0.0.1:44350:1752919488
Via: SIP/2.0/TCP localhost:780;branch=matthew.gabbard@stblaw.com__eDial_sep__matthew.gabbard@stblaw.com;received=127.0.0.1:48606:20570384
Call-ID: 119035121230567il-ed-mara-01
CSeq: 49 NOTIFY
From: sip:matthew.gabbard@stblaw.com;pool=il-ed-mara;box=il-ed-mara-01.ring2.com
To: "matthew.gabbard" <sip:matthew.gabbard@stblaw.com>
Contact: sip:localhost:8443;pool=il-ed-mara;box=il-ed-mara-02.ring2.com
X-max-leg-no: 5
X-CallEvent: CallEnd
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1593637170
X-Sticky: 1
X-Conf_no: 32667
X-restrict-participants: false
X-no-unmute: false
X-rollcall: disabled
X-Call-URL: true
X-control-master: matthew.gabbard@stblaw.com
X-domain: DEFAULT
X-client-address: 10.1.206.91
X-From: matthew.gabbard@stblaw.com
X-legs-on-server: 286
X-rss-id: 
X-start-muted: false
X-Video-SingleView: 0
X-suppress-system-im: false
X-restrict-notify: false
X-notify-im: false
X-no-audio: false
X-no-video: false
X-dialout: allowed
X-Billing-code-required: true
X-recording-enabled: true
X-leader-required: true
X-Video-UsingMCU: false
X-subject: Matt Gabbard's Meeting Room
X-Call-Instance-ID: MTE5MDM1MTIxMjMwNTY3aWwtZWQtbWFyYS0wMToxNTkzNjM3MTcwOjE1Njc0Ng==
X-Route-ID: 2
X-Call-Start-Time: 1593637170.156760
Content-Length: 1326
Content-type: application/sdp

v=0
o=matthew.gabbard@stblaw.com 3598380125 241 IN IP4 il-ed-mara-01.ring2.com
s=Matt Gabbard's Meeting Room
i="Matt%20Gabbard%20-%20" (+12124553521) CallByPhone-URL
c=TN RFC2543 +12124553521
t=3802625984 3802626770
m=audio 1 voice -
a=status:(4) dropped
a=mediastatus:sendrecv
a=cdr_start_time:1593637184.0
a=cause: (20001) Normal Hangup - Received SIP BYE From: 12124553521@216.111.92.73
a=leg_no:2
a=remote:216.111.92.36:12298
a=flags:1054242
a=new_change
a=server:il-ed-mara-06.ring2.com
a=clir:false:18777464263
a=far_end:216.111.92.73:33576
a=acs_guid:000414323482
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:matthew.gabbard@stblaw.com
a=legCallid:646390624@216.111.92.36
a=audio_payload:PCMU
a=preferred_identity:18777464263
a=x-ring2-callowner-login_alias:matthew.gabbard@stblaw.com
a=x-ring2-legowner-login_alias:matthew.gabbard@stblaw.com
a=x-ring2-alt-proj: 
a=x-ring2-call_source_type:16
a=x-ring2-displayname:Matt%20Gabbard%20-%20
a=x-ring2-displayname_type:1
a=x-ring2-clientaddress:74.72.152.187:47553
a=x-ring2-unique_device_id:c4d5d813037e58bbf10cf6de936068e0e99e93f0b38cc37bdd421b6a52e2e68b
a=x-ring2-UF1: 
a=ivr:dialout
a=dialout:click_in
a=access_code:2997255
a=privs:leader
a=x-ring2-smartproxy:usaze-asalt01.ring2.com
a=user-agent:Cisco-SIPGateway/IOS-16.3.3
